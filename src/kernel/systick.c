#include "../lib/lpc111x.h"
#include "systick.h"
#include "../lib/stdio.h"

void init_systick(float int_time) {
	SYST_RVR = (unsigned int) ((int_time/1000)*48000000);
	SYST_CVR = 0;
	SYST_CSR = 0x7;

	_printf("SysTick initialized\n");
}

void systick_handler(void) {
		_printf("Interrupción SysTick\n");
}
