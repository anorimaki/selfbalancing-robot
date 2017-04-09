#include "selfbalancing_app.h"
#include "util/trace.h"
#include "wifiproperties.h"




static selfbalancing::Application app;



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
	delay( 1000 );

	Serial.begin(115200);

	app.init();
}


void loop()
{
	app.loop();
}

