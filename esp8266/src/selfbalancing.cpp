#include "selfbalancing_app.h"
#include "util/trace.h"

static selfbalancing::Application app;


void setup()
{
	app.init();
}


void loop()
{
	app.loop();
}

