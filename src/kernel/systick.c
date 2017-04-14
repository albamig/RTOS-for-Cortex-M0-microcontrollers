#include "../lib/lpc111x.h"
#include "systick.h"
#include "../lib/stdio.h"

void init_systick(float int_time) {
	SYST_RVR = (unsigned int) ((int_time/1000)*48000000);
	SYST_CVR = 0;
	SYST_CSR = 0x7;

#ifdef DEBUG
	_printf("SysTick initialized\n");
#endif
}

void systick_handler(void) {
#ifdef DEBUG
	_printf("Interrupción SysTick\n");
#endif
}
