#include "Arduino.h"
#include "mpu/mpu9250.h"
#include "mpu/angles.h"
#include "http/httpserver.h"
#include "motion/motors.h"
#include "util/trace.h"
#include "wifiproperties.h"
#include "Wire.h"

#define SerialPort Serial

static mpu::Mpu9250 mpu9250;
static http::Server httpServer;
static motion::Motors motors;

static const uint8_t MPU_SCL = D5;
static const uint8_t MPU_SDA = D6;

static const uint8_t MOTORS_SCL = D1;
static const uint8_t MOTORS_SDA = D2;



static void errorHandler()
{
//	Serial.printf( "Error\n" );
}


static void wifiInit() {
	WiFi.begin ( wifi::ssid, wifi::password );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );
}


void setup()
{
	Serial.begin(115200);

					//load MPU firmware and nothing else to do with MPU
	Wire.begin( MPU_SDA, MPU_SCL );
	twi_setClock(400000);
	twi_setClockStretchLimit(250);
	mpu9250.init( &errorHandler );

	Wire.begin( MOTORS_SDA, MOTORS_SCL );
	motors.init();

	wifiInit();

	httpServer.init();
}


void loop()
{
	httpServer.impl().handleClient();
	delay( 10 );
}

