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


static void wifiInit()
{
	WiFi.begin( wifi::ssid, wifi::password );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}
	Serial.print( "IP address: " );
	Serial.println( WiFi.localIP() );
}



void Application::init()
{
	pinMode(16, OUTPUT);
	pinMode(14, OUTPUT);

	wifiInit();

	if ( !initMpu() ) {
		digitalWrite(14, LOW);
	}

	while( !checkMpu() ) {
		delay( 100 );
	}

	if ( !initMotors() ) {
	//	digitalWrite(15, LOW);
	}

	m_httpServer.init( &m_motors );

	delay( 400 );
}


void Application::loop()
{
	static int i= 0;

	++i;
	if ( i > 6 ) {
		i = 0;
	}
	if (  i > 3 ) {
		digitalWrite(16, LOW);
		digitalWrite(14, LOW);
	}
	else {
		digitalWrite(16, HIGH);
		digitalWrite(14, HIGH);
	}

	Serial.printf( "%d\n", i );

//	m_httpServer.impl().handleClient();
	delay( 500 );
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


bool Application::initMotors()
{
	i2c::init( MOTORS_SDA, MOTORS_SCL );

	if( !m_motors.init() ) {
		while(1)
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

