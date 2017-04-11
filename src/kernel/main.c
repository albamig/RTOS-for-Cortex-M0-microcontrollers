#include "../lib/lpc111x.h"
#include "systick.h"

int main() {
	init_systick(10);

	while (1);
}
