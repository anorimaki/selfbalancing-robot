#include "system.h"
#include "io/motors_reg.h"

uint8_t system_registers[ MOTORS_REGISTERS_SIZE ];

inline __attribute__((always_inline)) bool system_is_on()
{
	return system_registers[MOTORSREG_STATUS] & MOTORSREGBIT_STATUS_ON;
}

void system_init()
{
	system_registers[MOTORSREG_STATUS] = 0;
}