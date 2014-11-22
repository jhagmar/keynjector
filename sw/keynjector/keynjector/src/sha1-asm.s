/*
* Copyright 2014 Jonas Hagmar
*
* This file is part of keynjector.
*
* keynjector is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* keynjector is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with keynjector.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifdef SHA1_ASM
	.syntax		unified

	.align 2
.sha1_C:
	.word		 0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6

	.global		sha1_process
	.type		sha1_process, %function
sha1_process:

	.macro		copy_m
	ldmdb		r1!, {r2-r8, ip}
	rev		r2, r2
	rev		r3, r3
	rev		r4, r4
	rev		r5, r5
	rev		r6, r6
	rev		r7, r7
	rev		r8, r8
	rev		ip, ip
	stmdb		sp!, {r2-r8, ip}
	.endm

	.macro		update_w, i1, i2, i3, i4
	ldr		r7, [sp, \i1]
	ldr		r8, [sp, \i2]
	ldr		ip, [sp, \i3]
	ldr		lr, [sp, \i4]
	eor		r7, r7, r8
	eor		ip, ip, lr
	eor		r7, r7, ip
	mov		r7, r7, ror #31
	str		r7, [sp, \i1]
	.endm

	.macro		update_ws
	update_w	#0,	#52,	#32,	#8
	update_w	#4,	#56,	#36,	#12
	update_w	#8,	#60,	#40,	#16
	update_w	#12,	#0,	#44,	#20
	update_w	#16,	#4,	#48,	#24
	update_w	#20,	#8,	#52,	#28
	update_w	#24,	#12,	#56,	#32
	update_w	#28,	#16,	#60,	#36
	update_w	#32,	#20,	#0,	#40
	update_w	#36,	#24,	#4,	#44
	update_w	#40,	#28,	#8,	#48
	update_w	#44,	#32,	#12,	#52
	update_w	#48,	#36,	#16,	#56
	update_w	#52,	#40,	#20,	#60
	update_w	#56,	#44,	#24,	#0
	update_w	#60,	#48,	#28,	#4
	.endm

	.macro		sha_iter_1, a, b, c, d, e, w
	eor		r7, \c, \d
	and		r7, \b, r7, ror #2
	eor		r7, r7, \d, ror #2
	ldr		r8, [sp, \w]
	add		\e, r7, \e, ror #2
	add		\e, \e, \a, ror #27
	add		\e, \e, r6
	add		\e, \e, r8
	.endm

	.macro		sha_iter_2, a, b, c, d, e, w
	eor		r7, \c, \d
	eor		r7, \b, r7, ror #2
	ldr		r8, [sp, \w]
	add		\e, r7, \e, ror #2
	add		\e, \e, \a, ror #27
	add		\e, \e, r6
	add		\e, \e, r8
	.endm

	.macro		sha_iter_3, a, b, c, d, e, w
	orr		r7, \b, \c, ror #2
	and		r7, r7, \d, ror #2
	and		ip, \b, \c, ror #2
	orr		r7, r7, ip
	ldr		r8, [sp, \w]
	add		\e, r7, \e, ror #2
	add		\e, \e, \a, ror #27
	add		\e, \e, r6
	add		\e, \e, r8
	.endm
	
	push		{r4-r8, lr}

	add		r1, #64	
	copy_m
	copy_m

	ldmia		r0, {r1-r5}

	mov		r3, r3, ror #30
	mov		r4, r4, ror #30
	mov		r5, r5, ror #30

	ldr		r6, .sha1_C

	sha_iter_1	r1, r2, r3, r4, r5, #0
	sha_iter_1	r5, r1, r2, r3, r4, #4
	sha_iter_1	r4, r5, r1, r2, r3, #8
	sha_iter_1	r3, r4, r5, r1, r2, #12
	sha_iter_1	r2, r3, r4, r5, r1, #16
	sha_iter_1	r1, r2, r3, r4, r5, #20
	sha_iter_1	r5, r1, r2, r3, r4, #24
	sha_iter_1	r4, r5, r1, r2, r3, #28
	sha_iter_1	r3, r4, r5, r1, r2, #32
	sha_iter_1	r2, r3, r4, r5, r1, #36
	sha_iter_1	r1, r2, r3, r4, r5, #40
	sha_iter_1	r5, r1, r2, r3, r4, #44
	sha_iter_1	r4, r5, r1, r2, r3, #48
	sha_iter_1	r3, r4, r5, r1, r2, #52
	sha_iter_1	r2, r3, r4, r5, r1, #56
	sha_iter_1	r1, r2, r3, r4, r5, #60

	update_ws

	sha_iter_1	r5, r1, r2, r3, r4, #0
	sha_iter_1	r4, r5, r1, r2, r3, #4
	sha_iter_1	r3, r4, r5, r1, r2, #8
	sha_iter_1	r2, r3, r4, r5, r1, #12

	ldr		r6, .sha1_C + 4

	sha_iter_2	r1, r2, r3, r4, r5, #16
	sha_iter_2	r5, r1, r2, r3, r4, #20
	sha_iter_2	r4, r5, r1, r2, r3, #24
	sha_iter_2	r3, r4, r5, r1, r2, #28
	sha_iter_2	r2, r3, r4, r5, r1, #32
	sha_iter_2	r1, r2, r3, r4, r5, #36
	sha_iter_2	r5, r1, r2, r3, r4, #40
	sha_iter_2	r4, r5, r1, r2, r3, #44
	sha_iter_2	r3, r4, r5, r1, r2, #48
	sha_iter_2	r2, r3, r4, r5, r1, #52
	sha_iter_2	r1, r2, r3, r4, r5, #56
	sha_iter_2	r5, r1, r2, r3, r4, #60

	update_ws

	sha_iter_2	r4, r5, r1, r2, r3, #0
	sha_iter_2	r3, r4, r5, r1, r2, #4
	sha_iter_2	r2, r3, r4, r5, r1, #8
	sha_iter_2	r1, r2, r3, r4, r5, #12
	sha_iter_2	r5, r1, r2, r3, r4, #16
	sha_iter_2	r4, r5, r1, r2, r3, #20
	sha_iter_2	r3, r4, r5, r1, r2, #24
	sha_iter_2	r2, r3, r4, r5, r1, #28

	ldr		r6, .sha1_C + 8

	sha_iter_3	r1, r2, r3, r4, r5, #32
	sha_iter_3	r5, r1, r2, r3, r4, #36
	sha_iter_3	r4, r5, r1, r2, r3, #40
	sha_iter_3	r3, r4, r5, r1, r2, #44
	sha_iter_3	r2, r3, r4, r5, r1, #48
	sha_iter_3	r1, r2, r3, r4, r5, #52
	sha_iter_3	r5, r1, r2, r3, r4, #56
	sha_iter_3	r4, r5, r1, r2, r3, #60

	update_ws

	sha_iter_3	r3, r4, r5, r1, r2, #0
	sha_iter_3	r2, r3, r4, r5, r1, #4
	sha_iter_3	r1, r2, r3, r4, r5, #8
	sha_iter_3	r5, r1, r2, r3, r4, #12
	sha_iter_3	r4, r5, r1, r2, r3, #16
	sha_iter_3	r3, r4, r5, r1, r2, #20
	sha_iter_3	r2, r3, r4, r5, r1, #24
	sha_iter_3	r1, r2, r3, r4, r5, #28
	sha_iter_3	r5, r1, r2, r3, r4, #32
	sha_iter_3	r4, r5, r1, r2, r3, #36
	sha_iter_3	r3, r4, r5, r1, r2, #40
	sha_iter_3	r2, r3, r4, r5, r1, #44

	ldr		r6, .sha1_C + 12

	sha_iter_2	r1, r2, r3, r4, r5, #48
	sha_iter_2	r5, r1, r2, r3, r4, #52
	sha_iter_2	r4, r5, r1, r2, r3, #56
	sha_iter_2	r3, r4, r5, r1, r2, #60

	update_ws

	sha_iter_2	r2, r3, r4, r5, r1, #0
	sha_iter_2	r1, r2, r3, r4, r5, #4
	sha_iter_2	r5, r1, r2, r3, r4, #8
	sha_iter_2	r4, r5, r1, r2, r3, #12
	sha_iter_2	r3, r4, r5, r1, r2, #16
	sha_iter_2	r2, r3, r4, r5, r1, #20
	sha_iter_2	r1, r2, r3, r4, r5, #24
	sha_iter_2	r5, r1, r2, r3, r4, #28
	sha_iter_2	r4, r5, r1, r2, r3, #32
	sha_iter_2	r3, r4, r5, r1, r2, #36
	sha_iter_2	r2, r3, r4, r5, r1, #40
	sha_iter_2	r1, r2, r3, r4, r5, #44
	sha_iter_2	r5, r1, r2, r3, r4, #48
	sha_iter_2	r4, r5, r1, r2, r3, #52
	sha_iter_2	r3, r4, r5, r1, r2, #56
	sha_iter_2	r2, r3, r4, r5, r1, #60

	ldmia		r0, {r6-r8, ip, lr}
	add		r1, r6, r1
	add		r2, r7, r2
	add		r3, r8, r3, ror #2
	add		r4, ip, r4, ror #2
	add		r5, lr, r5, ror #2
	stmia		r0, {r1-r5}

	add		sp, #64
	
	pop		{r4-r8, pc}

#endif

/*	.align 2
.sha1_initial:
	.word		0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0

	.global		sha1_init
	.type		sha1_init, %function
sha1_init:

	adr		r1, .sha1_initial
	ldmia		r1!, {r2, r3, ip}
	stmia		r0!, {r2, r3, ip}
	ldmia		r1!, {r2, r3}
	stmia		r0!, {r2, r3}
	eor		r1, r1, r1
	str		r1, [r0, #64]
	str		r1, [r0, #68]
	bx		lr
*/
