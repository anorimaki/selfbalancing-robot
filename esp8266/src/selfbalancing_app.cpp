#include "selfbalancing_app.h"
#include "util/trace.h"
#include "mpu/angles.h"
#include "i2c/i2c.h"
#include "wifiproperties.h"


namespace selfbalancing
{


static const uint8_t MPU_SCL = D1;
static const uint8_t MPU_SDA = D2;

static const uint8_t MOTORS_SCL = D5;
static const uint8_t MOTORS_SDA = D6;


static void wifiInit()
{
	WiFi.begin( wifi::ssid, wifi::password );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );
}



void Application::init()
{
	wifiInit();
	m_httpServer.init( &m_motors );
	m_state = initMpu();
	delay( 400 );
}


void Application::loop()
{
#if 1
			//MPU check is here (and not in init method) to avoid errors if Arduino setup() function takes
			//too many time.
	if ( m_state == MpuChecking ) {
		m_state = checkMpu();
	}

	if ( m_state == MotorsInitiallization ) {
		m_state = initMotors();
	}

#else
	Optional<mpu::MpuData> data;
	if ( !m_mpu9250.getData(data) ) {
		TRACE_ERROR( "MPU check failed" );
	}

	if ( data ) {
		showData( *data );
	}
#endif

	m_httpServer.impl().handleClient();
//	delay( 500 );
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


Application::State Application::initMotors()
{
	i2c::init( MOTORS_SDA, MOTORS_SCL );

	if( !m_motors.init() ) {
		TRACE_ERROR( "Motors initialization failed" );
		return MotorsError;
	}

	TRACE("Motors initialized");

	return Running;
}


Application::State Application::initMpu()
{
	i2c::init( MPU_SDA, MPU_SCL );

	if ( !m_mpu9250.init() ) {
		TRACE_ERROR( "MPU initialization failed" );
		return MpuError;
	}

	TRACE("MPU initialized");

	return MpuChecking;
}


Application::State Application::checkMpu()
{
	Optional<mpu::MpuData> data;
	if ( !m_mpu9250.getData(data) ) {
		TRACE_ERROR( "MPU check failed" );
		return MpuError;
	}

	TRACE( "MPU check: %d", !!data );

	if ( !data ) {
		delay( 100 );
		return MpuChecking;
	}

	m_mpu9250.end();

	return MotorsInitiallization;
}


}

