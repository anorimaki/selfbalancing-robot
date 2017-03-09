#include "Arduino.h"
#include "mpu/mpu9250.h"

#define SerialPort Serial

static mpu::Mpu9250 mpu9250;


void setup()
{
	Serial.begin(115200);
	mpu9250.init();
}

void loop()
{
	if ( mpu9250 )
}

