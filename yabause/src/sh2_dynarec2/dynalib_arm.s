/*
; Copyright 2017 devMiyax(smiyaxdev@gmail.com)
; 
; This file is part of Yabause.
; 
; Yabause is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
; Yabause is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with Yabause; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

; r0  
; r1    
; r2  
; r3  
; r4  
; r5  
; r6
; r7   <- Address of GenReg
; r8   <- Address of PC
; r9   <- Address of SysReg
; r10  <- Address of CtrlReg
; r11
; r12 ip
; r13 sp
; r14 lr
; r15 pc


  http://www.mztn.org/slasm/arm07.html
  http://qiita.com/edo_m18/items/a7c747c5bed600dca977

*/

/*
;Memory Functions
extern _memGetByte, _memGetWord, _memGetLong
extern _memSetByte, _memSetWord, _memSetLong
extern _EachClock, _DelayEachClock, _DebugEachClock, _DebugDelayClock
*/


.macro opfunc name
.section .text 
  .global x86_\name
  x86_\name:
.endm

.macro opdesc name,b,c,d,e,f,g
.section .data
  .global \name\()_size
  \name\()_size: .long \b
  .global \name\()_src
   \name\()_src: .long \c
  .global \name\()_dest
   \name\()_dest: .long \d
  .global \name\()_off1
   \name\()_off1: .long \e
  .global \name\()_imm
   \name\()_imm: .long \f
  .global \name\()_off3
   \name\()_off3: .long \g
.endm  

.macro ctrlreg_load a     // 5
  add r0,r10, (\a*4)      // 2
  ldr	r0, [r0]
.endm

//=====================================================
// Basic
//=====================================================


.text
.align  1

//-----------------------------------------------------
// Begining of block
// r7   <- Address of GenReg
// r8   <- Address of PC
// r9   <- Address of SysReg
// r10  <- Address of CtrlReg
.global prologue
prologue:
add r0,r10      // 2

//-----------------------------------------------------
// normal part par instruction
.global seperator_normal
seperator_normal:
.size	seperator_normal, .-seperator_normal

//-----------------------------------------------------
// Delay slot part par instruction
.global seperator_delay_slot
seperator_delay_slot:
bl	SH2HandleBreakpoints

//-----------------------------------------------------
// Delay slot part par instruction
.global seperator_delay_after
seperator_delay_after:
bl	SH2HandleBreakpoints

//-------------------------------------------------------
// End of block
.global epilogue
epilogue:
mov	pc, r14

//-----------------------------------------------------
// Jump part
.global PageFlip
PageFlip:

//-------------------------------------------------------
// normal part par instruction( for debug build )
.global seperator_d_normal
seperator_d_normal:


//------------------------------------------------------
// Delay slot part par instruction( for debug build )
.global seperator_d_delay
seperator_d_delay:


opdesc CLRT,3,0,0,0,0,0
opfunc CLRT

opdesc CLRMAC,7,0,0,0,0,0
opfunc CLRMAC

opdesc NOP,		1,0,0,0,0,0
opfunc NOP

opdesc DIV0U,	6,0,0,0,0,0
opfunc DIV0U

opdesc SETT,	3,0,0,0,0,0
opfunc SETT

opdesc SLEEP,	14,0,0,0,0,0
opfunc SLEEP

opdesc SWAP_W,	19,4,15,0,0,0
opfunc SWAP_W

opdesc SWAP_B,	18,4,14,0,0,0
opfunc SWAP_B

opdesc TST,	29,4,12,0,0,0
opfunc TST


opdesc TSTI,	23,0,0,0,6,0
opfunc TSTI


opdesc ANDI,	9,0,0,0,5,0
opfunc ANDI

opdesc XORI,	9,0,0,0,5,0
opfunc XORI

opdesc ORI,	9,0,0,0,5,0
opfunc ORI

opdesc CMP_EQ_IMM,	25,0,0,0,8,0
opfunc CMP_EQ_IMM

opdesc XTRCT,	23,4,12,0,0,0
opfunc XTRCT

opdesc ADD,		16,4,12,0,0,0
opfunc ADD

opdesc ADDC,	33,4,12,0,0,0
opfunc ADDC

opdesc ADDV, 24,4,12,0,0,0
opfunc ADDV

opdesc SUBC,	33,4,12,0,0,0
opfunc SUBC

opdesc SUB,		16,4,12,0,0,0
opfunc SUB

opdesc NOT,		18,4,14,0,0,0
opfunc NOT

opdesc NEG,		18,4,14,0,0,0
opfunc NEG

opdesc NEGC,	50,4,14,0,0,0
opfunc NEGC

opdesc EXTUB,	21,4,17,0,0,0
opfunc EXTUB

opdesc EXTU_W,	21,4,17,0,0,0
opfunc EXTU_W

opdesc EXTS_B,	19,4,15,0,0,0
opfunc EXTS_B

opdesc EXTS_W,	17,4,13,0,0,0
opfunc EXTS_W

//Store Register Opcodes
//----------------------

opdesc STC_SR_MEM,	23,0,6,0,0,0
opfunc STC_SR_MEM

opdesc STC_GBR_MEM,	30,0,13,0,0,0
opfunc STC_GBR_MEM

opdesc STC_VBR_MEM,	30,0,13,0,0,0
opfunc STC_VBR_MEM


opdesc MOVBL,	28,4,20,0,0,0
opfunc MOVBL


opdesc MOVWL,		26,4,20,0,0,0
opfunc MOVWL

opdesc MOVL_MEM_REG,		25,4,20,0,0,0
opfunc MOVL_MEM_REG

opdesc MOVBP,	31,4,23,0,0,0
opfunc MOVBP

opdesc MOVWP,	30,4,24,0,0,0
opfunc MOVWP

opdesc MOVLP,	29,4,24,0,0,0
opfunc MOVLP


opdesc MOVW_A,	38,0,4,0,17,0
opfunc MOVW_A

opdesc MOVL_A,	37,0,4,0,17,0
opfunc MOVL_A

opdesc MOVI,	13,0,4,0,9,0
opfunc MOVI

//----------------------

opdesc MOVBL0,	30,4,22,0,0,0
opfunc MOVBL0

opdesc MOVWL0,	28,4,22,0,0,0
opfunc MOVWL0

opdesc MOVLL0,	27,4,22,0,0,0
opfunc MOVLL0

opdesc MOVT,		15,0,4,0,0,0
opfunc MOVT

opdesc MOVBS0,	28,4,12,0,0,0
opfunc MOVBS0

opdesc MOVWS0,	28,4,12,0,0,0
opfunc MOVWS0

opdesc MOVLS0,	28,4,12,0,0,0
opfunc MOVLS0

//===========================================================================
//Verified Opcodes
//===========================================================================

opdesc DT,		20,0,4,0,0,0
opfunc DT

opdesc CMP_PZ,	17,0,4,0,0,0
opfunc CMP_PZ

opdesc CMP_PL,	23,0,4,0,0,0
opfunc CMP_PL

opdesc CMP_EQ,	24,4,12,0,0,0
opfunc CMP_EQ

opdesc CMP_HS,	24,4,12,0,0,0
opfunc CMP_HS

opdesc CMP_HI,	24,4,12,0,0,0
opfunc CMP_HI

opdesc CMP_GE,	24,4,12,0,0,0
opfunc CMP_GE

opdesc CMP_GT,	24,4,12,0,0,0
opfunc CMP_GT

opdesc ROTL,	23,0,4,0,0,0
opfunc ROTL

opdesc ROTR,	25,0,4,0,0,0
opfunc ROTR

opdesc ROTCL,	30,0,4,0,0,0
opfunc ROTCL

opdesc ROTCR,	36,0,4,0,0,0
opfunc ROTCR

opdesc SHL,		20,0,4,0,0,0
opfunc SHL

opdesc SHLR,	25,0,4,0,0,0
opfunc SHLR

opdesc SHAR,	22,0,4,0,0,0
opfunc SHAR

opdesc SHLL2,	9,0,4,0,0,0
opfunc SHLL2

opdesc SHLR2,	9,0,4,0,0,0
opfunc SHLR2

opdesc SHLL8,	9,0,4,0,0,0
opfunc SHLL8

opdesc SHLR8,	9,0,4,0,0,0
opfunc SHLR8

opdesc SHLL16,	9,0,4,0,0,0
opfunc SHLL16

opdesc SHLR16,	9,0,4,0,0,0
opfunc SHLR16

opdesc AND,		16,4,12,0,0,0
opfunc AND

opdesc OR,		16,4,12,0,0,0
opfunc OR

opdesc XOR,		16,4,12,0,0,0
opfunc XOR

opdesc ADDI,	9,0,4,0,8,0
opfunc ADDI

opdesc AND_B,	35,0,0,0,21,0
opfunc AND_B

opdesc OR_B,	36,0,0,0,22,0
opfunc OR_B

opdesc XOR_B,	36,0,0,0,22,0
opfunc XOR_B

opdesc TST_B,	34,0,0,0,25,0
opfunc TST_B

// Jump Opcodes
//------------

opdesc JMP,		11,0,4,0,0,0
opfunc JMP

opdesc JSR,		19,0,12,0,0,0
opfunc JSR

opdesc BRA,		29,0,0,0,0,5
opfunc BRA

opdesc BSR,		37,0,0,0,0,13
opfunc BSR

opdesc BSRF,		24,0,4,0,0,0
opfunc BSRF

opdesc BRAF,		16,0,4,0,0,0
opfunc BRAF

opdesc RTS,			6,0,0,0,0,0
opfunc RTS

opdesc RTE,			46,0,0,0,0,0
opfunc RTE

opdesc TRAPA,	      75,0,0,0,50,0
opfunc TRAPA

opdesc BT,		30,0,0,0,18,0
opfunc BT

opdesc BF,		30,0,0,0,18,0
opfunc BF

opdesc BF_S,		23,0,0,0,11,0
opfunc BF_S

//Store/Load Opcodes
//------------------

opdesc STC_SR,	10,0,4,0,0,0
opfunc STC_SR

opdesc STC_GBR,	18,0,4,0,0,0
opfunc STC_GBR

opdesc STC_VBR,	18,0,4,0,0,0
opfunc STC_VBR

opdesc STS_MACH, 10,0,4,0,0,0
opfunc STS_MACH

opdesc STS_MACH_DEC,	25,0,4,0,0,0
opfunc STS_MACH_DEC

opdesc STS_MACL, 11,0,4,0,0,0
opfunc STS_MACL

opdesc STS_MACL_DEC,	26,0,4,0,0,0
opfunc STS_MACL_DEC

opdesc LDC_SR,	21,0,4,0,0,0
opfunc LDC_SR


opdesc LDC_SR_INC,	28,0,4,0,0,0
opfunc LDC_SR_INC

opdesc LDCGBR,	16,0,4,0,0,0
opfunc LDCGBR

opdesc LDC_GBR_INC,	29,0,4,0,0,0
opfunc LDC_GBR_INC


opdesc LDC_VBR,	16,0,4,0,0,0
opfunc LDC_VBR

opdesc LDC_VBR_INC,	29,0,4,0,0,0
opfunc LDC_VBR_INC

opdesc STS_PR,		11,0,4,0,0,0
opfunc STS_PR

opdesc STSMPR,	26,0,4,0,0,0
opfunc STSMPR

opdesc LDS_PR,		11,0,4,0,0,0
opfunc LDS_PR

opdesc LDS_PR_INC,	24,0,4,0,0,0
opfunc LDS_PR_INC

opdesc LDS_MACH,		10,0,4,0,0,0
opfunc LDS_MACH

opdesc LDS_MACH_INC,	23,0,4,0,0,0
opfunc LDS_MACH_INC

opdesc LDS_MACL,		11,0,4,0,0,0
opfunc LDS_MACL

opdesc LDS_MACL_INC,	24,0,4,0,0,0
opfunc LDS_MACL_INC

//Mov Opcodes
//-----------

opdesc MOVA,	25,0,0,0,14,0
opfunc MOVA


opdesc MOVWI,	31,0,4,0,8,0
opfunc MOVWI


opdesc MOVLI,       40,0,4,0,8,0
opfunc MOVLI

opdesc MOVBL4, 29,4,0,8,0,0
opfunc MOVBL4


opdesc MOVWL4, 31,4,0,8,0,0
opfunc MOVWL4


opdesc MOVLL4, 33,4,28,8,0,0
opfunc MOVLL4
 
opdesc MOVBS4,	29,7,0,15,0,0
opfunc MOVBS4

opdesc MOVWS4,	32,7,0,15,0,0
opfunc MOVWS4

opdesc MOVLS4,	36,4,12,20,0,0
opfunc MOVLS4
 
opdesc MOVBLG,    24,0,0,0,5,0
opfunc MOVBLG

opdesc MOVWLG,    26,0,0,0,5,0
opfunc MOVWLG

opdesc MOVLLG,    25,0,0,0,5,0
opfunc MOVLLG

opdesc MOVBSG,    26,0,0,0,8,0
opfunc MOVBSG

opdesc MOVWSG,    30,0,0,0,8,0
opfunc MOVWSG

opdesc MOVLSG,    30,0,0,0,8,0
opfunc MOVLSG

opdesc MOVBS,	25,4,12,0,0,0
opfunc MOVBS

opdesc MOVWS,	25,4,12,0,0,0
opfunc MOVWS

opdesc MOVLS,	25,4,12,0,0,0
opfunc MOVLS

opdesc MOVR,		16,4,12,0,0,0
opfunc MOVR

opdesc MOVBM,		29,4,12,0,0,0
opfunc MOVBM

opdesc MOVWM,		29,4,12,0,0,0
opfunc MOVWM

opdesc MOVLM,		29,4,12,0,0,0
opfunc MOVLM

opdesc TAS,  46,0,4,0,0,0
opfunc TAS

opdesc SUBV, 24,4,12,0,0,0
opfunc SUBV

// string cmp
opdesc CMPSTR, 60,4,12,0,0,0
opfunc CMPSTR

//-------------------------------------------------------------
//div0s 
opdesc DIV0S, 78,36,4,0,0,0
opfunc DIV0S


//===============================================================
// DIV1   1bit Divid operation
// 
// size = 69 + 135 + 132 + 38 = 374 
//===============================================================
opdesc DIV1, 410,62,7,0,0,0
opfunc DIV1

//------------------------------------------------------------
//dmuls
opdesc DMULS, 27,6,14,0,0,0
opfunc DMULS

//------------------------------------------------------------
//dmulu 32bit -> 64bit Mul
opdesc DMULU, 27,6,14,0,0,0
opfunc DMULU

//--------------------------------------------------------------
// mull 32bit -> 32bit Multip
opdesc MULL, 25,6,14,0,0,0
opfunc MULL

//--------------------------------------------------------------
// muls 16bit -> 32 bit Multip
opdesc MULS, 38,6,17,0,0,0
opfunc MULS

//--------------------------------------------------------------
// mulu 16bit -> 32 bit Multip
opdesc MULU, 38,6,17,0,0,0
opfunc MULU

//--------------------------------------------------------------
// MACL   ans = 32bit -> 64 bit MUL
//        (MACH << 32 + MACL)  + ans 
//-------------------------------------------------------------
opdesc MAC_L, 145,5,30,0,0,0 
opfunc MAC_L


//--------------------------------------------------------------
// MACW   ans = 32bit -> 64 bit MUL
//        (MACH << 32 + MACL)  + ans 
//-------------------------------------------------------------
opdesc MAC_W, 120,5,31,0,0,0
opfunc MAC_W
