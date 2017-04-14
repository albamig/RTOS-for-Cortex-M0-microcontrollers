#include "../lib/lpc111x.h"
#include "../lib/stdio.h"
#include "systick.h"

int main() {
	_printf("Launching...\n");

	init_systick(10);

	while (1);
}
