#include "selfbalancing_app.h"
#include "util/trace.h"
#include "mpu/angles.h"
#include "i2c/i2c.h"
#include "wifiproperties.h"
#include "motors_i2c_api.h"


namespace selfbalancing
{

static const uint8_t MPU_SCL = D1;
static const uint8_t MPU_SDA = D2;

static const uint8_t MOTORS_SCL = D6;
static const uint8_t MOTORS_SDA = D7;


void Application::init()
{
	TRACE("");

	m_display.systemInitialization();
	delay( 500 );

	initMotors();

	if ( !m_motors.isRunning() ) {
		if( !mpuInitialization() ) {
			return;
		}

		if ( !m_motors.resume() ) {
			TRACE_ERROR( "Motors start failed" );
			m_display.motorsInitError();
			return;
		}

		TRACE("Motors initialized");
	}

	if ( !initWifi() ) {
		m_display.wifiInitError( 2000 );
	}
	else {
		m_httpServer = new http::Server( &m_motors, &m_display );
	}

	m_display.systemInitialized();

	delay( 500 );
}


static const unsigned long debounceDelay = 500; //In ms

static bool calibrationPressed() {
	static unsigned long lastStateChange = 0;
	static int oldState = -1;
	int currentState = digitalRead(0);

	if ( (oldState != currentState) && (millis()>(lastStateChange+debounceDelay)) ) {
		oldState = currentState;
		return !currentState;
	}
	return false;
}


void Application::loop()
{
	if ( calibrationPressed() ) {
		m_motors.pause();

		i2c::init( MPU_SDA, MPU_SCL );			//Change I2C channel
		m_mpu9250.calibrate();

		i2c::init( MOTORS_SDA, MOTORS_SCL );	//Recover I2C channel
		m_motors.resume();
	}

	if ( m_httpServer ) {
		m_httpServer->impl().handleClient();
	}

	m_display.update();
}


void Application::showData( const mpu::MpuData& data )
{
	bool separator = false;

	if ( data.accel() ) {
		mpu::Angles angles( *data.accel() );
		angles.toDegress();
		Serial.printf( "Accel: %d, %d, %d. P: %s, R: %s, Y: %s",
				data.accel()->x(),
				data.accel()->y(),
				data.accel()->z(),
				String(angles.pitch()).c_str(),
				String(angles.roll()).c_str(),
				String(angles.yaw()).c_str() );

		separator = true;
	}

	if ( data.gyro() ) {
		if ( separator ) {
			Serial.printf( " ----- " );
		}
		Serial.printf( "Gyro: %d, %d, %d",
				data.gyro()->x(),
				data.gyro()->y(),
				data.gyro()->z() );

		separator = true;
	}

	if ( data.quaternation() ) {
		if ( separator ) {
			Serial.printf( " ----- " );
		}
		mpu::Angles angles( *data.quaternation() );
		angles.toDegress();
		Serial.printf( "Quat: P: %s, R: %s, Y: %s",
				String(angles.pitch()).c_str(),
				String(angles.roll()).c_str(),
				String(angles.yaw()).c_str() );
	}

	Serial.printf( "\n" );
}


bool Application::initWifi()
{
	WiFi.begin( wifi::ssid, wifi::password );

	uint8_t status = WiFi.status();
	while ( status == WL_DISCONNECTED ) {
		delay ( 1000 );
		m_display.initializingWifi();
		status = WiFi.status();
	}

	if ( status != WL_CONNECTED ) {
		TRACE_ERROR( "WIFI error %d", status );
		return false;
	}

	Serial.print( "IP address: " );
	Serial.println( WiFi.localIP() );
	return true;
}


void Application::initMotors()
{
	i2c::init( MOTORS_SDA, MOTORS_SCL );
	m_motors.init( &m_display );
}


bool Application::mpuInitialization()
{
	i2c::init( MPU_SDA, MPU_SCL );

	if ( !initMpu() ) {
		m_display.mpuInitError();
		return false;;
	}

	while( !checkMpu() ) {
		m_display.initializingMpu();
		delay( 100 );
	}

	if ( !m_mpu9250.end() ) {
		TRACE_ERROR( "MPU end failed" );
		m_display.mpuInitError();
		return false;
	}

	TRACE("MPU initialized");

	i2c::init( MOTORS_SDA, MOTORS_SCL );	//Recover I2C channel

	return true;
}


bool Application::initMpu()
{
	if ( !m_mpu9250.init() ) {
		TRACE_ERROR( "MPU initialization failed" );
		return false;
	}

	if ( !m_mpu9250.storedCalibration() ) {
		TRACE_ERROR( "MPU calibration failed" );
		return false;
	}

	if ( !m_mpu9250.configure() ) {
		TRACE_ERROR( "MPU configuration failed" );
		return false;
	}

	return true;
}


bool Application::checkMpu()
{
	Optional<mpu::MpuData> data;
	if ( !m_mpu9250.getData(data) ) {
		TRACE_ERROR( "MPU check failed" );
		return false;
	}

	TRACE( "MPU check: %d", !!data );

	return !!data;
}


}

