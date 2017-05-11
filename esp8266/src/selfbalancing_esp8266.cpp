#include "selfbalancing_app.h"
#include "util/trace.h"


static selfbalancing::Application app;


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

