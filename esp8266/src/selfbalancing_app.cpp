#include "selfbalancing_app.h"
#include "util/trace.h"
#include "mpu/angles.h"
#include "i2c/i2c.h"
#include "wifiproperties.h"
#include "motors_i2c_api.h"


namespace selfbalancing
{


void Application::init()
{
	delay( 100 );
	Serial.begin(115200);
	Serial.println();

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
		m_httpServer = new http::Server( &m_motors, &m_mpu9250, &m_display );
	}

	m_display.systemInitialized();

	delay( 500 );
}


void Application::loop()
{
	doInput();

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
	i2c::init( i2c::MOTORS_SDA, i2c::MOTORS_SCL );
	m_motors.init( &m_display );
}


bool Application::mpuInitialization()
{
	i2c::init( i2c::MPU_SDA, i2c::MPU_SCL );

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

	i2c::init( i2c::MOTORS_SDA, i2c::MOTORS_SCL );	//Recover I2C channel

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


#define MPU_OFFSET_INCREMENT 0x00000100
#define MPU_OFFSET_DECREMENT (-MPU_OFFSET_INCREMENT)

bool Application::changeMpuOffset( bool inc )
{
	int32_t offset;
	if ( !m_motors.getMpuOffset( &offset ) ) {
		TRACE_ERROR( "Get MPU offset failed" );
		return false;
	}
	offset += (inc ? MPU_OFFSET_INCREMENT : MPU_OFFSET_DECREMENT);
	if ( !m_motors.setMpuOffset( offset ) ) {
		TRACE_ERROR( "Set MPU offset failed" );
		return false;
	}
	m_display.mpuOffsetChanged( inc );
	return true;
}



enum InputCommand { NOTHING, MPU_OFFSET_MORE, MPU_OFFSET_LESS };
static const unsigned long debouncePeriod = 100; 					//In ms
static const unsigned long mpuOffsetMorePeriod = debouncePeriod*3; 	//In ms

#define BUTTON_PRESSED(state) (state==LOW)

static InputCommand checkButton() {
	static unsigned long lastChangeTime = 0;
	static int oldState = HIGH;
	static bool inMpuOffsetMorePeriod = false;

	bool currentState = digitalRead(0);
	unsigned long currentTime = millis();

	if ( currentState != oldState ) {
		lastChangeTime = currentTime;
		oldState = currentState;
		return NOTHING;
	}

	unsigned long period = currentTime-lastChangeTime;
	if ( period < debouncePeriod ) {
		return NOTHING;
	}

	if ( BUTTON_PRESSED(currentState) ) {
		if ( period > mpuOffsetMorePeriod ) {
			lastChangeTime = currentTime;		//Reset time to do increments each mpuOffsetMorePeriod if keep pressed
			inMpuOffsetMorePeriod = true;
			return MPU_OFFSET_MORE;
		}
	}
	else {
		if ( (period < mpuOffsetMorePeriod) && !inMpuOffsetMorePeriod ) {
						//Button has been released in mpuOffsetLessPeriod
			lastChangeTime = 0;					//Just to not repeat the command. Next period will be > mpuOffsetMorePeriod
			return MPU_OFFSET_LESS;
		}
		inMpuOffsetMorePeriod = false;
	}

	return NOTHING;
}


void Application::doInput()
{
	InputCommand input = checkButton();
	switch( input ) {
		case MPU_OFFSET_MORE:
			changeMpuOffset(true);
			break;
		case MPU_OFFSET_LESS:
			changeMpuOffset(false);
			break;
		default:
			break;
	}
}


}

