#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#ifdef __linux__
  #include <asm/termios.h>
#else
  #include <termios.h>
#endif

#define __linux__
#include <fcntl.h>
#include <stdio.h>

#define CRYSTAL 14746000

#ifndef __linux__
#include "windows.h"
#define true 1
#define false 0
#define DEV "COM3"
// Variables necesarias
HANDLE idComDev;
DCB dcb = {0};
//------------------------------------------------------------------------------
int AbrirCOM(char * COM_PORT, int baud)
{
 idComDev = CreateFile(COM_PORT, GENERIC_READ | GENERIC_WRITE,
                    0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
 if(idComDev ==INVALID_HANDLE_VALUE)
  { printf("Error 1, puerto serie: %d\n", GetLastError()); exit(1); }
 dcb.DCBlength=sizeof(dcb);

 if(!GetCommState(idComDev, &dcb))
  { printf("Error 2, puerto serie\n"); exit(2); }
// dcb.BaudRate = (DWORD) CBR_115200;
 dcb.BaudRate = (DWORD) baud;
 dcb.ByteSize = (BYTE) 8;
 dcb.Parity   = (BYTE) NOPARITY;
 dcb.StopBits = (BYTE) ONESTOPBIT;

 if(!SetCommState(idComDev, &dcb))
  { printf("Error 3, SetCommStatus\n"); exit(3); }

 COMMTIMEOUTS timeouts={0};
 timeouts.ReadIntervalTimeout = 50;
 timeouts.ReadTotalTimeoutConstant = 50;
 timeouts.ReadTotalTimeoutMultiplier = 10;
 timeouts.WriteTotalTimeoutConstant = 50;
 timeouts.WriteTotalTimeoutMultiplier = 10;

 if(!SetCommTimeouts(idComDev,&timeouts))
  { printf("Error 4, timeouts\n"); exit(4); }

 return true;
}
//------------------------------------------------------------------------------
void CerrarCOM()
{
 CloseHandle(idComDev);
}
#else
#define DEV "/dev/ttyUSB0"
#endif

// Códigos de bootloader
// ARM7 (LPC2xxx)
unsigned char boot2[]=
#include "boot2.h"
// CORTEX-M0 (LPC11xx)
unsigned char boot2m[]=
#include "boot2m.h"

///// Funciones auxiliares /////

// Lee una línea desde la UART a "buf" y retorna el nº de bytes leídos
int ugets(int fd, unsigned char *buf)
{
    int i=0,j;
    unsigned char a;

    for(;;) {
        j=read(fd,&a,1);
		if (j!=1) {printf("j=%d\n",j); continue;}
        if (a=='\n') break;
        if (a=='\r') continue;
        buf[i++]=a;
    }
    buf[i++]=0;
    return --i;
}

// write seguro. Reintenta hasta escribir todo el buffer
void safewrite(int fd,char *buf,int n)
{
	int i,j;
	i=0;
	do {
		j=write(fd,&buf[i],n);
		i+=j; n-=j;
	} while (n);
}

// Imprime un caracter en la UART (para uuenc)
void uputch(int fd,unsigned char a)
{
    write(fd,&a,1);
}

// Codificación de datos de tipo "uuencode"
// El ISP del micro quiere los bloques de datos en este formato
// Codifica los "n" bytes de "buf" y los transmite por la UART
//   "n" debe ser múltiplo de 3, ya que 3 bytes dan lugar a 4
// Retorna la suma de los datos transmitidos

int uuenc(int fd,unsigned char *buf,int n)
{
    int i,j,ck;
    unsigned char a,b,c,*p;

    p=buf;
    ck=0;
    do {
        j=45;
        if (n<j) j=n;
        uputch(fd,32+j);
        for (i=j/3;i;i--) {
            a=*p++; b=*p++; c=*p++;
            uputch(fd,32+(a>>2));
            uputch(fd,32+((a&0x3)<<4)+(b>>4));
            uputch(fd,32+((b&0xf)<<2)+(c>>6));
            uputch(fd,32+(c&0x3f));
            ck+=a+b+c;
        }
        uputch(fd,'\n');
        n-=j;
    } while (n);
    return ck;
}


/////////////////////////////////////////////////////////////////////////////
// Listado de dispositivos conocidos
#include "lpcid.c"

/////////////////////////////////////////////////////////////////////////////
// MAIN

main(int argc,char **argv)
{
        unsigned char a,dn[128],*imn,*logn,fn[256];
        static const int baudtable[][2]={
            50,B50,
            75,B75,
            110,B110,
            150,B150,
            200,B200,
            300,B300,
            600,B600,
            1200,B1200,
            2400,B2400,
            4800,B2400,
            9600,B9600,
            19200,B19200,
            38400,B38400,
            57600,B57600,
            115200,B115200,
            0,0
        };
        int i,j,k,fd,fdin,fdlog=0,dir,baud=115200,bbaud=38400,paridad=0,result,hex=0,crlf=0,dbase=0,isp=0,sl;
		int cortex=0,execaddr;
        struct termios term,term0;
        unsigned char *p,buf[1024],bn[4];
        static const unsigned char anim[4]={'|','\\','-','/'};

		printf("\n\n--------------------------------------------\n");
		printf("  bt2  Versión %s   J. Arias\n",VERSION);
		printf("Bootloader RAM para LPC ARM y terminal serie\n");
		printf("--------------------------------------------\n\n\n");
        // Parse args
        strcpy(dn,DEV);
        imn=logn=NULL;
        for (i=1;i<argc;i++) {
            if (*argv[i]=='-') {
                switch(argv[i][1]) {
				default:
                case 'h':   printf("Uso: %s opciones\n",argv[0]);
                            printf("Opciones:\n");
                            printf("-h : Help\n");
                            printf("-d <dispositivo_serie> : puerto serie a usar (/dev/ttyUSB..)\n");
                            printf("-s <velocidad> : Velocidad en baudios del puerto para terminal\n");
                            printf("-po : paridad impar para terminal\n");
                            printf("-pe : paridad par para terminal\n");
                            printf("-sb <velocidad> : Velocidad en baudios del puerto para bootloader\n");
                            printf("-l <fichero.bin> : carga fichero imagen\n");
                            printf("-b dir_base(hex) : dirección de carga\n");
                            printf("-m : CORTEX-M como CPU destino por defecto\n");
                            printf("-i : terminal para ISP\n");
                            printf("-x : volcado hexadecimal de datos recibidos\n");
                            printf("-c : emite \\r\\n en lugar de sólo \\n\n");
                            printf("-g <fichero_log>: graba los datos recibidos en fichero\n\n");
                            exit(0);
                case 'd':   strcpy(dn,argv[++i]); break;
                case 's':   if (argv[i][2]=='b') bbaud=atoi(argv[++i]); 
							else baud=atoi(argv[++i]);
							break;
                case 'm':   cortex=1; break;
                case 'x':   hex=1; break;
                case 'c':   crlf=1; break;
                case 'i':   isp=crlf=1; break;
                case 'l':   imn=argv[++i]; break;
                case 'g':   logn=argv[++i]; break;
                case 'b':   dbase=strtoul(argv[++i],NULL,16); break;
				case 'p':	if (argv[i][2]=='o') paridad=1;
							if (argv[i][2]=='e') paridad=2;
							break;
                }
            }
        }

        /* ponemos las cosas del puerto en su sitio */

#ifndef __linux__
		char comname[32];
        for (i=20;i>1;i--) {
        	sprintf(dn,"/dev/ttyS%d",i);
            //printf(">%s<\n",dn);
        	if ((fd=open(dn,O_RDWR|O_NOCTTY|O_NONBLOCK))>0) goto L1;
        }
        printf("*** Puerto serie no encontrado\n");
        exit(0);
L1:     close(fd);
        sprintf(comname,"\\\\.\\COM%d",i+1);
        AbrirCOM(comname,bbaud);
        CerrarCOM();
#endif
        for (i=0;baudtable[i][0];i++) {
            if (bbaud==baudtable[i][0]) break;
        }
        if (!baudtable[i][0]) {printf("Velocidad no soportada\n"); exit(0);}
	    printf("Puerto serie: %s\nVelocidad (boot): %d baudios\n",dn,bbaud);

		// Abrimos el puerto serie en modo no bloqueante
		if ((fd=open(dn,O_RDWR|O_NOCTTY|O_NONBLOCK))==-1) {perror("open"); return 0;}
		usleep(300000);
        // ponemos las cosas del puerto serie en su sitio
        bzero(&term,sizeof(term)); // todos los flags en cero por defecto
        term.c_cflag =CS8|CLOCAL| CREAD; // baud, bits, sin control de flujo, RX on
	term.c_ispeed=term.c_ospeed=baudtable[i][0];
		term.c_cc[VMIN]=1; // sin esto estamos siempre en un modo no bloqueante
        if (tcsetattr(fd,TCSANOW,&term)) { fprintf(stderr,"Fallo en tcsetattr1\n"); exit(0); }
        // Flush input 
        if (tcflush(fd,TCIFLUSH)) { fprintf(stderr,"Fallo en tcflush\n"); exit(0); }

        // Intentamos Reset automático
		// Pines: reset: /DTR, bootloader: /RTS
        printf("\n--------------------- LPCxxxx ISP --------------------\n");
        if(ioctl(fd, TIOCMGET, &i)) {fprintf(stderr,"Fallo en ioctl TIOCMGET\n"); exit(0);}
        i|=TIOCM_DTR|TIOCM_RTS; // Reset y Bootloader L
        if(ioctl(fd, TIOCMSET, &i)) {fprintf(stderr,"Fallo en ioctl TIOCMSET\n"); exit(0);}
        usleep(300000);
		if (imn || isp) {
			//printf("ISP\n");
	        i&=~TIOCM_DTR;        // Reset H
	        if(ioctl(fd, TIOCMSET, &i)) {fprintf(stderr,"Fallo en ioctl TIOCMSET\n"); exit(0);}
	        usleep(300000);
	        i&=~TIOCM_RTS;        // bootloader H
	        if(ioctl(fd, TIOCMSET, &i)) {fprintf(stderr,"Fallo en ioctl TIOCMSET\n"); exit(0);}
		} else {
			//printf("TERM\n");
	        i&=~TIOCM_RTS;        // bootloader H
	        if(ioctl(fd, TIOCMSET, &i)) {fprintf(stderr,"Fallo en ioctl TIOCMSET\n"); exit(0);}
			usleep(300000);
	        i&=~TIOCM_DTR;        // Reset H
	        if(ioctl(fd, TIOCMSET, &i)) {fprintf(stderr,"Fallo en ioctl TIOCMSET\n"); exit(0);}
		}
		// Si sólo necesitamos un terminal saltamos todo este lío
		if (!imn && !isp) goto terminal;

		// Sincronizando con ISP
		// stdin no bloqueante, no CANONICAL, para poder abortar pulsando [esc]
		i=fcntl(0,F_GETFL); fcntl(0,F_SETFL,i|O_NONBLOCK);
        tcgetattr(0,&term0);
        term0.c_lflag&=~ICANON;
        term0.c_lflag&=~ECHO;
        if (tcsetattr(0,TCSANOW,&term0)) {fprintf(stderr,"tcsetattr failed\n"); return 0;}
        sprintf(buf,"Synchronized\r\n");
        for(k=0;;k++) {
            tcflush(fd, TCIOFLUSH);		// vacía cola RX
			while (read(0,&a,1)==1);
            a='?'; write(fd,&a,1);              // Caracter "?" para autobaud
            usleep(200000);		// tiempo suficiente para recibir la respuesta
			// el micro debe responder con "Synchronized\r\n"
			// esperamos por respuesta
            for (p=buf;;) {
				j=read(0,&a,1);
				if (j && a==27) {	// Escape pulsado => Abortar
					// stdin bloqueante, modo canonical
					i=fcntl(0,F_GETFL); fcntl(0,F_SETFL,i&(~O_NONBLOCK));
        			term0.c_lflag|=ICANON;
        			term0.c_lflag|=ECHO;
			        if (tcsetattr(0,TCSANOW,&term0)) fprintf(stderr,"tcsetattr failed\n");
					return 0; // retorna file descriptor no válido
				}
                j=read(fd,&a,1);  // respuesta del LPC
                if (j!=1)  break; // sin respuesta: reintentar
                if (a!=*p) {printf("basura\n"); continue;} // basura: ignorar
                if (*(++p)==0) goto syn2;	// Mensaje correcto: continuamos
            }
			// dibujamos barra rotatoria entre reintentos
            putchar('\r'); putchar(anim[k&3]); fflush(stdout);
        }
syn2:   // Continuamos con la carga del bootloader 2
		// stdin bloqueante, CANONICAL
		i=fcntl(0,F_GETFL); fcntl(0,F_SETFL,i&(~O_NONBLOCK));
		term0.c_lflag|=ICANON;
        term0.c_lflag|=ECHO;
		if (tcsetattr(0,TCSANOW,&term0)) fprintf(stderr,"tcsetattr failed\n");
		// UART bloqueante a partir de ahora
		i=fcntl(fd,F_GETFL); fcntl(fd,F_SETFL,i&(~O_NONBLOCK));

        safewrite(fd,buf,strlen(buf));  // Escribe "Synchronized\r\n" en UART
        i=ugets(fd,buf);      		// elimina eco de la respuesta
        printf("\r>%s<\n",buf);
        i=ugets(fd,buf);
        printf(">%s<\n",buf);
        sprintf(buf,"%d\r\n",CRYSTAL/1000);	// Escribe frecuencia del cristal en kHz
        safewrite(fd,buf,strlen(buf));
        i=ugets(fd,buf);      		// elimina eco de la respuesta
        printf(">%s<\n",buf);
        i=ugets(fd,buf);
        printf(">%s<\n",buf);

        sprintf(buf,"A 0\r\n");		// Comando quitar eco
        safewrite(fd,buf,strlen(buf));
        i=ugets(fd,buf);      		// elimina eco de la respuesta por última vez
        printf(">%s<\n",buf);
        i=ugets(fd,buf);
        printf(">%s<\n",buf);

        sprintf(buf,"U 23130\r\n");	// Comando Unlock programming
        safewrite(fd,buf,strlen(buf));
        printf("%s",buf);
        i=ugets(fd,buf);
        printf(">%s<\n",buf);

        sprintf(buf,"J\r\n");  		// IDentify
        write(fd,buf,strlen(buf));
        printf("%s",buf);
        i=ugets(fd,buf);
        printf(">%s<\n",buf);
        i=ugets(fd,buf);
        printf(">%s<",buf);
	    i=atoi(buf);
		for (k=0;;k++) {
			if (!LPCtypes[k].id || LPCtypes[k].id==i) break;
		}
		if (LPCtypes[k].ChipVariant==CHIP_VARIANT_LPC11XX) cortex=1;
		if (LPCtypes[k].ChipVariant==CHIP_VARIANT_LPC13XX ||
			LPCtypes[k].ChipVariant==CHIP_VARIANT_LPC17XX) cortex=4;
		printf("\t(Model: LPC%s, %d kbytes RAM, core: %s",
			LPCtypes[k].Product,LPCtypes[k].RAMSize,(cortex)?"CORTEX-M":"ARM7TDMI"); 
		if (cortex) printf("%d)\n",cortex-1); else printf(")\n");

	    /////////////// Modo terminal de ISP ///////////////
        if (isp) {
        	sprintf(buf,"A 1\r\n"); // pone eco
        	safewrite(fd,buf,strlen(buf));
        	printf("%s",buf);
        	goto ispterm;
        }
		// La dirección base y tamaño depende del tipo de micro
		if (cortex) {
			dir=0x10000000;		// comienzo de RAM
			j=sizeof(boot2m);	// tamaño del bootloader
			p=boot2m;			// puntero al código
		}else{
			dir=0x40000000;		// comienzo de RAM
			j=sizeof(boot2);	// tamaño del bootloader
			p=boot2;			// puntero al código
		}
		// Ahora pasamos el código del bootloader a RAM
        j=((j+11)/12)*12; 		// pad a múltiplo de 12 bytes
        //dir+=512;		 		// El comienzo de la RAM lo usa el ISP
        dir+=0x300;		 		// El comienzo de la RAM lo usa el ISP
        sprintf(buf,"W %u %u\r\n",dir,j);     // Write to RAM
        safewrite(fd,buf,strlen(buf));
        printf("%s--uuencoded data--\n",buf);
        k=uuenc(fd,p,j);		// Manda el código codificado con uuencode
        sprintf(buf,"%u\r\n",k);	// seguido del checksum
        safewrite(fd,buf,strlen(buf));
        printf("%s",buf);
        ugets(fd,buf);
        printf(">%s<\n",buf);
        ugets(fd,buf);
        printf(">%s<\n",buf);

		// Comando Goto: Ejecuta el bootloader 2
        sprintf(buf,"G %u %c\r\n",dir+32,(cortex)?'T':'A');	// modo thumb en cortex-m
        safewrite(fd,buf,strlen(buf));
        printf("%s",buf);
        ugets(fd,buf);
        printf(">%s<\n",buf);

		////////////////////////////////////////////////////////////////////////////
        ///// A partir de ahora hablamos con el BOOTLOADER 2 y todo es más simple:
		/////   Los datos se pasan en binario crudo
		/////   Podemos ocupar la parte inferior de la RAM sin problemas

        printf("-------------------- Bootloader 2 --------------------\n");
        // Sincronizamos de nuevo. Primero limpiamos la cola de recepción
        i=fcntl(fd,F_GETFL); fcntl(fd,F_SETFL,i|O_NONBLOCK); 
        do {i=read(fd,&a,1);} while (i==1);
		fcntl(fd,F_SETFL,O_RDWR);
        j=0;
        do {
			// Mandamos una marca de sincronismo (0x80,0xAA,0x33,0x55)
            buf[0]=0x80; buf[1]=0xaa; buf[2]=0x33; buf[3]=0x55;
            safewrite(fd,buf,4);
			// esperamos respuesta 0x80
            fcntl(fd,F_SETFL,O_RDWR|O_NONBLOCK);
            for (i=a=0;i<10;i++) {
                if (read(fd,&a,1)==1) break;
                usleep(100000);
            }
            i=fcntl(fd,F_GETFL); fcntl(fd,F_SETFL,i&=~O_NONBLOCK);
            if (a!=0x80) {putchar('\r'); putchar(anim[j&3]); fflush(stdout); j++;}
            else puts("\rSYNC OK\n");
        } while (a!=0x80);

        // Copiamos en RAM el código definitivo
        if((fdin=open(imn,O_RDONLY))<=0) {perror("open code.bin"); exit(1);}
		dir=dbase;
		if (!dir) dir=(cortex)?0x10000000:0x40000000;
        for(j=0;;) {
            i=read(fdin,buf,1024);
			// cortex-m:
			// Sacamos la dirección de comienzo de la tabla de vectores (offset=4)
			if(j==0) execaddr=buf[4]|(buf[5]<<8)|(buf[6]<<16)|(buf[7]<<24);
            printf("\r%08X",dir); fflush(stdout);
            if (i<=0) break;
            a=0x81; write(fd,&a,1);	// Comando write to RAM (0x81)
            bn[0]=dir&0xff; bn[1]=(dir>>8)&0xff; bn[2]=(dir>>16)&0xff; bn[3]=(dir>>24)&0xff;
			safewrite(fd,bn,4);		// dirección de carga
            bn[0]=i&0xff; bn[1]=(i>>8)&0xff; bn[2]=(i>>16)&0xff; bn[3]=(i>>24)&0xff;
			safewrite(fd,bn,4);		// nº de bytes a cargar
			safewrite(fd,buf,i);	// datos
            read(fd,&a,1);			// leemos respuesta
            if (a!=0x80) { printf("Bad ACK\n"); exit(2);}
            dir+=i;
            j+=i;
        }
        printf("\nLoad code.bin OK (%d bytes)\n",j);
        close(fdin);

        // Ejecutamos el código cargado
		dir=dbase;
        if (!dir) dir=(cortex)?execaddr:0x40000000;
        a=0x82; write(fd,&a,1);		// Comando execute
        bn[0]=dir&0xff; bn[1]=(dir>>8)&0xff; bn[2]=(dir>>16)&0xff; bn[3]=(dir>>24)&0xff;
        safewrite(fd,bn,4);				// Dirección de salto
        read(fd,&a,1);				// respuesta
        if (a!=0x80) { printf("Bad ACK\n"); exit(2);}
        printf("EXEC new code at 0x%08X\n",dir);


        //////////////////////////////////////////////////
        // Terminal
        //////////////////////////////////////////////////
terminal:
        printf("------------------------- Terminal ------------------------\n");
#ifdef __linux__
        for (i=0;baudtable[i][0];i++) {
            if (baud==baudtable[i][0]) break;
        }
        if (!baudtable[i][0]) {printf("Velocidad no soportada\n"); exit(0);}
#else
		baud=bbaud;
#endif
        printf("Terminal: Velocidad: %d baudios paridad=%d\n",baud,paridad);
        term.c_ispeed=term.c_ospeed=baudtable[i][0];
		if (paridad) {
			term.c_cflag|=PARENB;
			if (paridad&1) term.c_cflag|=PARODD;
		}
        if (tcsetattr(fd,TCSANOW,&term)) {
                fprintf(stderr,"Fallo en tcsetattr2\n");
                exit(0);
        }
ispterm:
        printf("-----------------------------------------------------------\n");
        // non blocking uart
        i=fcntl(fd,F_GETFL); fcntl(fd,F_SETFL,i|O_NONBLOCK);

        // STDIN no bloqueante, no canon, no eco
        i=fcntl(0,F_GETFL); fcntl(0,F_SETFL,i|O_NONBLOCK);
        tcgetattr(0,&term);
        term.c_lflag&=~ICANON;
        term.c_lflag&=~ECHO;
        if (tcsetattr(0,TCSANOW,&term)) {
                fprintf(stderr,"Fallo en tcsetattr\n");
                exit(0);
        }

		// Crea fichero de log si se pide
		if (logn) {
			if ((fdlog=creat(logn,0666))<=0) {perror("creat log"); exit(0);}
		}

        fdin=0; // por defecto leemos de stdin
        for (;;) {
                sl=2; // contador para dormir el proceso
                if (read(fd,&a,1)==1) { // datos desde micro
                        if (hex) {printf("%02X ",a); fflush(stdout);}
                        else write(1,&a,1);
						if (logn) write(fdlog,&a,1); // Fichero de log
                } else sl--;
                j=read(fdin,&a,1);	// datos desde stdin o fichero
                if (j==0 && fdin) {
                        close(fdin);
                        fdin=0; // Si se acabo el fichero pasamos a stdin
                }
                if (j==1) {
                        switch (a) {
                        case    27:	// <esc> => salir de bt2
                                // ponemos las cosas de STDIN como estaban
                                tcgetattr(0,&term);
                                term.c_lflag|=ICANON;
                                term.c_lflag|=ECHO;
                                if (tcsetattr(0,TCSANOW,&term))
                                	fprintf(stderr,"Fallo en tcsetattr\n");
								if(logn) close(fdlog);
                                exit(0);
                        case    'X'-64: a=3; // <ctrl>-X se traduce a <ctrl>-C
                                break;
                        case    'P'-64: buf[0]=1; buf[1]=2; buf[2]=4; buf[3]=8; buf[4]=16; buf[5]=0x1F;
										write(fd,buf,6); 
                                break;
                        case    '\n':   if (crlf) a='\r'; // LF -> CR,LF si necesario
                                break;
                        case    'F'-64: 	// <ctrl>-F => "teclear" fichero
								printf("\nInclude File: ");
                                fflush(stdout);
                                tcgetattr(0,&term);	// stdin bloqueante y con eco
                                term.c_lflag|=ECHO;
                                tcsetattr(0,TCSANOW,&term);
                                i=fcntl(0,F_GETFL); fcntl(0,F_SETFL,i&(~O_NONBLOCK));
								// lee de stdin el nombre del fichero
                                fgets(fn,255,stdin); fn[strlen(fn)-1]=0;
								// stdin de nuevo no bloqueante y sin eco
                                fcntl(0,F_SETFL,i|O_NONBLOCK);
                                term.c_lflag&=~ECHO;
                                tcsetattr(0,TCSANOW,&term);
								// Abrimos el fichero. 
                                if ((fdin=open(fn,O_RDONLY))==-1) fdin=0;
								//A partir de ahora se leen datos del fichero en lugar
								//de stdin
                                continue;
                        }
                        write(fd,&a,1); usleep(1000); 	// retardo entre caracteres para no
														// desbordar al receptor
						// CR,LF
                        if (a=='\r') {a='\n'; write(fd,&a,1); usleep(5000);}
                } else sl--;
                if (!sl) usleep(40000); // A dormir si nadie tiene datos disponibles
        }
}

