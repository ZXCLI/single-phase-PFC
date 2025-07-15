@ DCL_DF22_C1.s - Full Direct Form 2 implementation in second order
@
@#############################################################################
@!
@! Copyright: Copyright (C) 2023 Texas Instruments Incorporated -
@!	All rights reserved not granted herein.
@!	Limited License.
@!
@! Texas Instruments Incorporated grants a world-wide, royalty-free,
@! non-exclusive license under copyrights and patents it now or hereafter
@! owns or controls to make, have made, use, import, offer to sell and sell
@! ("Utilize") this software subject to the terms herein. With respect to the
@! foregoing patent license, such license is granted solely to the extent that
@! any such patent is necessary to Utilize the software alone. The patent
@! license shall not apply to any combinations which include this software,
@! other than combinations with devices manufactured by or for TI 
@! ("TI Devices").
@! No hardware patent is licensed hereunder.
@!
@! Redistributions must preserve existing copyright notices and reproduce this
@! license (including the above copyright notice and the disclaimer and
@! (if applicable) source code license limitations below) in the documentation
@!  and/or other materials provided with the distribution.
@!
@! Redistribution and use in binary form, without modification, are permitted
@! provided that the following conditions are met:
@!
@! * No reverse engineering, decompilation, or disassembly of this software is 
@! permitted with respect to any software provided in binary form.
@! * Any redistribution and use are licensed by TI for use only 
@!   with TI Devices.
@! * Nothing shall obligate TI to provide you with source code for the 
@!   software licensed and provided to you in object code.
@!
@! If software source code is provided to you, modification and redistribution
@! of the source code are permitted provided that the following conditions 
@! are met:
@!
@! * any redistribution and use of the source code, including any resulting
@!   derivative works, are licensed by TI for use only with TI Devices.
@! * any redistribution and use of any object code compiled from the source
@!   code and any resulting derivative works, are licensed by TI for use 
@!   only with TI Devices.
@!
@! Neither the name of Texas Instruments Incorporated nor the names of its
@! suppliers may be used to endorse or promote products derived from this 
@! software without specific prior written permission.
@#############################################################################

.syntax unified
.arch armv7e-m
.fpu fpv4-sp-d16
.thumb

.global DCL_runDF22_C1
.type DCL_runDF22_C1, %function

@ C prototype: float DCL_runDF22_C1(DCL_DF22 *p, float ek)
@ argument 1 = *p : 32-bit ARMA structure address [r0]
@ argument 2 = ek : 32-bit floating-point input [s0]
@ return = uk : 32-bit floating-point [s0]

.section .text
.align 2

DCL_runDF22_C1:
    vpush {s4-s7}               @ Save preserved VFP registers

    vldr s1, [r0]               @ s1 = b0
    add r0, r0, #4

    vmul.f32 s1, s1, s0         @ s1 = v0 = b0 * ek

    vldr s2, [r0]               @ s2 = b1
    add r0, r0, #4
    vmul.f32 s2, s2, s0         @ s2 = v1 = b1 * ek

    vldr s3, [r0]               @ s3 = b2
    add r0, r0, #4
    vmul.f32 s3, s3, s0         @ s3 = v2 = b2 * ek

    vldr s4, [r0]               @ s4 = a1
    add r0, r0, #4
    vldr s5, [r0]               @ s5 = a2
    add r0, r0, #4
    vldr s6, [r0]               @ s6 = x1
    add r0, r0, #4
    vldr s7, [r0]               @ s7 = x2

    vadd.f32 s0, s1, s6         @ s0 = uk = v0 + x1
    vadd.f32 s2, s2, s7         @ s2 = v1 + x2

    vmul.f32 s6, s4, s0         @ s6 = v3 = a1 * uk
    vmul.f32 s7, s5, s0         @ s7 = v4 = a2 * uk

    vsub.f32 s2, s2, s6         @ s2 = x1d = v1 + x2 - v3
    vsub.f32 s3, s3, s7         @ s3 = x2d = v2 - v4

    @ Save updated state variables
    sub r0, r0, #8              @ Point back to x1 position
    vstr s2, [r0]               @ Save x1
    add r0, r0, #4
    vstr s3, [r0]               @ Save x2

    vpop {s4-s7}                @ Restore VFP registers
    bx lr                       @ Return

.size DCL_runDF22_C1, .-DCL_runDF22_C1