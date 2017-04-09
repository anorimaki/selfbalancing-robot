#ifndef SYSTEM_H
#define	SYSTEM_H

#include <stdint.h>
#include <stdbool.h>

extern uint8_t system_registers[];

void system_init();
bool system_is_on();

#endif

