#include "http/httpserver.h"
#include "http/webhandlers.h"
#include "util/trace.h"
#include "util/printbuffer.h"
#include "i2c/i2c.h"
#include "util/arduino_stl_support.h"
#include "motors_i2c_model.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <StreamString.h>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdio.h>



int snprintf( char * s, size_t n, const char * format, ... );



namespace http
{

static const char* CONTENT_TYPE_JSON = "application/json";

#define ACCESS_CONTROL_ALLOW_ORIGIN "Access-Control-Allow-Origin"
#define ACCESS_CONTROL_REQUEST_HEADERS "Access-Control-Request-Headers"
#define ACCESS_CONTROL_REQUEST_METHOD "Access-Control-Request-Method"
#define ACCESS_CONTROL_ALLOW_HEADERS "Access-Control-Allow-Headers"
#define ACCESS_CONTROL_ALLOW_METHODS "Access-Control-Allow-Methods"


template <typename T>
static void sendBasicJson( AsyncWebServerRequest* request, T& content )
{
	AsyncResponseStream *response = request->beginResponseStream(CONTENT_TYPE_JSON);
	content.printTo(*response);
	request->send(response);
}

static void sendNoContent( AsyncWebServerRequest* request )
{
	request->send(204);
}


static void sendError( AsyncWebServerRequest* request ) {
	String content = String("{\"error\":\"");
	while( !error::globalStack().empty() ) {
		const error::StackItem& item = error::globalStack().top();
		content.concat( item.file() );
		content.concat( "," );
		content.concat( item.line() );
		content.concat( ": " );
		content.concat( item.message().c_str() );
		error::globalStack().pop();
	}
	content.concat("\"}");

	request->send( 500, CONTENT_TYPE_JSON, content );
}

/************************************************************************/
// PidService
/************************************************************************/
PidService::PidService( AsyncWebServer* impl, const std::string& path,
						motion::PidEngine* pidEngine, io::Display* display ):
						m_impl(impl), m_pidEngine(pidEngine), m_display( display ) {

	AsyncMethodWebHandler<PidService>::Factory handlers(this, display);

	std::string statePath = path + "/state";
	m_impl->addHandler( handlers.create(statePath.c_str(), HTTP_GET, &PidService::handleState) );

	std::string settingsPath = path + "/settings";
	m_impl->addHandler( handlers.create(settingsPath.c_str(), HTTP_GET, &PidService::handleSettings) );

	m_impl->addHandler( handlers.create(settingsPath.c_str(), HTTP_PUT, &PidService::handleSetSettings) );
}


class PidStatesStream: public Stream {
public:
	PidStatesStream( std::unique_ptr<std::vector<::PIDStateEntry>> array ): m_array(std::move(array)) {
		m_length = calculateLength();
		m_read = 0;
		m_it = m_array->begin();
		m_currentSerialization = "[";
		m_currentSerializationPos = m_currentSerialization.begin();
	}

	int available() override {
		return m_length - m_read;
	}

	size_t write(uint8_t data) override {
		//Unsupported
		return 0;
	}

	int read() override {
		int ret = peek();
		if ( ret != -1 ) {
			++m_currentSerializationPos;
			++m_read;
		}
		return ret;
	}

	int peek() override {
		if ( m_currentSerializationPos == m_currentSerialization.end() ) {
			if ( m_it == m_array->end() ) {
				return -1;
			}
			serialize(*m_it);
			++m_it;

			char concatChar = (m_it == m_array->end()) ? ']' : ',';
			m_currentSerialization.concat(concatChar);
			m_currentSerializationPos = m_currentSerialization.begin();
		}

		return *m_currentSerializationPos;
	}

	void flush() override {}

private:
	int calculateLength() {
		int itemsSerializationSize = std::accumulate( m_array->begin(), m_array->end(), 0, [this](int acc, const ::PIDStateEntry& state) {
			serialize(state);
			return acc + m_currentSerialization.length();
		});
		return itemsSerializationSize + 1 + m_array->size();
	}

	void serialize( const ::PIDStateEntry& state ) {
		m_jsonBuffer.clear();
		JsonObject& entry = m_jsonBuffer.createObject();
		entry["i"] = state.index;
		entry["tar"] = state.state.target;
		entry["cur"] = state.state.current;
		entry["p_err"] = state.state.previous_error;
		entry["i_err"] = state.state.integral_error;

		m_currentSerialization = "";
		entry.printTo(m_currentSerialization);
	}

private:
	std::unique_ptr<std::vector<::PIDStateEntry>> m_array;
	int m_read;
	int m_length;
	std::vector<::PIDStateEntry>::const_iterator m_it;
	String m_currentSerialization;
	const char* m_currentSerializationPos;
	StaticJsonBuffer<JSON_OBJECT_SIZE(5)> m_jsonBuffer;
};



void PidService::handleState( AsyncWebServerRequest *request ) {
	std::unique_ptr<std::vector<::PIDStateEntry>> states( new std::vector<::PIDStateEntry>() );
	if ( !m_pidEngine->state( *states ) ) {
		TRACE_ERROR( "Error reading PID states" );
		sendError(request);
		return;
	}

	PidStatesStream* statesStream = new PidStatesStream( std::move(states) );
	request->onDisconnect( [statesStream]() {
		delete statesStream;
	});

	request->send( *statesStream, CONTENT_TYPE_JSON, statesStream->available() );
}


void PidService::handleSettings( AsyncWebServerRequest *request ) {
	::PIDSettings settings;
	if ( !m_pidEngine->settins(settings) ) {
		TRACE_ERROR( "Error reading PID settings" );
		sendError(request);
		return;
	}

	const int BUFFER_SIZE = JSON_OBJECT_SIZE(3) ;
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
	JsonObject &jsonSetings = jsonBuffer.createObject();
	jsonSetings["integral"] = settings.k_i;
	jsonSetings["proportional"] = settings.k_p;
	jsonSetings["derivative"] = settings.k_d;

	sendBasicJson( request, jsonSetings );
}


void PidService::handleSetSettings( AsyncWebServerRequest *request, char* data, size_t len ) {
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 0 );

	motion::Motors::PIDSettings settings;
	settings.k_i = root["integral"];
	settings.k_p = root["proportional"];
	settings.k_d = root["derivative"];

	if ( !m_pidEngine->setSettins(settings) ) {
		TRACE_ERROR( "Error settings pitch PID settings" );
		sendError( request );
		return;
	}

	sendNoContent( request );
}



/************************************************************************/
// Server
/************************************************************************/
Server::Server( motion::Motors* motors, mpu::Mpu9250* mpu, io::Display* display ):
				m_motors(motors), m_display(display), m_mpu(mpu), m_impl(80)
{
//	m_impl.on( "/", HTTP_ANY, [](AsyncWebServerRequest *request) {
//			request->redirect("/index.html");
//		});

	m_pitchService = new PidService( &m_impl, "/pitch", &m_motors->pitch(), m_display );
	m_speedService = new PidService( &m_impl, "/speed", &m_motors->speed(), m_display );
	m_headingService = new PidService( &m_impl, "/heading", &m_motors->heading(), m_display );

	AsyncMethodWebHandler<Server>::Factory handlers(this, display);

	m_impl.addHandler( handlers.create("/targets", HTTP_PUT, &Server::handlePutTargets ) );
	m_impl.addHandler( handlers.create("/mpu/settings", HTTP_GET, &Server::handleGetMpuSettings ) );
	m_impl.addHandler( handlers.create("/mpu/settings", HTTP_PUT, &Server::handlePutMpuSettings ) );
	m_impl.addHandler( handlers.create("/mpu/calibration", HTTP_GET, &Server::handleGetMpuCalibration ) );
	m_impl.addHandler( handlers.create("/mpu/calibration", HTTP_PUT, &Server::handlePutMpuCalibration ) );

	m_impl.onNotFound([this](AsyncWebServerRequest* request){
			if ( request->method() == HTTP_OPTIONS ) {
				handleOptionsRequest(request);
			}
			else {
				handleNotFound( request );
			}
		});

	SPIFFS.begin();
	m_impl.serveStatic("/", SPIFFS, "/app").setDefaultFile("index.html");

	DefaultHeaders::Instance().addHeader(ACCESS_CONTROL_ALLOW_ORIGIN, "*");

	m_impl.begin();
}


void Server::handleGetMpuSettings( AsyncWebServerRequest* request )
{
	int32_t pitchOffset;
	if ( !m_motors->getMpuOffset(&pitchOffset) ) {
		TRACE_ERROR( "Error reading MPU settings" );
		sendError( request );
		return;
	}

	StaticJsonBuffer<JSON_OBJECT_SIZE(1)> jsonBuffer;
	JsonObject& ret = jsonBuffer.createObject();
	ret["pitchOffset"] = ((float)pitchOffset) / 0x10000;		//Convert Q16 to float

	sendBasicJson( request, ret );
}


void Server::handlePutMpuSettings( AsyncWebServerRequest* request, char* data, size_t len )
{
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 0 );

	float pitchOffset = root["pitchOffset"];
	if ( !m_motors->setMpuOffset( pitchOffset*0x10000 ) ) {		//Convert float to Q16
		TRACE_ERROR( "Error setting MPU settings" );
		sendError( request );
		return;
	}

	sendNoContent( request );
}



void Server::handleGetMpuCalibration( AsyncWebServerRequest* request )
{
	StaticJsonBuffer<JSON_OBJECT_SIZE(2) * JSON_OBJECT_SIZE(3)> jsonBuffer;
	JsonObject& ret = jsonBuffer.createObject();

	JsonObject& accel = ret.createNestedObject("accel");
	accel["x"] = m_mpu->getAccelCalibration()[0];
	accel["y"] = m_mpu->getAccelCalibration()[1];
	accel["z"] = m_mpu->getAccelCalibration()[2];

	JsonObject& gyro = ret.createNestedObject("gyro");
	gyro["x"] = m_mpu->getGyroCalibration()[0];
	gyro["y"] = m_mpu->getGyroCalibration()[1];
	gyro["z"] = m_mpu->getGyroCalibration()[2];

	sendBasicJson( request, ret );
}


void Server::handlePutMpuCalibration( AsyncWebServerRequest* request )
{
	m_motors->pause();

	i2c::init( i2c::MPU_SDA, i2c::MPU_SCL );		//Change I2C channel
	m_mpu->calibrate();

	i2c::init( i2c::MOTORS_SDA, i2c::MOTORS_SCL );	//Recover I2C channel
	m_motors->resume();

	handleGetMpuCalibration( request );				//Send calibration data
}


void Server::handlePutTargets( AsyncWebServerRequest* request, char* data, size_t len )
{
	DynamicJsonBuffer jsonBuffer;
	const JsonObject& root = jsonBuffer.parseObject( data, 0 );

	int16_t speed = root["speed"];
	if ( !m_motors->speed().setTarget( speed ) ) {
		TRACE_ERROR( "Error setting speed target" );
		sendError( request );
		return;
	}

	int16_t heading = root["heading"];
	if ( !m_motors->heading().setTarget( heading ) ) {
		TRACE_ERROR( "Error setting heading target" );
		sendError( request );
		return;
	}

	sendNoContent( request );
}


void Server::handleNotFound( AsyncWebServerRequest* request )
{
	request->send( 404, "text/html", "<h1>Resource not found</h1>" );
}


void Server::handleOptionsRequest( AsyncWebServerRequest *request )
{
	AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "");

	String requestHeader = request->header(ACCESS_CONTROL_REQUEST_HEADERS);
	if ( requestHeader.length() > 0 ) {
		response->addHeader( ACCESS_CONTROL_ALLOW_HEADERS, requestHeader );
	}

	requestHeader = request->header(ACCESS_CONTROL_REQUEST_METHOD);
	response->addHeader( ACCESS_CONTROL_ALLOW_METHODS, (requestHeader.length()>0) ? requestHeader : "GET, POST, PUT, OPTIONS" );

	request->send( response );
}


}



