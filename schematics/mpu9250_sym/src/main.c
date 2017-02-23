#include "main.h"

#use i2c(SLAVE,Fast,I2C1,MASK=0x00,ADDRESS=0xD0,force_hw)

#byte SSP1CON1 = getenv( "SFR:SSP1CON1" )
#byte SSP1CON2 = getenv( "SFR:SSP1CON2" )
#bit CKP = SSP1CON2.4
#byte SSP1STAT = getenv( "SFR:SSP1STAT" )
#bit SSPRW = SSP1STAT.2


void init()
{
	enable_interrupts( INT_SSP );
	enable_interrupts( GLOBAL );
}


#INT_SSP
void i2c_isr()
{
	int8 addr, data;
		
	int8 state = i2c_isr_state();
	
	if ( state==0 ) {
		addr = i2c_read(1);
	}
	else if ( state==0x80 ) {
		addr = i2c_read(2);
		i2c_write( 0xAA );
	}
	else if( state >= 0x80 ) {
		if ( SSPRW )
			i2c_write( 0xAA );
	}
	else if( state > 0 ) {
		data = i2c_read(1);
	}
}


void main()
{
	init();
	
	while( TRUE ) {
	}
}

