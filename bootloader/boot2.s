		.arm
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
intro:	mov		r12,    #0xe0000000 @ UART0 base:  E000C000
		orr		r12,r12,#0x0000c000
		orr		r11,r12,#0x001F0000 @ SCB base:    E01FC000
		orr		r10,r12,#0x00020000 @ Pinsel base: E002C000
		@ obtenemos el final de la RAM alineando SP a 1KB
		mov		r3,#1024
		sub		r3,r3,#1
		add		r2,r3,sp
		bic		r2,r2,r3
		@ precargamos valores en registros antes de reubicar
		ldr		r8,syncw	    	@ Marca de sincronismo
		mov		r7,#0x80	   		@ ACK
		@ reubicación del código
		adr		r1,bend		    	@ final del codigo
		adr		r0,bbeg		    	@ principio del codigo
1:		ldr		r3,[r1,#-4]!		@ reubica codigo
		str		r3,[r2,#-4]!
		cmp		r1,r0
		bne		1b
		mov		pc,r2		    	@ y salta a la nueva direccion

syncw:	.word	0x80AA3355

@ el codigo anterior ya no es necesario una vez se salte al bootloader.
@ se puede sobreescribir con la aplicacion objeto de carga

@------------------------------------------------------------------
@    Este codigo va al final de la RAM. 
@    son 212 bytes.
@    La RAM se puede llenar hasta 164 bytes por debajo de su final,
@	 pisando parte del código del principio
@------------------------------------------------------------------

bbeg:
1:		bl		ugetch			@ Sync
		orr		r3,r1,r3,lsl #8
		cmp		r3,r8
		bne		1b
		strb	r7,[r12]		@ ACK

2:		bl		ugetch			@ commands
21:		cmp		r1,#0x81
		bne		3f
		@ write to RAM
		bl		uget32			@ get address
		mov		r4,r0
		bl		uget32			@ get count
		mov		r5,r0
25:		bl		ugetch			@ get data
		strb	r1,[r4],#1
		subs	r5,r5,#1
		bne		25b
		strb	r7,[r12]		@ ACK
		bl		ugetch

		cmp		r1,#0x82
		bne		21b
		@ Execute at address
3:		bl		uget32			@ get address
		strb	r7,[r12]		@ ACK
		@delay (~40ms)
		mov		r1,#(144*1024)
35:		subs	r1,r1,#1
		bne		35b
		@ back to almost reset state
		mov		r2,#0xAA
		mov		r3,#0x55
		mov		r4,#1
		str		r4,[r11,#0x80]	@ PLLCON (disconect PLL)
		str		r2,[r11,#0x8C]	@ PLLFEED
		str		r3,[r11,#0x8C]	@ PLLFEED
		str		r1,[r11,#0x80]	@ PLLCON (PLL off)
		str		r2,[r11,#0x8C]	@ PLLFEED
		str		r3,[r11,#0x8C]	@ PLLFEED
		str		r1,[r11,#0x1A0]	@ SCS
		mov		r4,#2
		str		r4,[r11,#0x40]	@ MEMMAP
		str		r1,[r10]		@ PINSEL0
		str		r1,[r12,#0x8]	@ U0FCR
		str		r1,[r12,#0xC]	@ U0LCR

		@ and jump to the code
		bx		r0

ugetch:	mov		r2,#1			@ retorna dato en R1
		b		1f	
uget32:	mov		r2,#4			@ retorna dato en R0
1:		ldrb	r1,[r12,#0x14]
		tst		r1,#0x1
		beq		1b
		ldrb	r1,[r12]
		mov		r0,r0,lsr #8
		orr		r0,r0,r1,lsl #24
		subs	r2,r2,#1
		bne		1b
		mov		pc,lr

bend:

