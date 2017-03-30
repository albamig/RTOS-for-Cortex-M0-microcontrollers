///////// Definir para evital la lectura del código del chip ////////
//#define CODEPROT
///////////////////// TIPO de PCB ///////////////////

////////////////////////////////////////////////////////////////////////////////////
// Retardos activos y E/S básica
#define _delay_us(n) delay_loop((n*(PCLK/1000)-6000)/4000)
#define _delay_ms(n) delay_loop((n*(PCLK/1000)-6)/4)
void delay_loop(unsigned int);

/////////////////////////////////////////////////////
//#define XTAL_OSC		// External clock reference
#define PCLK 48000000	// Processor Clock

// UART
#define BAUD	115200
//#define PARITYODD

