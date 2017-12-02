#include "io/input_i2c.h"
#include "api/motors_i2c_api.h"
#include "system.h"
#include "pidpitch.h"
#include "pidspeed.h"
#include "i2c1.h"

#define INVALID_REGISTER_ADDRESS 0xFF

static uint8_t send_buffer;
static uint8_t receive_buffer;

static void i2cslave_set_send_buffer( uint8_t register_address ) 
{
	if ( register_address < MOTORSREG_SYSTEM_END )
		send_buffer = system_registers[register_address];
	else if ( register_address < MOTORSREG_PITCH_PID_END )
		send_buffer = pidpitch_i2c_read(register_address-MOTORSREG_SYSTEM_END);
	else if ( register_address < MOTORSREG_SPEED_PID_END )
		send_buffer = pidspeed_i2c_read(register_address-MOTORSREG_PITCH_PID_END);
	else {
		printf( "send: %x\n", register_address );
	}
}


static void i2cslave_write( uint8_t register_address, uint8_t data ) 
{
	if ( register_address < MOTORSREG_SYSTEM_END )
		system_registers[register_address] = data;
	else if ( register_address < MOTORSREG_PITCH_PID_END )
		pidpitch_i2c_write( register_address-MOTORSREG_SYSTEM_END, data );
	else if ( register_address < MOTORSREG_SPEED_PID_END )
		pidspeed_i2c_write( register_address-MOTORSREG_PITCH_PID_END, data );
	else {
		printf( "rec: %x\n", register_address );
	}
}


bool I2C1_StatusCallback(I2C1_SLAVE_DRIVER_STATUS status)
{
	static uint8_t register_address;

    switch (status)
    {
		case I2C1_SLAVE_RECEIVE_REQUEST_DETECTED:
            register_address = INVALID_REGISTER_ADDRESS;
            break;
			
        case I2C1_SLAVE_TRANSMIT_REQUEST_DETECTED:
			i2cslave_set_send_buffer( register_address );
			register_address++;
            break;

        case I2C1_SLAVE_RECEIVED_DATA_DETECTED:
			if ( register_address == INVALID_REGISTER_ADDRESS ) {
				register_address = receive_buffer;
			}
			else {
				i2cslave_write( register_address, receive_buffer );
				register_address++;
			}
            break;
			
		default:
			break;
    }
	
	I2C1CONbits.SCLREL = 1;			//release SCL

    return true;
}


void inputi2c_init() 
{
	I2C1_SlaveAddressSet( motors_i2c_address );
	I2C1_ReadPointerSet(&send_buffer);
	I2C1_WritePointerSet(&receive_buffer);
}

