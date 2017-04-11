#define _delay_us(n) delay_loop((n*(PCLK/1000)-6000)/4000)
#define _delay_ms(n) delay_loop((n*(PCLK/1000)-6)/4)

void delay_loop(unsigned int);
