/**********************************************************
                 	System Init
**********************************************************/

#define FOSC    12000000        // Crystal frequency
#define CCLK    FOSC	    	// CPU clock
#define PDIV  	1               // Peripheral divider
#define PCLK    (CCLK/PDIV)     // Peripheral clock

#define _delay_us(n) _delay_loop(CCLK/4000000*n-1)
#define _delay_ms(n) _delay_loop(CCLK/4000*n-1)

#define BAUD0 57600

#define DIVI0 ((PCLK/16+BAUD0/2)/BAUD0)

extern inline void System_init(void)  {

    SCS= 0x20;			// Enable main OSC
    while( !(SCS & 0x40) );	/* Wait until main OSC is usable */
    CLKSRCSEL = 0x1;		/* select main OSC, 12MHz, as the PLL clock source */
    PCLKSEL0 = 0x55555555;      /* PCLK is the same as CCLK */
    PCLKSEL1 = 0x55555555;      


    ///////////////////////////////////////////////////////
    //                       UARTs
    ///////////////////////////////////////////////////////

    PINSEL0 = 0x00000050; //TXD0,RXD0
    U0LCR=0x83;		// DLAB = 1
    U0DLL=DIVI0&0xFF;
    U0DLM=DIVI0>>8;
    U0LCR=0x3;	    // DLAB=0, 8-bit data, 1-bit stop, no parity
    U0FCR=0x07;     // FIFOs enabled and reset, 1-byte IRQ threshold

}

extern inline void System_uninit(void)
{
    PCLKSEL0 = 0x00000000;      /* PCLK is 1/4 CCLK */
    PCLKSEL1 = 0x00000000;
    PINSEL0 = 0;
    U0FCR=0;
    U0LCR=0;
    CLKSRCSEL=0;
    SCS=0;   
}
