/*  Copyright 2017 devMiyax(smiyaxdev@gmail.com)

This file is part of Yabause.

Yabause is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Yabause is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Yabause; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <core.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h> 
#include "sh2core.h"
#include "DynarecSh2.h"
#include "opcodes.h"
#include "debug.h"
#include "yabause.h"
//#define DEBUG_CPU

CompileBlocks * CompileBlocks::instance_ = NULL;
DynarecSh2 * DynarecSh2::CurrentContext = NULL;

i_desc opcode_list[] =
{
  { ZERO_F,	"clrt",						0xffff, 0x8,	0, sh2_CLRT},
  { ZERO_F,   "clrmac",					0xffff, 0x28,	0, sh2_CLRMAC},
  { ZERO_F,   "div0u",					0xffff, 0x19,	0, sh2_DIV0U},
  { ZERO_F,   "nop",						0xffff, 0x9,	0, sh2_NOP},
  { ZERO_F,   "rte",						0xffff, 0x2b,	0, sh2_RTE},
  { ZERO_F,   "rts",						0xffff, 0xb,	0, sh2_RTS},
  { ZERO_F,   "sett",						0xffff, 0x18,	0, sh2_SETT},
  { ZERO_F,   "sleep",					0xffff, 0x1b,	0, sh2_SLEEP},
  { N_F,      "cmp/pl r%d",				0xf0ff, 0x4015,	0, sh2_CMP_PL},
  { N_F,      "cmp/pz r%d",				0xf0ff, 0x4011,	0, sh2_CMP_PZ},
  { N_F,      "dt r%d",					0xf0ff, 0x4010,	0, sh2_DT},
  { N_F,      "movt r%d",					0xf0ff, 0x0029,	0, sh2_MOVT},
  { N_F,      "rotl r%d",					0xf0ff, 0x4004,	0, sh2_ROTL},
  { N_F,      "rotr r%d",					0xf0ff, 0x4005,	0, sh2_ROTR},
  { N_F,      "rotcl r%d",				0xf0ff, 0x4024,	0, sh2_ROTCL},
  { N_F,      "rotcr r%d",				0xf0ff, 0x4025,	0, sh2_ROTCR},
  { N_F,      "shal r%d",					0xf0ff, 0x4020,	0, sh2_SHL},
  { N_F,      "shar r%d",					0xf0ff, 0x4021,	0, sh2_SHAR},
  { N_F,      "shll r%d",					0xf0ff, 0x4000,	0, sh2_SHL},
  { N_F,      "shlr r%d",					0xf0ff, 0x4001,	0, sh2_SHLR},
  { N_F,      "shll2 r%d",				0xf0ff, 0x4008,	0, sh2_SHLL2},
  { N_F,      "shlr2 r%d",				0xf0ff, 0x4009,	0, sh2_SHLR2},
  { N_F,      "shll8 r%d",				0xf0ff, 0x4018,	0, sh2_SHLL8},
  { N_F,      "shlr8 r%d",				0xf0ff, 0x4019,	0, sh2_SHLR8},
  { N_F,      "shll16 r%d",				0xf0ff, 0x4028,	0, sh2_SHLL16},
  { N_F,      "shlr16 r%d",				0xf0ff, 0x4029,	0, sh2_SHLL16},
  { N_F,      "stc sr, r%d",				0xf0ff, 0x0002,	0, sh2_STC_SR},
  { N_F,      "stc gbr, r%d",				0xf0ff, 0x0012,	0, sh2_STC_GBR},
  { N_F,      "stc vbr, r%d",				0xf0ff, 0x0022,	0, sh2_STC_VBR},
  { N_F,      "sts mach, r%d",			0xf0ff, 0x000a,	0, sh2_STS_MACH},
  { N_F,      "sts macl, r%d",			0xf0ff, 0x001a,	0, sh2_STS_MACL},
  { N_F,      "sts pr, r%d",				0xf0ff, 0x002a,	0, sh2_STS_PR},
  { N_F,      "tas.b @r%d",				0xf0ff, 0x401b,	0, sh2_TAS_B},
  { N_F,      "stc.l sr, @-r%d",			0xf0ff, 0x4003,	0, sh2_STC_SR_DEC},
  { N_F,      "stc.l gbr, @-r%d",			0xf0ff, 0x4013,	0, sh2_STC_GBR_DEC},
  { N_F,      "stc.l vbr, @-r%d",			0xf0ff, 0x4023,	0, sh2_STC_VBR_DEC},
  { N_F,      "sts.l mach, @-r%d",		0xf0ff, 0x4002,	0, sh2_STS_MACH_DEC},
  { N_F,      "sts.l macl, @-r%d",		0xf0ff, 0x4012,	0, sh2_STS_MACL_DEC},
  { N_F,      "sts.l pr, @-r%d",			0xf0ff, 0x4022,	0, sh2_STS_PR_DEC},
  { M_F,      "ldc r%d, sr",				0xf0ff, 0x400e,	0, sh2_LDC_SR},
  { M_F,      "ldc r%d, gbr",				0xf0ff, 0x401e,	0, sh2_LDC_GBR},
  { M_F,      "ldc r%d, vbr",				0xf0ff, 0x402e,	0, sh2_LDC_VBR},
  { M_F,      "lds r%d, mach",			0xf0ff, 0x400a,	0, sh2_LDS_MACH},
  { M_F,      "lds r%d, macl",			0xf0ff, 0x401a,	0, sh2_LDS_MACL},
  { M_F,      "lds r%d, pr",				0xf0ff, 0x402a,	0, sh2_LDS_PR},
  { M_F,      "jmp @r%d",					0xf0ff, 0x402b,	0, sh2_JMP},
  { M_F,      "jsr @r%d",					0xf0ff, 0x400b,	0, sh2_JSR}, 
  { M_F,      "ldc.l @r%d+, sr",			0xf0ff, 0x4007,	0, sh2_LDC_SR_INC},
  { M_F,      "ldc.l @r%d+, gbr",			0xf0ff, 0x4017,	0, sh2_LDC_GBR_INC},
  { M_F,      "ldc.l @r%d+, vbr",			0xf0ff, 0x4027,	0, sh2_LDC_VBR_INC},
  { M_F,      "lds.l @r%d+, mach",		0xf0ff, 0x4006,	0, sh2_LDS_MACH_INC},
  { M_F,      "lds.l @r%d+, macl",		0xf0ff, 0x4016,	0, sh2_LDS_MACL_INC},
  { M_F,      "lds.l @r%d+, pr",			0xf0ff, 0x4026,	0, sh2_LDS_PR_INC},
  { M_F,      "braf r%d",					0xf0ff, 0x0023,	0, sh2_BRAF},
  { M_F,      "bsrf r%d",					0xf0ff, 0x0003,	0, sh2_BSRF},
  { NM_F,     "add r%d, r%d",				0xf00f, 0x300c,	0, sh2_ADD},
  { NM_F,     "addc r%d, r%d",			0xf00f, 0x300e,	0, sh2_ADDC},
  { NM_F,     "addv r%d, r%d",			0xf00f, 0x300f,	0, sh2_ADDV},
  { NM_F,     "and r%d, r%d",				0xf00f, 0x2009,	0, sh2_AND},
  { NM_F,     "cmp/eq r%d, r%d",			0xf00f, 0x3000,	0, sh2_CMP_EQ},
  { NM_F,     "cmp/hs r%d, r%d",			0xf00f, 0x3002,	0, sh2_CMP_HS},
  { NM_F,     "cmp/ge r%d, r%d",			0xf00f, 0x3003,	0, sh2_CMP_GE},
  { NM_F,     "cmp/hi r%d, r%d",			0xf00f, 0x3006,	0, sh2_CMP_HI},
  { NM_F,     "cmp/gt r%d, r%d",			0xf00f, 0x3007,	0, sh2_CMP_GT},
  { NM_F,     "cmp/str r%d, r%d",			0xf00f, 0x200c,	0, sh2_CMP_STR},
  { NM_F,     "div1 r%d, r%d",			0xf00f, 0x3004,	0, sh2_DIV1},
  { NM_F,     "div0s r%d, r%d",			0xf00f, 0x2007,	0, sh2_DIV0S},
  { NM_F,     "dmuls.l r%d, r%d",			0xf00f, 0x300d,	0, sh2_DMULS_L},
  { NM_F,     "dmulu.l r%d, r%d",			0xf00f, 0x3005,	0, sh2_DMULU_L},
  { NM_F,     "exts.b r%d, r%d",			0xf00f, 0x600e,	0, sh2_EXTS_B},
  { NM_F,     "exts.w r%d, r%d",			0xf00f, 0x600f,	0, sh2_EXTS_W},
  { NM_F,     "extu.b r%d, r%d",			0xf00f, 0x600c,	0, sh2_EXTU_B},
  { NM_F,     "extu.w r%d, r%d",			0xf00f, 0x600d,	0, sh2_EXTU_W},
  { NM_F,     "mov r%d, r%d",				0xf00f, 0x6003,	0, sh2_MOVR},
  { NM_F,     "mul.l r%d, r%d",			0xf00f, 0x0007,	0, sh2_MUL_L},
  { NM_F,     "muls.w r%d, r%d",			0xf00f, 0x200f,	0, sh2_MULS},
  { NM_F,     "mulu.w r%d, r%d",			0xf00f, 0x200e,	0, sh2_MULU},
  { NM_F,     "neg r%d, r%d",				0xf00f, 0x600b,	0, sh2_NEG},
  { NM_F,     "negc r%d, r%d",			0xf00f, 0x600a,	0, sh2_NEGC},
  { NM_F,     "not r%d, r%d",				0xf00f, 0x6007,	0, sh2_NOT},
  { NM_F,     "or r%d, r%d",				0xf00f, 0x200b,	0, sh2_OR},
  { NM_F,     "sub r%d, r%d",				0xf00f, 0x3008,	0, sh2_SUB},
  { NM_F,     "subc r%d, r%d",			0xf00f, 0x300a,	0, sh2_SUBC},
  { NM_F,     "subv r%d, r%d",			0xf00f, 0x300b,	0, sh2_SUBV},
  { NM_F,     "swap.b r%d, r%d",			0xf00f, 0x6008,	0, sh2_SWAP_B},
  { NM_F,     "swap.w r%d, r%d",			0xf00f, 0x6009,	0, sh2_SWAP_W},
  { NM_F,     "tst r%d, r%d",				0xf00f, 0x2008,	0, sh2_TST},
  { NM_F,     "xor r%d, r%d",				0xf00f, 0x200a,	0, sh2_XOR},
  { NM_F,     "xtrct r%d, r%d",			0xf00f, 0x200d,	0, sh2_XTRCT},
  { NM_F,     "mov.b r%d, @r%d",			0xf00f, 0x2000,	0, sh2_MOVB},
  { NM_F,     "mov.w r%d, @r%d",			0xf00f, 0x2001,	0, sh2_MOVW},
  { NM_F,     "mov.l r%d, @r%d",			0xf00f, 0x2002,	0, sh2_MOVL},
  { NM_F,     "mov.b @r%d, r%d",			0xf00f, 0x6000,	0, sh2_MOVB_MEM},
  { NM_F,     "mov.w @r%d, r%d",			0xf00f, 0x6001,	0, sh2_MOVW_MEM},
  { NM_F,     "mov.l @r%d, r%d",			0xf00f, 0x6002,	0, sh2_MOVL_MEM},
  { NM_F,     "mac.l @r%d+, @r%d+",		0xf00f, 0x000f,	0, sh2_MAC_L},
  { NM_F,     "mac.w @r%d+, @r%d+",		0xf00f, 0x400f,	0, sh2_MAC_W},
  { NM_F,     "mov.b @r%d+, r%d",			0xf00f, 0x6004,	0, sh2_MOVB_INC},
  { NM_F,     "mov.w @r%d+, r%d",			0xf00f, 0x6005,	0, sh2_MOVW_INC},
  { NM_F,     "mov.l @r%d+, r%d",			0xf00f, 0x6006,	0, sh2_MOVL_INC},
  { NM_F,     "mov.b r%d, @-r%d",			0xf00f, 0x2004,	0, sh2_MOVB_DEC},
  { NM_F,     "mov.w r%d, @-r%d",			0xf00f, 0x2005,	0, sh2_MOVW_DEC},
  { NM_F,     "mov.l r%d, @-r%d",			0xf00f, 0x2006,	0, sh2_MOVL_DEC},
  { NM_F,     "mov.b r%d, @(r0, r%d)",	0xf00f, 0x0004,	0, sh2_MOVB_R0},
  { NM_F,     "mov.w r%d, @(r0, r%d)",	0xf00f, 0x0005,	0, sh2_MOVW_R0},
  { NM_F,     "mov.l r%d, @(r0, r%d)",	0xf00f, 0x0006,	0, sh2_MOVL_R0},
  { NM_F,     "mov.b @(r0, r%d), r%d",	0xf00f, 0x000c,	0, sh2_MOVB_R0_MEM},
  { NM_F,     "mov.w @(r0, r%d), r%d",	0xf00f, 0x000d,	0, sh2_MOVW_R0_MEM},
  { NM_F,     "mov.l @(r0, r%d), r%d",	0xf00f, 0x000e,	0, sh2_MOVL_R0_MEM},
  { MD_F,     "mov.b @(0x%03X, r%d), r0",	0xff00, 0x8400,	0, sh2_MOVB_DISP_R0},
  { MD_F,     "mov.w @(0x%03X, r%d), r0", 0xff00, 0x8500,	0, sh2_MOVW_DISP_R0},
  { ND4_F,    "mov.b r0, @(0x%03X, r%d)", 0xff00, 0x8000,	0, sh2_MOVB_R0_DISP},
  { ND4_F,    "mov.w r0, @(0x%03X, r%d)", 0xff00, 0x8100,	0, sh2_MOVW_R0_DISP},
  { NMD_F,    "mov.l r%d, @(0x%03X, r%d)",0xf000, 0x1000,	0, sh2_MOVL_DISP_MEM},
  { NMD_F,    "mov.l @(0x%03X, r%d), r%d",0xf000, 0x5000,	0, sh2_MOVL_MEM_DISP},
  { D_F,      "mov.b r0, @(0x%03X, gbr)",	0xff00, 0xc000,	1, sh2_MOVB_R0_GBR},
  { D_F,      "mov.w r0, @(0x%03X, gbr)",	0xff00, 0xc100,	2, sh2_MOVW_R0_GBR},
  { D_F,      "mov.l r0, @(0x%03X, gbr)",	0xff00, 0xc200,	4, sh2_MOVL_R0_GBR},
  { D_F,      "mov.b @(0x%03X, gbr), r0",	0xff00, 0xc400,	1, sh2_MOVB_GBR_R0},
  { D_F,      "mov.w @(0x%03X, gbr), r0",	0xff00, 0xc500,	2, sh2_MOVW_GBR_R0},
  { D_F,      "mov.l @(0x%03X, gbr), r0",	0xff00, 0xc600,	4, sh2_MOVL_GBR_R0},
  { D_F,      "mova @(0x%03X, pc), r0",	0xff00, 0xc700,	4, sh2_MOVA},
  { D_F,      "bf 0x%08X",				0xff00, 0x8b00,	4, sh2_BF},
  { D_F,      "bf/s 0x%08X",				0xff00, 0x8f00,	5, sh2_BF_S},
  { D_F,      "bt 0x%08X",				0xff00, 0x8900,	5, sh2_BT},
  { D_F,      "bt/s 0x%08X",				0xff00, 0x8d00,	5, sh2_BT_S},
  { D12_F,    "bra 0x%08X",				0xf000, 0xa000,	5, sh2_BRA},
  { D12_F,    "bsr 0x%08X",				0xf000, 0xb000,	0, sh2_BSR},
  { ND8_F,    "mov.w @(0x%03X, pc), r%d", 0xf000, 0x9000,	0, sh2_MOV_DISP_W},
  { ND8_F,    "mov.l @(0x%03X, pc), r%d",	0xf000, 0xd000,	2, sh2_MOV_DISP_L},
  { I_F,      "and.b #0x%02X, @(r0, gbr)",0xff00, 0xcd00,	4, sh2_AND_B},
  { I_F,      "or.b #0x%02X, @(r0, gbr)", 0xff00, 0xcf00,	0, sh2_OR_B},
  { I_F,      "tst.b #0x%02X, @(r0, gbr)",0xff00, 0xcc00,	0, sh2_TST_B},
  { I_F,      "xor.b #0x%02X, @(r0, gbr)",0xff00, 0xce00,	0, sh2_XOR_B},
  { I_F,      "and #0x%02X, r0",			0xff00, 0xc900,	0, sh2_ANDI},
  { I_F,      "cmp/eq #0x%02X, r0",		0xff00, 0x8800,	0, sh2_CMP_EQ_IMM},
  { I_F,      "or #0x%02X, r0",			0xff00, 0xcb00,	0, sh2_ORI},
  { I_F,      "tst #0x%02X, r0",			0xff00, 0xc800,	0, sh2_TST_R0},
  { I_F,      "xor #0x%02X, r0",			0xff00, 0xca00,	0, sh2_XORI},
  { I_F,      "trapa #0x%X",				0xff00, 0xc300,	0, sh2_TRAPA},
  { NI_F,     "add #0x%02X, r%d",			0xf000, 0x7000,	0, sh2_ADDI},
  { NI_F,     "mov #0x%02X, r%d",			0xf000, 0xe000,	0, sh2_MOVI},
  { 0,        NULL,						0,      0,		0, 0}
};

#define opdesc(op, y, c)	x86op_desc(x86_##op, &##op##_size, &##op##_src, &##op##_dest, &##op##_off1, &##op##_imm, &##op##_off3, y, c)
#define opNULL			x86op_desc(0,0,0,0,0,0,0,0,0)

#define PROLOGSIZE		     27    
#define EPILOGSIZE		      3
#define SEPERATORSIZE	     10
#define SEPERATORSIZE_NORMAL 7
#define SEPERATORSIZE_DEBUG  24
#define SEPERATORSIZE_DELAY  7
#define SEPERATORSIZE_DELAY_SLOT  27
#define SEPERATORSIZE_DELAY_AFTER  10 
#define SEPERATORSIZE_DELAYD 34
#define DELAYJUMPSIZE	     17
 
#define MININSTRSIZE    3
#define MAXINSTRSIZE	416
#define MAXJUMPSIZE		46

#define SAFEPAGESIZE	MAXBLOCKSIZE - MAXINSTRSIZE - SEPERATORSIZE_DELAY_SLOT - SEPERATORSIZE_DELAY_AFTER - SEPERATORSIZE_NORMAL - EPILOGSIZE
#define MAXINSTRCNT		MAXBLOCKSIZE/(MININSTRSIZE+SEPERATORSIZE)
int instrSize[NUMOFBLOCKS][MAXINSTRCNT];

#define opinit(x)	extern const unsigned short x##_size; \
                    extern const unsigned char x##_src, x##_dest, x##_off1, x##_imm, x##_off3; \
            void x86_##x();


extern "C" {

opinit(CLRT);
opinit(CLRMAC);
opinit(NOP);
opinit(DIV0U);
opinit(SETT);
opinit(SLEEP);

opinit(STS_MACH);
opinit(STS_MACL);
opinit(STS_MACH_DEC);
opinit(STS_MACL_DEC);
opinit(STS_PR);
opinit(STC_SR);
opinit(STC_VBR);
opinit(STC_GBR);
opinit(LDS_PR);
opinit(LDS_MACH);
opinit(LDS_MACL);
opinit(LDS_PR_INC);
opinit(LDS_MACH_INC);
opinit(LDS_MACL_INC);
opinit(LDC_SR);
opinit(LDC_VBR);
opinit(LDCGBR);
opinit(LDC_SR_INC);
opinit(LDC_VBR_INC);
opinit(LDC_GBR_INC);
opinit(STC_SR_MEM);
opinit(STC_VBR_MEM);
opinit(STC_GBR_MEM);
opinit(STSMPR);

opinit(CMP_EQ);
opinit(CMP_HI);
opinit(CMP_GE);
opinit(CMP_HS);
opinit(CMP_GT);

opinit(DT);
opinit(CMP_PL);
opinit(CMP_PZ);
opinit(ROTL);
opinit(ROTR);
opinit(ROTCL);
opinit(ROTCR);
opinit(SHL);
opinit(SHLR);
opinit(SHAR);
opinit(SHLL2);
opinit(SHLR2);
opinit(SHLL8);
opinit(SHLR8);
opinit(SHLL16);
opinit(SHLR16);

opinit(AND);
opinit(OR);
opinit(XOR);
opinit(NOT);
opinit(XTRCT);
opinit(ADD);
opinit(ADDC);
opinit(SUB);
opinit(SUBC);
opinit(NEG);
opinit(NEGC);
opinit(TST);
opinit(TSTI);

opinit(AND_B);
opinit(OR_B);
opinit(TST_B);
opinit(XOR_B);

opinit(ADDI);
opinit(ANDI);
opinit(ORI);
opinit(XORI);
opinit(MOVI);
opinit(CMP_EQ_IMM);

opinit(SWAP_B);
opinit(SWAP_W);
opinit(EXTUB);
opinit(EXTU_W);
opinit(EXTS_B);
opinit(EXTS_W);

opinit(BT)
opinit(BF);
opinit(BF_S);
opinit(JMP);
opinit(JSR);
opinit(BRA);
opinit(BSR);
opinit(BSRF);
opinit(BRAF);
opinit(RTE);
opinit(RTS);

opinit(MOVA);
opinit(MOVT);
opinit(MOVBL);
opinit(MOVWL);
opinit(MOVL_MEM_REG);
opinit(MOVBS);
opinit(MOVWS);
opinit(MOVLS);
opinit(MOVWI);
opinit(MOVLI);
opinit(MOVBS4);
opinit(MOVWS4);
opinit(MOVLS4);
opinit(MOVR);
opinit(MOVBP);
opinit(MOVWP);
opinit(MOVLP);
opinit(MOVBL0);
opinit(MOVWL0);
opinit(MOVLL0);
opinit(MOVBS0);
opinit(MOVWS0);
opinit(MOVLS0);
opinit(MOVBSG);
opinit(MOVLSG);
opinit(MOVWL4);  // 0x8500
opinit(MOVLL4);
opinit(MOVLM);
opinit(TAS);
opinit(ADDV);
opinit(CMPSTR);
opinit(DIV0S);
opinit(DMULS);
opinit(DMULU);
opinit(MULL);
opinit(MULS);
opinit(MULU);
opinit(SUBV);
opinit(MAC_L);
opinit(MOVBM);
opinit(MOVWM);
opinit(MOVBL4);
opinit(MOVWSG);
opinit(MOVBLG);
opinit(MOVWLG);
opinit(MOVLLG);
opinit(TRAPA);
opinit(DIV1);
opinit(MAC_W);


void prologue(void);
void epilogue(void);
void seperator(void);
void seperator_normal(void);
void seperator_delay(void);
void seperator_delay_slot(void);
void seperator_delay_after(void);

void seperator_d_normal(void);
void seperator_d_delay(void);

void PageJump(void); // jumps to a different page
void PageFlip(void); // "flips" the page

extern x86op_desc asm_list[];

}

x86op_desc asm_list[] =
{
  opdesc(CLRT,0,1),       
  opdesc(CLRMAC,0,1),     
  opdesc(DIV0U,0,1),      
  opdesc(NOP,0,1),
  opdesc(RTE,4,4),
  opdesc(RTS,4,2),
  opdesc(SETT,0,1),
  opdesc(SLEEP,0,3),
  opdesc(CMP_PL,0,1),
  opdesc(CMP_PZ,0,1),
  opdesc(DT,0,1),
  opdesc(MOVT,0,1),
  opdesc(ROTL,0,1),
  opdesc(ROTR,0,1),
  opdesc(ROTCL,0,1),
  opdesc(ROTCR,0,1),
  opdesc(SHL,0,1),
  opdesc(SHAR,0,1),
  opdesc(SHL,0,1),
  opdesc(SHLR,0,1),
  opdesc(SHLL2,0,1),
  opdesc(SHLR2,0,1),
  opdesc(SHLL8,0,1),
  opdesc(SHLR8,0,1),
  opdesc(SHLL16,0,1),
  opdesc(SHLR16,0,1),
  opdesc(STC_SR,0xFF,1),
  opdesc(STC_GBR,0xFF,1),
  opdesc(STC_VBR,0xFF,1),
  opdesc(STS_MACH,0xFF,1),
  opdesc(STS_MACL,0xFF,1),
  opdesc(STS_PR,0xFF,1),
  opdesc(TAS,0,4),         // 0x401b
  opdesc(STC_SR_MEM,0xFF,2),
  opdesc(STC_GBR_MEM,0xFF,2),
  opdesc(STC_VBR_MEM,0xFF,2),
  opdesc(STS_MACH_DEC,0xFF,2),
  opdesc(STS_MACL_DEC,0xFF,2),
  opdesc(STSMPR,0xFF,1),     // 0x4022
  opdesc(LDC_SR,0xFF,1),
  opdesc(LDCGBR,0xFF,1),
  opdesc(LDC_VBR,0xFF,1),
  opdesc(LDS_MACH,0xFF,1),
  opdesc(LDS_MACL,0xFF,1),
  opdesc(LDS_PR,0xFF,1),
  opdesc(JMP, 3,2),
  opdesc(JSR, 4,2),
  opdesc(LDC_SR_INC,0xFF,3),
  opdesc(LDC_GBR_INC,0xFF,3),
  opdesc(LDC_VBR_INC,0xFF,3),
  opdesc(LDS_MACH_INC,0xFF,1),
  opdesc(LDS_MACL_INC,0xFF,1),
  opdesc(LDS_PR_INC,0xFF,1),
  opdesc(BRAF,4,2),
  opdesc(BSRF,4,2),
  opdesc(ADD,0,1),
  opdesc(ADDC,0,1),
  opdesc(ADDV,0,1),  // 0x300F
  opdesc(AND,0,1),
  opdesc(CMP_EQ,0,1),
  opdesc(CMP_HS,0,1), 
  opdesc(CMP_GE,0,1),
  opdesc(CMP_HI,0,1),
  opdesc(CMP_GT,0,1), 
  opdesc(CMPSTR,0,1), // 0x200C
  opdesc(DIV1,0,1),   // 0x3004
  opdesc(DIV0S,0,1),  // 0x2007
  opdesc(DMULS,0,4),  // 0x300D
  opdesc(DMULU,0,4),  // 0x3005
  opdesc(EXTS_B,0,1),
  opdesc(EXTS_W,0,1),
  opdesc(EXTUB,0,1),
  opdesc(EXTU_W,0,1),
  opdesc(MOVR,0,1),  // 0x6003
  opdesc(MULL,0,4),  // 0x0007
  opdesc(MULS,0,3),  // 0x200f
  opdesc(MULU,0,3),  // 0x200e
  opdesc(NEG,0,1),
  opdesc(NEGC,0,1),
  opdesc(NOT,0,1),
  opdesc(OR,0,1),
  opdesc(SUB,0,1),
  opdesc(SUBC,0,1), 
  opdesc(SUBV,0,1),  // 0x300B
  opdesc(SWAP_B,0,1),
  opdesc(SWAP_W,0,1),
  opdesc(TST,0,1),
  opdesc(XOR,0,1),
  opdesc(XTRCT,0,1),
  opdesc(MOVBS,0,1), 
  opdesc(MOVWS,0,1),
  opdesc(MOVLS,0,1),
  opdesc(MOVBL,0,1), // 6000
  opdesc(MOVWL,0,1), // 6001
  opdesc(MOVL_MEM_REG,0,1),
  opdesc(MAC_L,0,3),  // 0x000f
  opdesc(MAC_W,0,3),  // 0x400f
  opdesc(MOVBP,0,1),  // 6004
  opdesc(MOVWP,0,1),  // 6005
  opdesc(MOVLP,0,1),  // 6006
  opdesc(MOVBM,0,1),  // 0x2004,
  opdesc(MOVWM,0,1),  // 0x2005,
  opdesc(MOVLM,0,1),  // 0x2006
  opdesc(MOVBS0,0,1), // 0x0004
  opdesc(MOVWS0,0,1), // 0x0005
  opdesc(MOVLS0,0,1), // 0x0006
  opdesc(MOVBL0,0,1), // 0x000C
  opdesc(MOVWL0,0,1), // 0x000D 
  opdesc(MOVLL0,0,1), // 0x000E
  opdesc(MOVBL4,0,1), // 0x8400
  opdesc(MOVWL4,0,1), // 0x8500
  opdesc(MOVBS4,0,1), // 0x8000
  opdesc(MOVWS4,0,1), // 0x8100
  opdesc(MOVLS4,0,1), // 0x1000
  opdesc(MOVLL4,0,1), // 0x5000 ,1
  opdesc(MOVBSG,0,1), // 0xC000
  opdesc(MOVWSG,0,1), // 0xc100
  opdesc(MOVLSG,0,1), // 0xC200
  opdesc(MOVBLG,0,1), // 0xC400
  opdesc(MOVWLG,0,1), // 0xC500
  opdesc(MOVLLG,0,1), // 0xC600
  opdesc(MOVA,0,1),
  opdesc(BF,1,3),
  opdesc(BF_S,3,2),
  opdesc(BT,1,3),
  opdesc(BT,3,2),
  opdesc(BRA,2,2),
  opdesc(BSR,2,2),
  opdesc(MOVWI,0,1),
  opdesc(MOVLI,0, 1),
  opdesc(AND_B,0,3),
  opdesc(OR_B,0,3),
  opdesc(TST_B,0, 1),
  opdesc(XOR_B,0,3),
  opdesc(ANDI,0,1),
  opdesc(CMP_EQ_IMM,0,1),
  opdesc(ORI,0,1),
  opdesc(TSTI,0,1),  // C800
  opdesc(XORI,0,1),
  opdesc(TRAPA,5,8), // 0xc300
  opdesc(ADDI,0,1),
  opdesc(MOVI,0,1),
  opNULL
}; 

Block *CompileBlocks::Init(Block *dynaCode)
{
  dynaCode = (Block*)ALLOCATE(sizeof(Block)*NUMOFBLOCKS);
  memset((void*)dynaCode, 0, sizeof(Block)*NUMOFBLOCKS);

  //
  memset(LookupTable, NULL, sizeof(LookupTable));
  memset(LookupParentTable, NULL, sizeof(LookupParentTable));
  memset(LookupTableRom, NULL, sizeof(LookupTableRom));
  memset(LookupTableLow, NULL, sizeof(LookupTableLow));
  memset(LookupTableC, NULL, sizeof(LookupTableC));

  blockCount = 0;
  LastMakeBlock = 0;

  g_CompleBlock = dynaCode;
  return dynaCode;
}

int CompileBlocks::opcodeIndex(u16 op)
{
  register int i = 0;
  while (((op & opcode_list[i].mask) != opcode_list[i].bits) && opcode_list[i].mnem != 0) i++;
  return i;
}

void CompileBlocks::FindOpCode(u16 opcode, u8 * instindex)
{
  *instindex = opcodeIndex(opcode);
  return;
}

void CompileBlocks::BuildInstructionList()
{
  u32 optest;
  memset(dsh2_instructions, 0, sizeof(u8)*MAX_INSTSIZE);
  for (optest = 0; optest <= 0xFFFF; optest++) {
    FindOpCode(optest, &dsh2_instructions[optest]);
  }
}

Block * CompileBlocks::CompileBlock(u32 pc, u32 * ParentT = NULL)
{
  compile_count_++;

  LastMakeBlock++;
  if (LastMakeBlock >= NUMOFBLOCKS) {
    LastMakeBlock = 0;
  }

  blockCount = LastMakeBlock;

  if (g_CompleBlock[blockCount].b_addr != NULL) {
    switch (g_CompleBlock[blockCount].b_addr & 0x0FF00000) {
    case 0x00000000:
      if (yabsys.emulatebios) {
        // ToDo: bios emulation
        exit(0);
      }
      else {
        LookupTableRom[g_CompleBlock[blockCount].b_addr & 0x000FFFFF] = NULL;
      }
      break;
    case 0x00200000:
      LookupTableLow[g_CompleBlock[blockCount].b_addr & 0x000FFFFF] = NULL;
      break;
    case 0x06000000:
      /*case 0x06100000:*/
      LookupTable[(g_CompleBlock[blockCount].b_addr & 0x000FFFFF) >> 1] = NULL;
      break;
    default:
      if ((g_CompleBlock[blockCount].b_addr & 0xFF000000) == 0xC0000000) {
        LookupTableC[g_CompleBlock[blockCount].b_addr & 0x000FFFFF] = NULL;
      }
      break;
    }
  }

  g_CompleBlock[blockCount].b_addr = pc;

  EmmitCode(&g_CompleBlock[blockCount], ParentT);

  return &g_CompleBlock[blockCount];
}

void CompileBlocks::ShowStatics() {
  LOG("Compile %d/%d", compile_count_, exec_count_);
  compile_count_ = 0;
  exec_count_ = 0;
}

void CompileBlocks::opcodePass(x86op_desc *op, u16 opcode, u8 *ptr)
{
  // multiply source and dest regions by 4 (size of register) 

  if (*(op->src) != 0)
    *(ptr + *(op->src)) = (u8)(((opcode >> 4) & 0xf) << 2);

  if (*(op->dest) != 0)
    *(ptr + *(op->dest)) = (u8)(((opcode >> 8) & 0xf) << 2);

  if (*(op->off1) != 0)
    *(ptr + *(op->off1)) = (u8)(opcode & 0xf);

  if (*(op->imm) != 0)
    *(ptr + *(op->imm)) = (u8)(opcode & 0xff);

  if (*(op->off3) != 0)
    *(u16*)(ptr + *(op->off3)) = (u16)(opcode & 0xfff);
}


void CompileBlocks::EmmitCode(Block *page, u32 * ParentT )
{
  int i, j, jmp = 0, count = 0;
  u16 op, temp;
  u32 addr = page->b_addr;
  u32 start_addr = page->b_addr;
  u8 *ptr, *startptr;

  startptr = ptr = page->code;
  i = 0;
  j = 0;
  count = 0;  
  memcpy((void*)ptr, (void*)prologue, PROLOGSIZE);
  ptr += PROLOGSIZE;
  int MaxSize = 0;

  void * nomal_seperator;
  u32 nomal_seperator_size;
  u8 nomal_seperator_counter_offset;
  void * delay_seperator;
  u32 delay_seperator_size;
  u8 delayslot_seperator_counter_offset;

  if (debug_mode_) {
    nomal_seperator = (void*)seperator_d_normal;
    nomal_seperator_size = SEPERATORSIZE_DEBUG;
    nomal_seperator_counter_offset = 3;
    delay_seperator = (void*)seperator_d_delay;
    delay_seperator_size = SEPERATORSIZE_DELAYD;
    delayslot_seperator_counter_offset = 6;
  }
  else {
    nomal_seperator = (void*)seperator_normal;
    nomal_seperator_size = SEPERATORSIZE_NORMAL;
    nomal_seperator_counter_offset = 6;
    delay_seperator = (void*)seperator_delay_slot;
    delay_seperator_size = SEPERATORSIZE_DELAY_SLOT;
    delayslot_seperator_counter_offset = 6;
  }
  
  MaxSize = MAXBLOCKSIZE - MAXINSTRSIZE- delay_seperator_size - SEPERATORSIZE_DELAY_AFTER - nomal_seperator_size - EPILOGSIZE;
  while (ptr - startptr < MaxSize) {
    // translate the opcode and insert code
    op = memGetWord(addr);
    i = dsh2_instructions[op];

    if( ParentT ){
      ParentT[(addr&0x000FFFFF)>>1] = (start_addr&0x000FFFFF)>>1;
    }
    addr += 2;

#ifdef BUILD_INFO
    DumpInstX( i, addr-2, op  );
#endif
   
    if (asm_list[i].func == 0) {
      LOG("Unimplemented Opcode (0x%4x) at 0x%8x\n", op, addr-2);
      // TODO: Stop Slave
      //if( g_CurrentContext->m_pSaturnSh2->m_bSlave ){
      //  g_Saturn->m_bSlaveRunning = false;
      //}
      memcpy((void*)ptr, (void*)nomal_seperator, nomal_seperator_size);
      instrSize[blockCount][count++] = nomal_seperator_size;
      ptr += nomal_seperator_size;
      break;
    }

    // Regular Opcode ( No Delay Branch )
    if (asm_list[i].delay == 0) { 
      memcpy((void*)ptr, (void*)(asm_list[i].func), *(asm_list[i].size));
      memcpy((void*)(ptr + *(asm_list[i].size)), (void*)nomal_seperator, nomal_seperator_size);
      instrSize[blockCount][count++] = *(asm_list[i].size) + nomal_seperator_size;
      opcodePass(&asm_list[i], op, ptr);
      u8 * counterpos = ptr + *(asm_list[i].size) + nomal_seperator_counter_offset;
      *counterpos = asm_list[i].cycle;
      ptr += *(asm_list[i].size) + nomal_seperator_size;
    }

    // No Intrupt Func ToDo: Never end block these functions
    else if (asm_list[i].delay == 0xFF ) { 
      memcpy((void*)ptr, (void*)(asm_list[i].func), *(asm_list[i].size));
      memcpy((void*)(ptr + *(asm_list[i].size)), (void*)nomal_seperator, nomal_seperator_size);
      instrSize[blockCount][count++] = *(asm_list[i].size) + nomal_seperator_size;
      opcodePass(&asm_list[i], op, ptr);
      ptr += *(asm_list[i].size) + nomal_seperator_size;
    }

    // Normal Jump
    else if (asm_list[i].delay == 1 || asm_list[i].delay == 5 ) { 
      memcpy((void*)ptr, (void*)(asm_list[i].func), *(asm_list[i].size));
      memcpy((void*)(ptr + *(asm_list[i].size)), (void*)nomal_seperator, nomal_seperator_size);
      memcpy((void*)(ptr + *(asm_list[i].size)+ nomal_seperator_size), (void*)PageFlip, DELAYJUMPSIZE);
      instrSize[blockCount][count++] = *(asm_list[i].size) + nomal_seperator_size + DELAYJUMPSIZE;
      opcodePass(&asm_list[i], op, ptr);
      u8 * counterpos = ptr + *(asm_list[i].size) + nomal_seperator_counter_offset;
      *counterpos = asm_list[i].cycle;
      ptr += *(asm_list[i].size) + nomal_seperator_size + DELAYJUMPSIZE;
    }

    // Jmp With Delay Operation
    else { 

      u8 cycle = asm_list[i].cycle;
      memcpy((void*)ptr, (void*)(asm_list[i].func), *(asm_list[i].size));
      memcpy((void*)(ptr + *(asm_list[i].size)), (void*)delay_seperator, delay_seperator_size);
      instrSize[blockCount][count++] = *(asm_list[i].size) + delay_seperator_size;
      opcodePass(&asm_list[i], op, ptr);
      ptr += *(asm_list[i].size) + delay_seperator_size;

      // Get NExt instruction
      temp = memGetWord(addr);
      addr += 2;
      j = opcodeIndex(temp);
#ifdef BUILD_INFO
      DumpInstX( j, addr-2, temp  );
#endif
      if (asm_list[j].func == 0) {
        LOG("Unimplemented Opcode (0x%4x) at 0x%8x\n", temp, addr-2);
        break;
      }

      cycle += asm_list[j].cycle;
      memcpy((void*)ptr, (void*)(asm_list[j].func), *(asm_list[j].size));
      memcpy((void*)(ptr + *(asm_list[j].size)), (void*)seperator_delay_after, SEPERATORSIZE_DELAY_AFTER);
      instrSize[blockCount][count++] = *(asm_list[j].size) + SEPERATORSIZE_DELAY_AFTER;
      opcodePass(&asm_list[j], temp, ptr);
      u8 * counterpos = ptr + *(asm_list[j].size) + delayslot_seperator_counter_offset;
      *counterpos = cycle;
      ptr += *(asm_list[j].size) + SEPERATORSIZE_DELAY_AFTER;
    }
    if( asm_list[i].delay != 0xFF && asm_list[i].delay != 0x00 ) break;
  }
  page->e_addr = addr-2;
  memcpy((void*)ptr, (void*)epilogue, EPILOGSIZE);

  page->isInfinityLoop = false;

  return;
}

DynarecSh2::DynarecSh2() {
  m_pDynaSh2     = new tagSH2;
  m_pCompiler = CompileBlocks::getInstance();
  m_ClockCounter = 0;
  m_IntruptTbl.clear();
  m_bIntruptSort = true;
  pre_cnt_ = 0;
  pre_exe_count_ = 0;
  interruput_chk_cnt_ = 0;
  interruput_cnt_ = 0;
}

void DynarecSh2::ResetCPU(){
  memset((void*)m_pDynaSh2->GenReg, 0, sizeof(u32) * 16);
  memset((void*)m_pDynaSh2->CtrlReg, 0, sizeof(u32) * 3);
  memset((void*)m_pDynaSh2->SysReg, 0, sizeof(u32) * 6);

  m_pDynaSh2->CtrlReg[0] = 0x000000;  // SR
  m_pDynaSh2->CtrlReg[2] = 0x000000; // VBR
  m_pDynaSh2->SysReg[3] = memGetLong(m_pDynaSh2->CtrlReg[2]);
  m_pDynaSh2->GenReg[15] = memGetLong(m_pDynaSh2->CtrlReg[2] + 4);
  m_pDynaSh2->SysReg[4] = 0;
  m_pDynaSh2->SysReg[5] = 0;
  pre_cnt_ = 0;
  pre_exe_count_ = 0;
  interruput_chk_cnt_ = 0;
  interruput_cnt_ = 0;
  m_IntruptTbl.clear();
}

void DynarecSh2::ExecuteCount( u32 Count ) {
  
  u32 targetcnt = 0;
  
  if (Count > pre_exe_count_) {
    targetcnt = m_pDynaSh2->SysReg[4] + Count - pre_exe_count_;
  }
  else {
    pre_exe_count_ = pre_exe_count_-Count;
    return;
  }

  // Overflow
  if (targetcnt < m_pDynaSh2->SysReg[4]){
    targetcnt = Count + (0xFFFFFFFF - m_pDynaSh2->SysReg[4]) + 1;
    m_pDynaSh2->SysReg[4] = 0;
  }


  if ((GET_SR() & 0xF0) < GET_ICOUNT()) {
    this->CheckInterupt();
  }

  while (GET_COUNT() < targetcnt) {
    if (Execute() == IN_INFINITY_LOOP ) {
      SET_COUNT(targetcnt);
    }
    CurrentSH2->cycles = GET_COUNT();
  }

  if (Count == 1) {
    one_step_ = true;
    pre_exe_count_ = 0;
  }
  else {
    one_step_ = false;
    pre_exe_count_ = m_pDynaSh2->SysReg[4] - targetcnt;
    if (pre_exe_count_ < 0) {
      pre_exe_count_ = 0;
    }
  }
}

int DynarecSh2::CheckOneStep() {
  if (one_step_ == 1) {
    m_pDynaSh2->SysReg[3] += 2;
    return 1;
  }
  return 0;
}

int DynarecSh2::Execute(){

  Block * pBlock = NULL;

  m_pCompiler->exec_count_++;
    
  switch( GET_PC() & 0x0FF00000 )
  {
    
  // ROM
  case 0x00000000:
    pBlock = m_pCompiler->LookupTableRom[ GET_PC() & 0x000FFFFF ];
    if( pBlock == NULL )
    {
      pBlock = m_pCompiler->CompileBlock(GET_PC());
      m_pCompiler->LookupTableRom[ GET_PC() & 0x000FFFFF ] = pBlock;
    }
    break;

  // Low Memory
  case 0x00200000:
    pBlock = m_pCompiler->LookupTableLow[ GET_PC() & 0x000FFFFF ];
    if( pBlock == NULL )
    {
      pBlock = m_pCompiler->CompileBlock(GET_PC());
      m_pCompiler->LookupTableLow[ GET_PC() & 0x000FFFFF ] = pBlock;
    }
    break;

  // High Memory
  case 0x06000000:
  /*case 0x06100000:*/

    pBlock = m_pCompiler->LookupTable[ (GET_PC() & 0x000FFFFF)>>1 ];
    if( pBlock == NULL )
    {
      pBlock = m_pCompiler->CompileBlock(GET_PC(), (u32*)m_pCompiler->LookupParentTable);
      m_pCompiler->LookupTable[ (GET_PC() & 0x000FFFFF)>>1 ] = pBlock;
    } 
    break;

  // Cache
  default:
    if( (GET_PC() & 0xFF000000) == 0xC0000000 )
    {
      pBlock = m_pCompiler->LookupTableC[ GET_PC() & 0x000FFFFF ];
      if( pBlock == NULL )
      {
        pBlock = m_pCompiler->CompileBlock(GET_PC());
        m_pCompiler->LookupTableC[ GET_PC() & 0x000FFFFF ] = pBlock;
      } 
    }else{
      pBlock = m_pCompiler->CompileBlock(GET_PC());
    }
    break;  
   }
    
  //LOG("\n---dynaExecute %08X----\n", GET_PC() );
  ((dynaFunc)((void*)(pBlock->code)))(m_pDynaSh2); 

  return 0;
}

bool operator < (const dIntcTbl & data1 , const dIntcTbl & data2 )
{
  return data1.level > data2.level;
} 
bool operator == (const dIntcTbl & data1 , const dIntcTbl & data2 )
{
  return ( data1.Vector == data2.Vector );
} 

void DynarecSh2::AddInterrupt( u8 Vector, u8 level )
{
  dIntcTbl tmp;
  tmp.Vector = Vector;
  tmp.level  = level;

  m_bIntruptSort = false;
  m_IntruptTbl.push_back(tmp);
  m_IntruptTbl.unique(); 

  if( m_IntruptTbl.size() > 1 ) {
    m_IntruptTbl.sort();
  }
  m_bIntruptSort = true;
  m_pDynaSh2->SysReg[5] = m_IntruptTbl.begin()->level<<4;
}


int DynarecSh2::CheckInterupt(){

  interruput_chk_cnt_++;

  if( m_IntruptTbl.size() == 0 ) {
    return 0;
  }
  
  dlstIntct::iterator pos = m_IntruptTbl.begin();
  if( InterruptRutine((*pos).Vector, (*pos).level ) != 0 ) {
    m_IntruptTbl.pop_front();
    if( m_IntruptTbl.size() != 0 ) {
      m_pDynaSh2->SysReg[5] = m_IntruptTbl.begin()->level<<4;
    }else{
      m_pDynaSh2->SysReg[5] = 0x0000;
    }
    return 1;
  }
  return 0;
}

int DynarecSh2::InterruptRutine(u8 Vector, u8 level)
{
  if (((u32)level) > ((m_pDynaSh2->CtrlReg[0] >> 4) & 0x0F)) {

    u32 prepc = m_pDynaSh2->SysReg[3];

    interruput_cnt_++;
    m_pDynaSh2->GenReg[15] -= 4;
    memSetLong(m_pDynaSh2->GenReg[15], m_pDynaSh2->CtrlReg[0]);
    m_pDynaSh2->GenReg[15] -= 4;
    memSetLong(m_pDynaSh2->GenReg[15], m_pDynaSh2->SysReg[3]);
    m_pDynaSh2->CtrlReg[0] |= ((u32)(level << 4) & 0x000000F0);
    m_pDynaSh2->SysReg[3] = memGetLong(m_pDynaSh2->CtrlReg[2] + (((u32)Vector) << 2));
#if defined(DEBUG_CPU)
    LOG("**** [%s] Exception vecnum=%u, PC=%08X to %08X, level=%08X", (is_slave_==false)?"M":"S", Vector, prepc, m_pDynaSh2->SysReg[3], level);
#endif
    return 1;
  }
  return 0;
}

void DynarecSh2::ShowStatics(){
#if defined(DEBUG_CPU)
  LOG("Exec cnt %d interruput_chk_cnt_ = %d, interruput_cnt_ = %d", GET_COUNT() - pre_cnt_, interruput_chk_cnt_, interruput_cnt_ );
  pre_cnt_ = GET_COUNT();
  interruput_chk_cnt_ = 0;
  interruput_cnt_ = 0;
#endif
}