
lowlevel_init.o：     文件格式 elf32-littlearm


Disassembly of section .text:

00000000 <lowlevel_init>:
   0:	e59f0038 	ldr	r0, [pc, #56]	; 40 <lowlevel_init+0x40>
   4:	e3a01c01 	mov	r1, #256	; 0x100
   8:	e5801000 	str	r1, [r0]
   c:	e59f0030 	ldr	r0, [pc, #48]	; 44 <lowlevel_init+0x44>
  10:	e3a01000 	mov	r1, #0
  14:	e5801000 	str	r1, [r0]
  18:	e59f0028 	ldr	r0, [pc, #40]	; 48 <lowlevel_init+0x48>
  1c:	e3a01000 	mov	r1, #0
  20:	e0400001 	sub	r0, r0, r1
  24:	e3a01312 	mov	r1, #1207959552	; 0x48000000
  28:	e2802034 	add	r2, r0, #52	; 0x34
  2c:	e4903004 	ldr	r3, [r0], #4
  30:	e4813004 	str	r3, [r1], #4
  34:	e1520000 	cmp	r2, r0
  38:	1afffffb 	bne	2c <lowlevel_init+0x2c>
  3c:	e1a0f00e 	mov	pc, lr
  40:	56000050 	.word	0x56000050
  44:	56000054 	.word	0x56000054
  48:	0000004c 	.word	0x0000004c

0000004c <SMRDATA>:
  4c:	22011110 	.word	0x22011110
  50:	00000700 	.word	0x00000700
  54:	00000700 	.word	0x00000700
  58:	00000700 	.word	0x00000700
  5c:	00000700 	.word	0x00000700
  60:	00000740 	.word	0x00000740
  64:	00000700 	.word	0x00000700
  68:	00018005 	.word	0x00018005
  6c:	00018005 	.word	0x00018005
  70:	008c04f4 	.word	0x008c04f4
  74:	000000b1 	.word	0x000000b1
  78:	00000030 	.word	0x00000030
  7c:	00000030 	.word	0x00000030
