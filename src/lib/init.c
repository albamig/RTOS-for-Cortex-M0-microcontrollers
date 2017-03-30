#include "lpc111x.h"
#include "system.h"

/////////////////////////////////////////////////////////////
//				Prototypes for vector table
void Reset_init(void);
void Default_Handler(void);
__attribute__ ((weak)) void systick_handler(void);

/////////////////////////////////////////////////////////////
//                  	VECTOR TABLE
// the section "vectors" is placed at the beginning of flash 
// by the linker script
/////////////////////////////////////////////////////////////

extern void (* const Vectors[])(void);
__attribute__((section(".vectors"))) void (* const Vectors[]) (void) = {
	(void *)0x10002000, 	/*  0 Top of stack 	*/ 
	Reset_init,   			/*  1 Reset Handler */
	Default_Handler,		/*  2 NMI 			*/
	Default_Handler,		/*  3 Hard Fault 	*/
	0,	                	/*  4 Reserved 		*/
	0,                   	/*  5 Reserved 		*/
	0,                   	/*  6 Reserved 		*/
	0,                   	/*  7 Reserved 		*/
	0,                   	/*  8 Reserved 		*/
	0,                   	/*  9 Reserved 		*/
	0,                   	/* 10 Reserved 		*/
	Default_Handler,			/* 11 SVC 	   		*/
	0,                   	/* 12 Reserved 		*/
	0,                   	/* 13 Reserved 		*/
	Default_Handler,     	/* 14 PendSV   		*/
	systick_handler,     	/* 15 SysTick  		*/		
/* External interrupt handlers follow */
	Default_Handler, 		/* 16 PIO0_0  ( IRQ0) */
	Default_Handler, 		/* 17 PIO0_1  ( IRQ1) */
	Default_Handler, 		/* 18 PIO0_2  ( IRQ2) */
	Default_Handler, 		/* 19 PIO0_3  ( IRQ3) */
	Default_Handler, 		/* 20 PIO0_4  ( IRQ4) */
	Default_Handler, 		/* 21 PIO0_5  ( IRQ5) */
	Default_Handler, 		/* 22 PIO0_6  ( IRQ6) */
	Default_Handler, 		/* 23 PIO0_7  ( IRQ7) */
	Default_Handler, 		/* 24 PIO0_8  ( IRQ8) */
	Default_Handler, 		/* 25 PIO0_9  ( IRQ9) */
	Default_Handler, 		/* 26 PIO0_10 (IRQ10) */
	Default_Handler, 		/* 27 PIO0_11 (IRQ11) */
	Default_Handler,		/* 28 PIO1_0  (IRQ12) */
	Default_Handler ,  		/* 29 C_CAN   (IRQ13) */
	Default_Handler, 		/* 30 SSP1    (IRQ14) */
	Default_Handler, 		/* 31 I2C     (IRQ15) */
	Default_Handler, 		/* 32 CT16B0  (IRQ16) */
	Default_Handler, 		/* 33 CT16B1  (IRQ17) */
	Default_Handler, 		/* 34 CT32B0  (IRQ18) */
	Default_Handler, 		/* 35 CT32B1  (IRQ19) */
	Default_Handler, 		/* 36 SSP0    (IRQ20) */
	Default_Handler,		/* 37 UART    (IRQ21) */
	Default_Handler, 		/* 38 RESERVED     	  */
	Default_Handler, 		/* 39 RESERVED     	  */
	Default_Handler, 		/* 40 ADC     (IRQ24) */
	Default_Handler, 		/* 41 WDT     (IRQ25) */
	Default_Handler, 		/* 42 BOD     (IRQ26) */
	Default_Handler, 		/* 43 RESERVED     	  */
	Default_Handler, 		/* 44 PIO3    (IRQ28) */
	Default_Handler, 		/* 45 PIO2    (IRQ29) */
	Default_Handler, 		/* 46 PIO1    (IRQ30) */
	Default_Handler 		/* 47 PIO0    (IRQ31) */
};

/////////////////////////////////////////////////////////////////////////
// El código de "init.c" va en la sección .bcrp para aprovechar el hueco
// que hay entre la tabla de vectores y la palabra de protección
// contra lectura (CRP, @0x2FC), 572 bytes.
/////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// This function sets the main clock to the PLL
// The PLL input is the 12MHz IRC or Crystal oscillator
// depending on XTAL_OSC being defined.
// MSEL = 3, PSEL = 2, sets the PLL factor as x4
// resulting in a 48MHz main clock, and 192MHz CCO (156MHz<fcco<320MHz)
///////////////////////////////////////////////////////////////////////

extern inline void clock_init()
{
	unsigned int i;
#ifdef XTAL_OSC	
	PDRUNCFG &= ~(BIT7|BIT5); // Power up the PLL & system oscillator.
	SYSPLLCLKSEL = 1; 	// select system oscillator (crystal)
#else
	PDRUNCFG &= ~BIT7; // Power up the PLL.
	SYSPLLCLKSEL = 0; // select internal RC oscillator
#endif
	_delay_ms(40/4);	// delay for oscillator startup (40ms)
	SYSPLLCTRL = (3 << 0) | (1 << 5); // set divisors/multipliers
	SYSPLLCLKUEN = 1; // inform PLL of update

	// If locked in a reasonable time switch to PLL, else keep
	// running on the 12MHz IRC
	for (i=1<<20;i;i--) {
		if (SYSPLLSTAT&1) {	// if locked
			MAINCLKSEL = 3; // Use PLL as main clock
			MAINCLKUEN = 1; // Inform core of clock update
			break;
		}
	} 
}


////////////////////////////////////////////////////////////////////////
// System-dependent init
////////////////////////////////////////////////////////////////////////

extern inline void sys_init()
{
    // Turn on clock for IOCON
    SYSAHBCLKCTRL|= BIT16;
	// Release all peripheral resets
	PRESETCTRL=0x0F;	

	// UART. See "Enabling sequence for UART clock" on LPC111x User Manual
	// first assign pin functions for UART	
	IOCON_PIO1_6=1; // RXD
	IOCON_PIO1_7=1; // TXD
	// then enable the UART clock
	UARTCLKDIV=1;	// then enable the UART clock
	SYSAHBCLKCTRL= 0x7FFFF;	// Clock for all (also for UART)
	// data baud & format
#define UDIVI ((PCLK/BAUD+8)/16)
    U0LCR=0x80;
	U0DLM=UDIVI>>8;
	U0DLL=UDIVI&255;
#ifdef PARITYODD
	U0LCR=0x0B;
#else
  #ifdef PARITYEVEN
	U0LCR=0x1B;
  #else
	U0LCR=0x3;	// 8-bit 
  #endif
#endif
	U0FCR=7;		// con FIFOS
	//U0IER=1;		// interrupt on RX
	//ISER=1<<21;		// NVIC: Enable UART0 interrupt

	// SYSTIMER
	//SYST_RVR=240000-1;	// Reload value for 5ms interval
	//SYST_CVR=0;			// Clear current value
	//SYST_CSR=7;			// Enable interrupt, enable timer, CLK=processor clock

/*
	// IOCONFIG 
		// Por defecto reservados. ¡Serán idiotas en NXP!
	IOCON_R_PIO0_11=1;	
		// PIO1.0-PIO1.3: GPIO, sin pull-up, hysteresis, digital input.
	IOCON_R_PIO1_0=IOCON_R_PIO1_1=IOCON_R_PIO1_2=IOCON_SWDIO_PIO1_3=1|(1<<5)|(1<<7);
	IOCON_PIO1_4=IOCON_PIO1_5=		// Sin pull-ups, con histéresis, digital input
	IOCON_PIO3_4=IOCON_PIO3_5=
	IOCON_PIO2_10=IOCON_PIO2_11= 	
	IOCON_PIO0_3=(1<<5)|(1<<7);			
*/
	GPIO2DIR=0x1F<<4;	// PIO2.4-PIO2.8 como salidas
	GPIO3DIR=7<<2;		// PIO3.2-PIO3.4 como salidas

	// Timer 32B0: free run (cuenta en microsegundos)
	TMR32B0TCR=3;
	TMR32B0PR=47;
	TMR32B0TCR=1;

	// ADC
	PDRUNCFG&=~(1<<4);	// Power to ADC
	IOCON_R_PIO0_11=2;	// P0.11 como AD0
}


///////////////////////////////////////////////////////////////////
// Entry point from vector table
///////////////////////////////////////////////////////////////////

// The following are 'declared' in the linker script
extern unsigned char  INIT_DATA_VALUES;
extern unsigned char  INIT_DATA_START;
extern unsigned char  INIT_DATA_END;
extern unsigned char  BSS_START;
extern unsigned char  BSS_END;

void __attribute__((naked,section(".bcrp"))) Reset_init()
{
	unsigned char *src;
	unsigned char *dest;

	// do global/static data initialization
	src= &INIT_DATA_VALUES;
	dest= &INIT_DATA_START;
	while (dest<&INIT_DATA_END) *dest++ = *src++;
	// zero out the uninitialized global/static variables
	dest = &BSS_START;
	while (dest<&BSS_END) *dest++=0;
	// other init tasks
	clock_init();
	_delay_ms(20);
	sys_init();
	ISER=1<<21;		// NVIC: Enable UART0 interrupt
	// call main
	main();
}

///////////////////////////////////////////////////////////////////
// Exception reporting
///////////////////////////////////////////////////////////////////

void __attribute__((naked,section(".bcrp")))  Default_Handler()
{
	unsigned int i,*sp;
	asm volatile ("mrs %0,psr\n":"=r"(i));
	_printf("\nDefault Handler. Exception_number=%d\n",i&0x3f);
	asm volatile ("mov %0,sp\n":"=r"(sp));
	for(i=0;i<8;i++) _printf("%08x ",sp[i]);
	while(1);
}

// Dummy exit function
void __attribute__((naked,section(".bcrp"))) exit(int op) 
{
	while(1);
}

void __attribute__((naked, section(".bcrp"), aligned(8))) delay_loop(unsigned int d){
	asm volatile("Ldelay_loop: sub r0, #1"); 
	asm volatile("bne Ldelay_loop"); 
	asm volatile("bx lr");
}
//////////////////////////////////////////////////////////////////////////////////////
////////////////// CODE READ PROTECTION (@ 0x2FC, see linker_script) /////////////////
//////////////////////////////////////////////////////////////////////////////////////
#ifdef CODEPROT
const unsigned int CRP[] __attribute__((section(".crp"))) ={0x87654321};
#else
const unsigned int CRP[] __attribute__((section(".crp"))) ={0}; // no CRP
#endif

