/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  armboot - Startup Code for ARM920 CPU-core
 *
 *  Copyright (c) 2001	Marius Gröger <mag@sysgo.de>
 *  Copyright (c) 2002	Alex Züpke <azu@sysgo.de>
 *  Copyright (c) 2002	Gary Jennejohn <garyj@denx.de>
 */

#include <asm-offsets.h>
#include <common.h>
#include <config.h>

/*
 *************************************************************************
 *
 * Startup Code (called from the ARM reset exception vector)
 *
 * do important init only if we don't start from memory!
 * relocate armboot to ram
 * setup stack
 * jump to second stage
 *
 *************************************************************************
 */

	.globl	reset

reset:
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr, r0

#if	defined(CONFIG_AT91RM9200DK) || defined(CONFIG_AT91RM9200EK)
	/*
	 * relocate exception table
	 */
	ldr	r0, =_start
	ldr	r1, =0x0
	mov	r2, #16
copyex:
	subs	r2, r2, #1
	ldr	r3, [r0], #4
	str	r3, [r1], #4
	bne	copyex
#endif

#if defined(CONFIG_S3C2440)
#define WTCON 		0x53000000
#define INTMSK 		0x4A000008
#define INTSUBMSK 	0x4A00001C
#define CLKDIVN 	0x4C000014 
#define LOCKTIME 	0x4C000000 
#define MPLLCON 	0x4C000004

	ldr r0, =WTCON 
	mov r1, #0x0 
	str r1, [r0] 

	mov r1, #0xFFFFFFFF 
	ldr r0, =INTMSK 
	str r1, [r0]  

	ldr r1, =0xFFFFFFFF 
	ldr r0, =LOCKTIME 
	str r1, [r0] 

	ldr r0, =CLKDIVN 
	mov r1, #0x5 	// FCLK:HCLK:PCLK = 1:4:8
	str r1, [r0]

	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #0xC0000000
	mcr p15, 0, r0, c1, c0, 0

	ldr r1, =(0x5C << 12 | 0x01 << 4 | 0x01)  	// FCLK = 4000MHz
	ldr r0, =MPLLCON 
	str r1, [r0] 

	mrc p15, 0, r0, c1, c0, 0 
	orr r0, r0, #(1 << 12)
	mcr p15, 0, r0, c1, c0, 0

#endif /* CONFIG_S3C2440 */

	/*
	 * we do sys-critical inits only at reboot,
	 * not when booting from ram!
	 */
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	bl	cpu_init_crit
#endif

	bl	_main

/*------------------------------------------------------------------------------*/

	.globl	c_runtime_cpu_setup
c_runtime_cpu_setup:

	mov	pc, lr

/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */


#ifndef CONFIG_SKIP_LOWLEVEL_INIT
cpu_init_crit:
	/*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002300	@ clear bits 13, 9:8 (--V- --RS)
	bic	r0, r0, #0x00000087	@ clear bits 7, 2:0 (B--- -CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (A) Align
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-Cache
	mcr	p15, 0, r0, c1, c0, 0

#ifndef CONFIG_SKIP_LOWLEVEL_INIT_ONLY
	/*
	 * before relocating, we have to setup RAM timing
	 * because memory timing is board-dependend, you will
	 * find a lowlevel_init.S in your board directory.
	 */
	mov	ip, lr

	bl	lowlevel_init
	mov	lr, ip
#endif
	mov	pc, lr
#endif /* CONFIG_SKIP_LOWLEVEL_INIT */
