#include "main.h"
#include "mpu9250.h"

#pin_select U1TX=PIN_B11
#pin_select U1RX=PIN_B10
#use rs232(baud=57600, UART1, STOP=1)

void init()
{
	setup_spi( SPI_SS_DISABLED );
    setup_spi2( SPI_SS_DISABLED );
    setup_wdt( WDT_OFF );
    setup_adc_ports( ADC_OFF );
	
	mpu_init();
	mpu_set_sensors( MPU_SENSOR_XYZ_GYRO|MPU_SENSOR_XYZ_ACCEL );
}

void main()
{
	init();
	
	while(1)
	{
		signed int16 data[3];
		mpu_get_accel_reg( data, 0 );
		
		printf( "%04Ld %04Ld %04Ld\n", data[0], data[1], data[2]  );
		
		delay_ms(500);
	}
}

#include "mpu9250.c"
#include "mpu_i2c.c"