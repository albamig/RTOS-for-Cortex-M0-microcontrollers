#include "../lib/lpc111x.h"
#include "../lib/stdio.h"
#include "systick.h"

int main() {
#ifdef DEBUG
	_printf("Launching...\n");
#endif

	init_systick(10);

	while (1);
}
