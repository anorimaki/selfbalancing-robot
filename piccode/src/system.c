#include "system.h"
#include "api/motors_i2c_api.h"

uint8_t system_registers[ MOTORSREG_SYSTEM_END ];


void system_init()
{
	system_registers[MOTORSREG_STATUS] = 0;
}


inline __attribute__((always_inline)) bool system_is_running()
{
	return system_registers[MOTORSREG_STATUS] & MOTORSREGBIT_STATUS_RUNNING;
}

inline __attribute__((always_inline)) bool system_pause_request()
{
	return system_registers[MOTORSREG_STATUS] & MOTORSREGBIT_STATUS_PAUSE_REQUEST;
}

inline __attribute__((always_inline)) bool system_run_request()
{
	return system_registers[MOTORSREG_STATUS] & MOTORSREGBIT_STATUS_RUN_REQUEST;
}

inline __attribute__((always_inline)) void system_running()
{
	system_registers[MOTORSREG_STATUS] = MOTORSREGBIT_STATUS_RUNNING;
}

inline __attribute__((always_inline)) void system_paused()
{
	system_registers[MOTORSREG_STATUS] = 0;
}
