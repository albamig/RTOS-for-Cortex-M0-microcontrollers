#include "../lib/lpc111x.h"
#include "../lib/system.h"


int main() {
	init_systick(10);
	_printf("Llego\n");

	while (1);
}

