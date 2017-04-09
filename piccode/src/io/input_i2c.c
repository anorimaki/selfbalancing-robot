#include "io/input_i2c.h"
#include "io/motors_protocol.h"
#include "system.h"
#include "i2c1.h"

#define INVALID_REGISTER_ADDRESS 0xFF

bool I2C1_StatusCallback(I2C1_SLAVE_DRIVER_STATUS status)
{
	static uint8_t receive_buffer;
	static uint8_t register_address;

    switch (status)
    {
		case I2C1_SLAVE_RECEIVE_REQUEST_DETECTED:
            I2C1_WritePointerSet(&receive_buffer);
			register_address = INVALID_REGISTER_ADDRESS;
            break;
			
        case I2C1_SLAVE_TRANSMIT_REQUEST_DETECTED:
            I2C1_ReadPointerSet(&system_registers[register_address]);
            register_address++;
            break;

        case I2C1_SLAVE_RECEIVED_DATA_DETECTED:
			if ( register_address == INVALID_REGISTER_ADDRESS ) {
				register_address = receive_buffer;
			}
			else {
				system_registers[register_address] = receive_buffer;
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
}