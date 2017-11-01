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
	m_display.systemInitialization();
	delay( 100 );

	if ( !initWifi() ) {
		m_display.wifiInitError( 3000 );
	}

	if ( !initMpu() ) {
		m_display.mpuInitError();
		return;
	}

	while( !checkMpu() ) {
		m_display.initializingMpu();
		delay( 100 );
	}

	if ( !initMotors() ) {
		m_display.motorsInitError();
		return;
	}

	m_httpServer.init( &m_motors, &m_display );

	m_display.systemInitialized();

	delay( 1000 );
}


void Application::loop()
{
	m_httpServer.impl().handleClient();
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

	Serial.print( "\nIP address: " );
	Serial.println( WiFi.localIP() );
	return true;
}


bool Application::initMotors()
{
	i2c::init( MOTORS_SDA, MOTORS_SCL );

	if( !m_motors.init( &m_display ) ) {
		TRACE_ERROR( "Motors initialization failed" );
		return false;
	}

	TRACE("Motors initialized");

	return true;
}


bool Application::initMpu()
{
	i2c::init( MPU_SDA, MPU_SCL );

	if ( !m_mpu9250.init() ) {
		TRACE_ERROR( "MPU initialization failed" );
		return false;
	}

	if ( !m_mpu9250.calibrate() ) {
		TRACE_ERROR( "MPU calibration failed" );
		return false;
	}

	if ( !m_mpu9250.configure() ) {
		TRACE_ERROR( "MPU configuration failed" );
		return false;
	}

	TRACE("MPU initialized");

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

	if ( !data ) {
		return false;
	}

	m_mpu9250.end();

	return true;
}


}

