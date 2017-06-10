#include "system.h"
#include "api/motors_i2c_api.h"

uint8_t system_registers[ MOTORSREG_SYSTEM_END ];

inline __attribute__((always_inline)) bool system_is_on()
{
	return system_registers[MOTORSREG_STATUS] & MOTORSREGBIT_STATUS_ON;
}

void system_init()
{
	system_registers[MOTORSREG_STATUS] = 0;
}