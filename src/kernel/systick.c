#include "../lib/lpc111x.h"

void init_systick(int int_time) {
	SYST_RVR = (int_time/1000)*48000000;	
	SYST_CVR = 0;
	SYST_CSR = 0x7;
}

void systick_handler(void) {
		_printf("Interrupción SysTick\n");
}
