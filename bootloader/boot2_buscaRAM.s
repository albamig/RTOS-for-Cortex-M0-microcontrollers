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
		@ buscamos el final de la RAM mediante un DATA abort
		mov		r3,#2
		str		r3,[r11,#0x40]		@ MEMMAP a RAM
		adr		r0,abtrut
		mov		r2,#0x40000000		@ comienzo de RAM
		ldr		r3,opcode			@ opcode de (mov pc,r0)
		str		r3,[r2,#16]!		@ vector de data_abort
100:	ldr		r3,[r2,#4]!			@ escaneamos RAM. Salimos de bucle por data_abort
		b		100b		

abtrut:	@ tras data abort tenemos en R2 el final de la RAM
		msr		cpsr_c,#0xdf		@ modo SYS de nuevo
		ldr		r8,syncw	    	@ Marca de sincronismo
		mov		r7,#0x80	   		@ ACK
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
@    son 56 instr, 224 bytes.
@    La RAM se puede llenar hasta 176 bytes por debajo de su final
@	 pisando parte del código del principio
@------------------------------------------------------------------

bbeg:
1:		bl		ugetch			@ Sync
		orr		r1,r0,r1,lsl #8
		cmp		r1,r8
		bne		1b
		strb	r7,[r12]		@ ACK

2:		bl		ugetch			@ commands
21:		cmp		r0,#0x81
		bne		3f
		@ write to RAM
		bl		uget32			@ get address
		mov		r4,r0
		bl		uget32			@ get count
		mov		r5,r0
25:		bl		ugetch			@ get data
		strb	r0,[r4],#1
		subs	r5,r5,#1
		bne		25b
		strb	r7,[r12]		@ ACK
		bl		ugetch

		cmp		r0,#0x82
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
opcode:	mov		pc,r0
	
uget32:	mov		r2,#4
1:		ldrb	r1,[r12,#0x14]
		tst		r1,#0x1
		beq		1b
		ldrb	r1,[r12]
		mov		r0,r0,lsr #8
		orr		r0,r0,r1,lsl #24
		subs	r2,r2,#1
		bne		1b
		mov		pc,lr

ugetch:	ldrb	r0,[r12,#0x14]
		tst		r0,#0x1
		beq		ugetch
		ldrb	r0,[r12]
		mov		pc,lr

bend:

