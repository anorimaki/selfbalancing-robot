#include "Arduino.h"
#include "mpu/mpu9250.h"
#include "mpu/angles.h"
#include "http/httpserver.h"
#include "util/trace.h"
#include "wifiproperties.h"

#define SerialPort Serial

static mpu::Mpu9250 mpu9250;
static http::Server httpServer;


static void errorHandler()
{
//	Serial.printf( "Error\n" );
}


void setup()
{
	Serial.begin(115200);

	Serial.printf( "Hello\n" );

	mpu9250.init( &errorHandler );

	WiFi.begin ( wifi::ssid, wifi::password );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );

	httpServer.init();
}

void loop()
{
	Optional<mpu::MpuData> data = mpu9250.getData();
	if ( data ) {
		const mpu::Quaternation& q= data->quaternation().get();
		mpu::Angles angles(q);
		angles.toDegress();

		Serial.printf( "[%lu] Quater: %s %s %s %s ->", data->timestamp(),
				String(q.w()).c_str(),
				String(q.x()).c_str(),
				String(q.y()).c_str(),
				String(q.z()).c_str() );

		Serial.printf( " %s %s %s\n",
						String(angles.pitch()).c_str(),
						String(angles.roll()).c_str(),
						String(angles.yaw()).c_str() );

		httpServer.impl().handleClient();

	}
	delay( 10 );
}

