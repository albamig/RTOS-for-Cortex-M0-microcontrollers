		.thumb
		.set _start, 0
		.global _start

		@ 32 bytes de relleno (el bootloader ISP los pisa)
		.word	0
		.word	0
		.word	0
		.word	0
		.word	0
		.word	0
		.word	0
		.word	0

		@ direcion de carga + 32 : Entrada al codigo de reubicacion
		@ Reubica bootloader e inicializa variables
intro:	
		@ MEMMAP a RAM
		ldr		r0,mmapbase		
		mov		r1,#1
		str		r1,[r0]				
		@ Obtenemos el final de la RAM a partir de SP
		mov		r2,sp				
		lsl		r3,r1,#10			@ R3 = 1023
		sub		r3,#1
		add		r2,r3				@ R2 = SP alineado a 1KB
		bic		r2,r3
		mov 	sp,r2				@ SP al final de la RAM (SP no se usa aquí)
		@ Precargamos valores en registros antes de reubicar
		ldr		r7,uartbase 		@ R7: UART0 base:  40008000
		ldr		r6,syncw	    	@ Marca de sincronismo
		mov		r5,#0x80	   		@ ACK
		@ Reubicación de código
		adr		r1,bend		    	@ final del codigo
		adr		r0,bbeg		    	@ principio del codigo
1:		sub		r1,#4				
		ldr		r3,[r1]		
		sub		r2,#4
		str		r3,[r2]
		cmp		r1,r0
		bne		1b
		mov		r4,#0
		mov		pc,r2		@ y salta a la nueva direccion

		.align	2
uartbase:	.word	0x40008000
mmapbase:	.word	0x40048000
syncw:		.word	0x80AA3355

@ el codigo anterior ya no es necesario una vez se salte al bootloader.
@ se puede sobreescribir con la aplicacion objeto de carga

@------------------------------------------------------------------
@    Este codigo va al final de la RAM. 
@    son 108 bytes.
@    La RAM se puede llenar hasta 74 bytes por debajo de su final
@	 pisando parte del código del principio
@------------------------------------------------------------------

bbeg:
1:		bl		ugetch			@SYNC
		lsl		r4,#8
		orr		r4,r1
		cmp		r4,r6
		bne		1b
		strb	r5,[r7]			@ACK

2:		bl		ugetch			@comandos
3:		cmp		r1,#0x81		
		bne		4f
		@write to RAM
		bl		uget32
		mov		r4,r0			@address
		bl		uget32
		mov		r6,r0			@count
5:		bl		ugetch			@ get data
		strb	r1,[r4]
		add		r4,#1
		sub 	r6,#1
		bne		5b
		strb	r5,[r7]			@ACK
		bl		ugetch
		cmp 	r1,#0x82
		bne		3b
		@execute at address
4:		bl		uget32
		strb	r5,[r7]			@ACK
		@delay
		mov		r1,#144
		lsl		r1,#10
6:		sub		r1,#1
		bne		6b
		@ back to reset state
		mov		r1,#0x10
		add		r7,r1
		strb 	r1,[r7,#0x18]	@ UART Fractional divider off
		@ and jump to the code
		bx		r0
	
ugetch: mov		r2,#1	@ dato retornado en R1
		b		1f
uget32:	mov		r2,#4	@ dato retornado en R0
1:		mov		r3,#1
		ldrb	r1,[r7,#0x14]
		tst		r1,r3
		beq		1b
		ldrb	r1,[r7]
		lsr		r0,#8
		lsl		r3,r1,#24
		orr		r0,r3
		sub		r2,#1
		bne		1b
		mov		pc,lr

		.align	2
bend:

