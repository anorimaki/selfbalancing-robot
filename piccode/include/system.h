#ifndef SYSTEM_H
#define	SYSTEM_H

#include <stdint.h>
#include <stdbool.h>

extern uint8_t system_registers[];

void system_init();
bool system_is_running();
bool system_pause_request();
bool system_run_request();
void system_running();
void system_paused();

#endif

