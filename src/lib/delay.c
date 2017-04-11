#include "delay.h"

void __attribute__((naked, section(".bcrp"), aligned(8))) delay_loop(unsigned int d){
	asm volatile("Ldelay_loop: sub r0, #1"); 
	asm volatile("bne Ldelay_loop"); 
	asm volatile("bx lr");
}
