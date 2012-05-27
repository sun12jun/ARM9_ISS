#include <stdio.h>
#include <stdlib.h>

#include <arm.h>
#include <common.h>

#include "thumb1.h"

inline int32u_t carryfrom(int32u_t a, int32u_t b)
{
	if( (BIT(a, 31) & BIT(b, 31)) ||  ( (BIT(a, 31)||BIT(b, 31)) & !BIT(a + b, 31) ) )
		return 1;
	else 
		return 0;
}

inline int32u_t overflowfrom(int32u_t a, int32u_t b, int32u_t nadd_sub)
{
	if(nadd_sub==OVF_ADD)
	{
		if(BIT(a, 31) == BIT(b, 31))
			if(BIT( (a + b), 31) != BIT(a, 31))
				return 1;
	}
	else
	{
		if(BIT(a, 31) != BIT(b, 31))
			if(BIT( (a-b), 31 ) != BIT(a, 31))
				return 1;
	}

	return 0;
}

inline int32u_t notborrowfrom(int32u_t a, int32u_t b)
{
	if ( ((BIT(a, 31)&(!BIT(b, 31)))) || ((BIT(a, 31)||(!BIT(b, 31)))&(!BIT(a-b, 31)))  )
		return 1;
	else
		return 0;
}


void thumb1_fetch()
{
	debug("I'm in thumb1 fetch()\n");

	int32u_t inst;

	if((ARM_PC % 2))
		printf("alignment fault during fetch\n");

	inst = inst_buffer[ARM_PC >> 2];

	if( ARM_PC % 4 == 2 ) // fetch low 16 bit
		ins->opcode_thumb = ( USUBBITS(inst, 31, 16) ); 
	else					// fetch high 16 bit
		ins->opcode_thumb = ( USUBBITS(inst, 15, 0) ); 
}

void thumb1_decode()
{
	debug("I'm in thumb1 decode()\n");
	int16u_t inst;

	inst = ins->opcode_thumb;

	switch(USUBBITS(inst, 15, 13))
	{
		case 0:
			switch(USUBBITS(inst, 12, 11))// LSL
			{
				case 0x0: // LSL(1)
					ins->de1 = 0;
					ins->de2 = 0;
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm = USUBBITS(inst, 10, 6);

					ins->exec = lsl1;
					break;

				case 0x1: // LSR(1)
					ins->de1 = 0;
					ins->de2 = 1;
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm = USUBBITS(inst, 10, 6);

					ins->exec = lsr1;
					break;

				case 0x2: // ASR(1)
					ins->de1 = 0;
					ins->de2 = 2;
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm = USUBBITS(inst, 10, 6);

					ins->exec = asr1;
					break;

				default:
					break;
			}

			switch(USUBBITS(inst, 12, 9))
			{
				case 0xc: // ADD(3)
					ins->de1 = 0;
					ins->de2 = 0xc;
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = add3;

					break;
				case 0xd: // sub(3)
					ins->de1 = 0;
					ins->de2 = 0xd;
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = sub3;
					break;

				case 0xe:
					if(USUBBITS(inst, 8, 6)==0) // mov2
					{
						ins->de1 = 0;
						ins->de2 = 0xe;
						ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
						ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
						ins->exec = mov2;
					}
					else // add1
					{
						ins->de1 = 0;
						ins->de2 = 0xe;
						ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
						ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
						ins->imm = USUBBITS(inst, 8, 6);
						ins->exec = add1;
					}

					break;

				case 0xf: // sub1
					ins->de1 = 0;
					ins->de2 = 0xf;
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm = USUBBITS(inst, 8, 6);
					ins->exec = sub1;
					break;
			}
			break;

		case 1:
			switch(USUBBITS(inst, 12, 11))
			{
				case 0x0: // mov1
					ins->de1 = 1;
					ins->de2 = 0x0;
					ins->rd = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = mov1;
					break;

				case 0x1: // cmp1
					ins->de1 = 1;
					ins->de2 = 0x1;
					ins->rn = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = cmp1;
					break;

				case 0x2: // add2
					ins->de1 = 1;
					ins->de2 = 0x2;
					ins->rd = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = add2;
					break;

				case 0x3: // sub2
					ins->de1 = 1;
					ins->de2 = 0x3;
					ins->rd = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = sub2;
					break;
			}

			break;

		case 2:
			switch(USUBBITS(inst, 12, 6))
			{
				case 0x0: // and
					ins->de1 = 2;
					ins->de2 = 0x0;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = and;
					
					break;

				case 0x1: // eor
					ins->de1 = 2;
					ins->de2 = 0x1;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = eor;
				
					break;

				case 0x2: // lsl(2)
					ins->de1 = 2;
					ins->de2 = 0x2;
					ins->rs = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = lsl2;
				
					break;

				case 0x3: // lsr(2)
					ins->de1 = 2;
					ins->de2 = 0x3;
					ins->rs = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = lsr2;
				
					break;

				case 0x4: // asr(2)
					ins->de1 = 2;
					ins->de2 = 0x4;
					ins->rs = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = asr2;
				
					break;
				case 0x5: // adc
					ins->de1 = 2;
					ins->de2 = 0x5;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = adc;
				
					break;

				case 0x6: // sbc
					ins->de1 = 2;
					ins->de2 = 0x6;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = sbc;
				
					break;
				case 0x7: // ror
					ins->de1 = 2;
					ins->de2 = 0x7;
					ins->rs = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = ror;
				
					break;
				case 0x8: // tst
					ins->de1 = 2;
					ins->de2 = 0x8;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rn = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = tst;
				
					break;

				case 0x9: // neg
					ins->de1 = 2;
					ins->de2 = 0x9;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = neg;
				
					break;
				case 0xa: // cmp(2)
					ins->de1 = 2;
					ins->de2 = 0xa;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rn = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = cmp2;
				
					break;
				case 0xb: // cmn
					ins->de1 = 2;
					ins->de2 = 0xb;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rn = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = cmn;
				
					break;

				case 0xc: // orr
					ins->de1 = 2;
					ins->de2 = 0xc;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = orr;
				
					break;

				case 0xd: // mul
					ins->de1 = 2;
					ins->de2 = 0xd;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = mul;
				
					break;

				case 0xe: // bic
					ins->de1 = 2;
					ins->de2 = 0xe;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = bic;
					break;
				case 0xf: // mvn
					ins->de1 = 2;
					ins->de2 = 0xf;
					ins->rm = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);

					ins->exec = mvn;

					break;
			}

			switch(USUBBITS(inst, 12, 8))
			{
				case 0x4: // add4
					ins->de1 = 2;
					ins->de2 = 0x4;

					if(!(BIT(inst, 7) || BIT(inst, 6)))
						printf("the add4 result is unpredictable\n");

					ins->rd = &(env->regs[(BIT(inst, 7) << 3) + USUBBITS(inst, 2, 0)]);
					ins->rm = &(env->regs[(BIT(inst, 6) << 3) + USUBBITS(inst, 5, 3)]);

					ins->exec = add4;

					break;

				case 0x5: // cmp3
					ins->de1 = 2;
					ins->de2 = 0x5;

					if(!(BIT(inst, 7) || BIT(inst, 6)) || (BIT(inst, 7) << 3 + USUBBITS(inst, 2, 0))==0xf )
						printf("the add4 result is unpredictable\n");

					ins->rm = &(env->regs[(BIT(inst, 6) << 3) + USUBBITS(inst, 5, 3)]);
					ins->rn = &(env->regs[(BIT(inst, 7) << 3) + USUBBITS(inst, 2, 0)]);

					ins->exec = cmp3;

					break;

				case 0x6: // cpy, mov3
					ins->de1 = 2;
					ins->de2 = 0x6;

					ins->rm = &(env->regs[(BIT(inst, 6) << 3) + USUBBITS(inst, 5, 3)]);
					ins->rd = &(env->regs[(BIT(inst, 7) << 3) + USUBBITS(inst, 2, 0)]);

					ins->exec = mov3;

					break;
			}

			switch(USUBBITS(inst, 12, 11))
			{
				case 1: // ldr3
					ins->de1 = 2;
					ins->de2 = 0x1;

					ins->rd = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = ldr3;

					break;
			}

			switch(USUBBITS(inst, 12, 9))
			{
				case 0x8: // str(2)
					ins->de1 = 2;
					ins->de2 = 0x8;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = str2;

					break;

				case 0x9: // strh(2)
					ins->de1 = 2;
					ins->de2 = 0x9;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = strh2;

					break;

				case 0xa: //strb(2)
					ins->de1 = 2;
					ins->de2 = 0xa;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = strb2;

					break;

				case 0xb: // ldrsb
					ins->de1 = 2;
					ins->de2 = 0xb;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = ldrsb;

					break;

				case 0xc: // ldr(2)
					ins->de1 = 2;
					ins->de2 = 0xc;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = ldr2;

					break; 

				case 0xd: // ldrh(2)
					ins->de1 = 2;
					ins->de2 = 0xd;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = ldrh2;

					break;

				case 0xe: // ldrb(2)
					ins->de1 = 2;
					ins->de2 = 0xe;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = ldrb2;

					break;

				case 0xf: // ldrsh
					ins->de1 = 2;
					ins->de2 = 0xf;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->rm = &(env->regs[USUBBITS(inst, 8, 6)]);

					ins->exec = ldrsh;

					break;

			}

			break;

		case 3:
			switch(USUBBITS(inst, 12, 11))
			{
				case 0x0: // str(1)
					ins->de1 = 3;
					ins->de2 = 0x0;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm = USUBBITS(inst, 10, 6);

					ins->exec = str1;

					break;

				case 0x1: // ldr(1)
					ins->de1 = 3;
					ins->de2 = 0x1;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm = USUBBITS(inst, 10, 6);

					ins->exec = ldr1;

					break;

				case 0x2: // strb(1)
					ins->de1 = 3;
					ins->de2 = 0x2;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm = USUBBITS(inst, 10, 6);

					ins->exec = strb1;

					break;

				case 0x3: // ldrb(1)
					ins->de1 = 3;
					ins->de2 = 0x3;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm =USUBBITS(inst, 10, 6);

					ins->exec = ldrb1;

					break;
			}

			break;

		case 4:
			switch(USUBBITS(inst, 12, 11))
			{
				case 0x0: // strh(1)
					ins->de1 = 4;
					ins->de2 = 0x0;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm = USUBBITS(inst, 10, 6);

					ins->exec = strh1;

					break;

				case 0x1: // ldrh(1)
					ins->de1 = 4;
					ins->de2 = 0x1;

					ins->rd = &(env->regs[USUBBITS(inst, 2, 0)]);
					ins->rn = &(env->regs[USUBBITS(inst, 5, 3)]);
					ins->imm = USUBBITS(inst, 10, 6);

					ins->exec = ldrh1;
					break;

				case 0x2: // str(3)
					ins->de1 = 4;
					ins->de2 = 0x2;

					ins->rd = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = str3;
				
					break;

				case 0x3: // ldr(4)
					ins->de1 = 4;
					ins->de2 = 0x3;

					ins->rd = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = ldr4;
				
					break;
			}

			break;

		case 5:
			switch(USUBBITS(inst, 12, 11))
			{
				case 0x0: // add(5)
					ins->de1 = 5;
					ins->de2 = 0x0;

					ins->rd = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = add5;
					break;

				case 0x1: // add(6)
					ins->de1 = 5;
					ins->de2 = 0x1;

					ins->rd = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = add6;
					break;
			}

			switch(USUBBITS(inst, 12, 7))
			{
				case 0x20: // add(7)
					ins->de1 = 5;
					ins->de2 = 0x20;

					ins->imm = USUBBITS(inst, 6, 0);

					ins->exec = add7;
					break;

				case 0x21: // sub(4)
					ins->de1 = 5;
					ins->de2 = 0x21;

					ins->imm = USUBBITS(inst, 6, 0);

					ins->exec = sub4;
					break;

			}

			switch(USUBBITS(inst, 12, 9))
			{
				case 0xa: // push
					ins->de1 = 5;
					ins->de2 = 0xa;
					// register list
					if(USUBBITS(inst, 8, 0)==0)
						printf("this push result is unpredictable\n");

					ins->imm = USUBBITS(inst, 8, 0);

					ins->exec = push;
					break;

				case 0xe: // pop
					ins->de1 = 5;
					ins->de2 = 0xe;
					// register list
					if(USUBBITS(inst, 8, 0)==0)
						printf("this pop result is unpredictable\n");

					ins->imm = USUBBITS(inst, 8, 0);

					ins->exec = pop;
				
					break;

				case 0xf: // bkpt
					ins->de1 = 5;
					ins->de2 = 0xf;

					ins->imm = USUBBITS(inst, 7, 0);
					ins->exec = bkpt;
				
					break;
			}

			break;

		case 6:
			switch( USUBBITS(inst, 12, 11) )
			{
				case 0x0: // STMIA
					ins->de1 = 6;
					ins->de2 = 0x0;
					// register list
					if(USUBBITS(inst, 7, 0)==0)
						printf("this stmia result is unpredictable\n");

					ins->rn = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = stmia;
				
					break;

				case 0x1: // LDMIA
					ins->de1 = 6;
					ins->de2 = 0x1;
					// register list
					if(USUBBITS(inst, 7, 0)==0)
						printf("this ldmia result is unpredictable\n");

					ins->rn = &(env->regs[USUBBITS(inst, 10, 8)]);
					ins->imm = USUBBITS(inst, 7, 0);

					ins->exec = ldmia;
					break;
			}

			switch(BIT(inst, 12))
			{
				case 0x1: // b(1)
					ins->de1 = 6;
					ins->de2 = 0x1;

					ins->imm = SUBBITS(inst, 7, 0);
					ins->cond= USUBBITS(inst, 11, 8);

					ins->exec = b1;

					break;
			}

			break;

		case 7:
			switch(USUBBITS(inst, 12, 11))
			{
				case 0x0: // b(2)
					ins->de1 = 7;
					ins->de2 = 0x0;

					ins->imm = SUBBITS(inst, 10, 0);

					ins->exec = b2;
					break;

				case 0x1:
					printf("BLX.. not supported in this version\n");
					break;

				case 0x2: // bl_h2
					ins->de1 = 7;
					ins->de2 = 0x2;

					ins->imm = USUBBITS(inst, 10, 0);

					ins->exec = bl_h2;

					break;

				case 0x3: // bl_h3
					ins->de1 = 7;
					ins->de2 = 0x2;

					ins->imm = USUBBITS(inst, 10, 0);
					ins->exec = bl_h3;

					break;
			}
			break;
	}
}

 void lsl1()
{
	if(ins->imm==0)
		ARM_RD = ARM_RM;
	else
	{
		CPSR_SET_C(GET(ARM_RM, 32 - ins->imm));
		ARM_RD = ARM_RM << (ins->imm);
	}

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void lsr1()
{
	if(ins->imm==0)
	{
		CPSR_SET_C(GET(ARM_RM, 31));
		ARM_RD = 0;
	}
	else
	{
		CPSR_SET_C(GET(ARM_RM, ins->imm - 1));
		ARM_RD = ARM_RM >> (ins->imm);
	}

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void asr1()
{
	int i;

	if(ins->imm==0)
	{
		CPSR_SET_C(GET(ARM_RM, 31));

		if(GET(ARM_RM, 31) == 0)
			ARM_RD = 0;
		else
			ARM_RD = 0xFFFFFFF;
	}
	else
	{
		CPSR_SET_C(GET(ARM_RM, ins->imm - 1));
		if(GET(ARM_RM, 31)) // negative
		{
			ARM_RD = ARM_RM;
			for(i = 0; i < ins->imm; i++)
			{
				ARM_RD = ARM_RD >> 1;
				ARM_RD |= ARM_RM & 0x80000000;
				//ARM_RD = USUBBITS(ARM_RM, 31, 0) >> (ins->imm);
				//ARM_RD |= GET(ARM_RM, 31) << 31;
			}
		}
		else
			ARM_RD = ARM_RM >> (ins->imm);
	}

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}


 void add3()
{
	ARM_RD = ARM_RN + ARM_RM;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );

	CPSR_SET_C( carryfrom(ARM_RN, ARM_RM) );
	CPSR_SET_V( overflowfrom(ARM_RN, ARM_RM, OVF_ADD) );
}


 void sub3() // NOT Borrowfrom
{
	ARM_RD = ARM_RN - ARM_RM;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );

	CPSR_SET_C( notborrowfrom(ARM_RN, ARM_RM) );
	CPSR_SET_V( overflowfrom(ARM_RN, ARM_RM, OVF_SUB) );
}

 void mov2()
{
	ARM_RD = ARM_RN;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );

	CPSR_SET_C( 0 );
	CPSR_SET_V( 0 );
}

 void add1()
{
	ARM_RD = ARM_RN + ins->imm;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );

	CPSR_SET_C( carryfrom(ARM_RN, (ins->imm)) );
	CPSR_SET_V( overflowfrom(ARM_RN, (ins->imm), OVF_ADD) );
}

 void sub1()
{
	ARM_RD = ARM_RN - ins->imm;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );

	CPSR_SET_C( notborrowfrom(ARM_RN, (ins->imm)) );
	CPSR_SET_V( overflowfrom(ARM_RN, (ins->imm), OVF_SUB) );

}

 void mov1()
{
	ARM_RD = ins->imm;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void cmp1()
{
	int32u_t alu_out;
	alu_out = ARM_RN - ins->imm;

	CPSR_SET_N(GET(alu_out, 31));
	CPSR_SET_Z( (alu_out==0)? 1:0 );
	CPSR_SET_C( notborrowfrom(ARM_RN, (ins->imm)) );
	CPSR_SET_V( overflowfrom(ARM_RN, (ins->imm), OVF_SUB) );
	 
}

 void add2()
{
	ARM_RD = ARM_RD + ins->imm;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
	CPSR_SET_C( carryfrom(ARM_RD, (ins->imm)) );
	CPSR_SET_V( overflowfrom(ARM_RD, (ins->imm), OVF_ADD) );
}

 void sub2()
{
	ARM_RD = ARM_RD - ins->imm;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
	CPSR_SET_C( notborrowfrom(ARM_RD, (ins->imm)) );
	CPSR_SET_V( overflowfrom(ARM_RD, (ins->imm), OVF_SUB) );
}

 void and()
{
	ARM_RD = ARM_RD & ARM_RM;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void eor()
{
	ARM_RD = ARM_RD ^ ARM_RM;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void lsl2()
{
	if(USUBBITS(ARM_RS, 7, 0)==0)
	{
	}
	else if(USUBBITS(ARM_RS, 7, 0) < 32)
	{
		CPSR_SET_C(GET(ARM_RD, 32 - USUBBITS(ARM_RS, 7, 0)));
		ARM_RD = ARM_RD << (USUBBITS(ARM_RS, 7, 0));
	}
	else if(USUBBITS(ARM_RS, 7, 0) == 32)
	{
		CPSR_SET_C( BIT(ARM_RD, 0) );
		ARM_RD = 0;
	}
	else
	{
		CPSR_SET_C( 0 );
		ARM_RD = 0;
	}

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void lsr2()
{
	if(USUBBITS(ARM_RS, 7, 0)==0)
	{
	}
	else if(USUBBITS(ARM_RS, 7, 0) < 32)
	{
		CPSR_SET_C(GET(ARM_RD, USUBBITS(ARM_RS, 7, 0)-1));
		ARM_RD = ARM_RD >> (USUBBITS(ARM_RS, 7, 0));
	}
	else if(USUBBITS(ARM_RS, 7, 0) == 32)
	{
		CPSR_SET_C( BIT(ARM_RD, 31) );
		ARM_RD = 0;
	}
	else
	{
		CPSR_SET_C( 0 );
		ARM_RD = 0;
	}

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void asr2()
{
	int32s_t sign, i;

	if(USUBBITS(ARM_RS, 7, 0)==0)
	{
	}
	else if(USUBBITS(ARM_RS, 7, 0) < 32)
	{
		CPSR_SET_C(GET(ARM_RD, USUBBITS(ARM_RS, 7, 0)-1));

		sign = GET(ARM_RD, 31);
		for(i = 0; i < USUBBITS(ARM_RS, 7, 0); i++)
		{
			ARM_RD = ARM_RD >> 1;
			ARM_RD |= sign << 31;
			//ARM_RD = USUBBITS(ARM_RD, 31, 0) >> 1;
			//ARM_RD |= sign << 31;
		}
	}
	else
	{
		CPSR_SET_C( BIT(ARM_RD, 31) );

		if(BIT(ARM_RD, 31)==0)
			ARM_RD = 0;
		else
			ARM_RD = 0xFFFFFFFF;
	}
	
	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void adc()
{

	ARM_RD = ARM_RD + ARM_RM + CPSR_GET_C;
	
	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
	CPSR_SET_C( (carryfrom(ARM_RD, ARM_RM)? 1: carryfrom(ARM_RD, ARM_RM + CPSR_GET_C)? 1: 0) );
	CPSR_SET_V( overflowfrom(ARM_RD, (ARM_RM + CPSR_GET_C), OVF_ADD) );

}

 void sbc()
{
	ARM_RD = ARM_RD - ARM_RM - ~CPSR_GET_C;
	
	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
	CPSR_SET_C( notborrowfrom(ARM_RD, (ARM_RM + ~CPSR_GET_C)) );
	CPSR_SET_V( overflowfrom(ARM_RD, (ARM_RM + ~CPSR_GET_C), OVF_SUB) );

}

 void ror()
{
	int32u_t temp;

	if(USUBBITS(ARM_RS, 7, 0)==0)
	{
	}
	else if(USUBBITS(ARM_RS, 4, 0)==0)
	{
		CPSR_SET_C(GET(ARM_RD, 31));
	}
	else
	{
		CPSR_SET_C( GET(ARM_RD, USUBBITS(ARM_RS, 4, 0)-1) );
		temp = USUBBITS(ARM_RD, USUBBITS(ARM_RS, 4, 0)-1, 0);
		ARM_RD = USUBBITS(ARM_RD, 31, USUBBITS(ARM_RS, 4, 0)) >> USUBBITS(ARM_RS, 4, 0);
		ARM_RD |= temp << (31 - USUBBITS(ARM_RS, 4, 0)+1);
	}

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void tst()
{
	int32u_t alu_out;
	alu_out = ARM_RN & ARM_RM;

	CPSR_SET_N(GET(alu_out, 31));
	CPSR_SET_Z( (alu_out==0)? 1:0 );
}

 void neg() // not borrow from
{
	ARM_RD = 0 - ARM_RM;

	CPSR_SET_N(GET(ARM_RN, 31));
	CPSR_SET_Z( (ARM_RN==0)? 1:0 );
	CPSR_SET_C( notborrowfrom(0, ARM_RM) );
	CPSR_SET_V( overflowfrom(0, ARM_RM, OVF_SUB));
}


 void cmp2() // not borrow from
{
	int32u_t alu_out;
	alu_out = ARM_RN - ARM_RM;

	CPSR_SET_N(GET(alu_out, 31));
	CPSR_SET_Z( (alu_out==0)? 1:0 );
	CPSR_SET_C( notborrowfrom(ARM_RN, ARM_RM) );
	CPSR_SET_V( overflowfrom(ARM_RN, ARM_RM, OVF_SUB));
}


 void cmn()
{
	int32u_t alu_out;
	alu_out = ARM_RN + ARM_RM;

	CPSR_SET_N(GET(alu_out, 31));
	CPSR_SET_Z( (alu_out==0)? 1:0 );
	CPSR_SET_C( carryfrom(ARM_RN, ARM_RM) );
	CPSR_SET_V( overflowfrom(ARM_RN, ARM_RM, OVF_ADD));
}

 void orr()
{
	ARM_RD = ARM_RD | ARM_RM;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void mul()
{
	ARM_RD = ARM_RM * ARM_RD;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void bic()
{
	ARM_RD = ARM_RD & ~ARM_RM;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void mvn()
{
	ARM_RD =  ~ARM_RM;

	CPSR_SET_N(GET(ARM_RD, 31));
	CPSR_SET_Z( (ARM_RD==0)? 1:0 );
}

 void add4()
{
	ARM_RD = ARM_RD + ARM_RM;
}

 void cmp3() // not borrow from
{
	int32u_t alu_out;
	alu_out = ARM_RN - ARM_RM;

	CPSR_SET_N(GET(alu_out, 31));
	CPSR_SET_Z( (alu_out==0)? 1:0 );
	CPSR_SET_C( notborrowfrom(ARM_RN, ARM_RM) );
	CPSR_SET_V( overflowfrom(ARM_RN, ARM_RM, OVF_SUB) );

}

 void mov3()
{
	ARM_RD = ARM_RM;
}

 void ldr3()
{
	int32u_t addr;

	addr = (ARM_PC & 0xFFFFFFFC) + (ins->imm * 4);
	ARM_RD = data_buffer[addr>>2];
}

 void str2() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ARM_RM;

	if(addr % 4 != 0) // this is unaligned
		printf("memory address is unaligned, the result is unpredictable\n");

	data_buffer[addr >> 2] = ARM_RD;
}


 void strh2() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ARM_RM;

	if(addr % 2 != 0)
		printf("memory address is unaligned, the result is unpredictable\n");

	data_buffer[addr >> 2] = (addr % 4 == 0)? ((data_buffer[addr>>2] & 0xFFFF0000) | USUBBITS(ARM_RD, 15, 0)) :\
							 (addr % 4 == 2)? ((data_buffer[addr>>2] & 0x0000FFFF) | ( USUBBITS(ARM_RD, 15, 0) << 16 )) : 0;
}

 void strb2() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ARM_RM;

	data_buffer[addr >> 2] = (addr % 4 == 0)? ((data_buffer[addr>>2] & 0xFFFFFF00) | ( USUBBITS(ARM_RD, 7, 0) ) ):\
							 (addr % 4 == 1)? ((data_buffer[addr>>2] & 0xFFFF00FF) | ( USUBBITS(ARM_RD, 7, 0) << 8) ):\
							 (addr % 4 == 2)? ((data_buffer[addr>>2] & 0xFF00FFFF) | ( USUBBITS(ARM_RD, 7, 0) << 16) ):\
							 (addr % 4 == 3)? ((data_buffer[addr>>2] & 0x00FFFFFF) | ( USUBBITS(ARM_RD, 7, 0) << 24) ): 0;

}

 void ldrsb() // sign extended
{
	int32u_t addr;

	addr = ARM_RN + ARM_RM;

	ARM_RD = (addr % 4 == 0)? SIGN_EXTENDED32((int8s_t)USUBBITS(data_buffer[addr >> 2], 7, 0),  8):\
		  	 (addr % 4 == 1)? SIGN_EXTENDED32((int8s_t)USUBBITS(data_buffer[addr >> 2], 15, 8), 8):\
	         (addr % 4 == 2)? SIGN_EXTENDED32((int8s_t)USUBBITS(data_buffer[addr >> 2], 23, 16), 8) :\
	         (addr % 4 == 3)? SIGN_EXTENDED32((int8s_t)USUBBITS(data_buffer[addr >> 2], 31, 24), 8): 0;
}

 void ldr2() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ARM_RM;

	if(addr % 4 != 0)
		printf("memory address is unaligned, the result is unpredictable\n");

	ARM_RD = data_buffer[addr >> 2];
}

 void ldrh2() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ARM_RM;

	if(addr % 2 != 0)
		printf("memory address is unaligned, the result is unpredictable\n");

	ARM_RD = (addr % 4 == 0)? USUBBITS(data_buffer[addr >> 2], 15, 0):\
	         (addr % 4 == 2)? USUBBITS(data_buffer[addr >> 2], 31, 16): 0;
}

 void ldrb2() // zeroextended
{
	int32u_t addr;

	addr = ARM_RN + ARM_RM;

	ARM_RD = (addr % 4 == 0)? ZERO_EXTENDED32((int8u_t)USUBBITS(data_buffer[addr >> 2], 7, 0),  8):\
		  	 (addr % 4 == 1)? ZERO_EXTENDED32((int8u_t)USUBBITS(data_buffer[addr >> 2], 15, 8), 8):\
	         (addr % 4 == 2)? ZERO_EXTENDED32((int8u_t)USUBBITS(data_buffer[addr >> 2], 23, 16), 8) :\
	         (addr % 4 == 3)? ZERO_EXTENDED32((int8u_t)USUBBITS(data_buffer[addr >> 2], 31, 24), 8): 0;
}

 void ldrsh() // sign extended, cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ARM_RM;

	if(addr % 2 != 0)
		printf("memory address is unaligned, the result is unpredictable\n");

	ARM_RD = (addr % 4 == 0)? SIGN_EXTENDED32((int16s_t)USUBBITS(data_buffer[addr >> 2], 15, 0), 16):\
		     (addr % 4 == 2)? SIGN_EXTENDED32((int16s_t)USUBBITS(data_buffer[addr >> 2], 31, 16), 16):0;
}

 void str1() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ins->imm*4;

	if(addr % 4 != 0)
		printf("memory address is unaligned, the result is unpredictable\n");

	data_buffer[addr >> 2] = ARM_RD;
}

 void ldr1() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ins->imm*4;

	if(addr % 4 != 0)
		printf("memory address is unaligned, the result is unpredictable\n");

	ARM_RD = data_buffer[addr >> 2];
}

 void strb1()
{
	int32u_t addr;

	addr = ARM_RN + ins->imm;

	data_buffer[addr >> 2] = (addr % 4 == 0)? ((data_buffer[addr>>2] & 0xFFFFFF00) | ( USUBBITS(ARM_RD, 7, 0)) ) :\
							 (addr % 4 == 1)? ((data_buffer[addr>>2] & 0xFFFF00FF) | ( USUBBITS(ARM_RD, 7, 0)) << 8) :\
							 (addr % 4 == 2)? ((data_buffer[addr>>2] & 0xFF00FFFF) | ( USUBBITS(ARM_RD, 7, 0)) << 16) :\
							 (addr % 4 == 3)? ((data_buffer[addr>>2] & 0x00FFFFFF) | ( USUBBITS(ARM_RD, 7, 0)) << 24) : 0;
}

 void ldrb1() // zero extended
{
	int32u_t addr;

	addr = ARM_RN + ins->imm;

	ARM_RD = (addr % 4 == 0)? ZERO_EXTENDED32((int8u_t)USUBBITS(data_buffer[addr >> 2], 7, 0),  8):\
		  	 (addr % 4 == 1)? ZERO_EXTENDED32((int8u_t)USUBBITS(data_buffer[addr >> 2], 15, 8), 8):\
	         (addr % 4 == 2)? ZERO_EXTENDED32((int8u_t)USUBBITS(data_buffer[addr >> 2], 23, 16), 8) :\
	         (addr % 4 == 3)? ZERO_EXTENDED32((int8u_t)USUBBITS(data_buffer[addr >> 2], 31, 24), 8): 0;
}

 void strh1() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ins->imm*2;

	if(addr % 2 != 0)
		printf("memory address is unaligned, the result is unpredictable\n");

	data_buffer[addr >> 2] = (addr % 4 == 0)? ((data_buffer[addr>>2] & 0xFFFF0000) | USUBBITS(ARM_RD, 15, 0)) :\
							 (addr % 4 == 2)? ((data_buffer[addr>>2] & 0x0000FFFF) | ( USUBBITS(ARM_RD, 15, 0) << 16 )) : 0;
}

 void ldrh1() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_RN + ins->imm*2;

	if(addr % 2 != 0)
		printf("memory address is unaligned, the result is unpredictable\n");

	ARM_RD = (addr % 4 == 0)? ZERO_EXTENDED32(USUBBITS(data_buffer[addr >> 2], 15, 0), 16):\
	         (addr % 4 == 2)? ZERO_EXTENDED32(USUBBITS(data_buffer[addr >> 2], 31, 16), 16): 0;
}

 void str3() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_SP + ins->imm*4;

	if(addr % 4 != 0) // this is unaligned
		printf("memory address is unaligned, the result is unpredictable\n");

	data_buffer[addr >> 2] = ARM_RD;
}

 void ldr4() // cp15 modeling is needed
{
	int32u_t addr;

	addr = ARM_SP + ins->imm*4;

	if(addr % 4 != 0)
		printf("memory address is unaligned, the result is unpredictable\n");

	ARM_RD = data_buffer[addr >> 2];
}

 void add5()
{
	ARM_RD = (ARM_PC & 0xFFFFFFFC) + ins->imm*4;
}

 void add6()
{
	ARM_RD = ARM_SP + (ins->imm << 2);
}

 void add7()
{
	ARM_SP += (ins->imm << 2);
}

 void sub4()
{
	ARM_SP -= (ins->imm << 2);
}

 void push()
{
	int32u_t i, j, start;
	j = 0, start = 0;

	for(i = 0; i < 9; i++)
		j += ((ins->imm >> i) & 0x1);

	ARM_SP -= j*4;
	start = ARM_SP;

	for(i = 0; i < 9; i++, start += 4)
	{
		if((ins->imm >> i) & 0x1)
		{
			if(i==8) // LR
				data_buffer[start>>2] = ARM_LR;
			else
				data_buffer[start>>2] = ARM_R(i);
		}

	}
}

 void pop()
{
	int32u_t i, j, start;
	j = 0, start = 0;

	for(i = 0; i < 9; i++)
		j += ((ins->imm >> i) & 0x1);

	start = ARM_SP;
	ARM_SP += j*4;

	for(i = 0; i < 9; i++, start += 4)
	{
		if((ins->imm >> i) & 0x1)
		{
			if(i==8) // PC
			{
				ARM_PC = data_buffer[start>>2];
				if(ARM_PC & 0x1)
					CPSR_SET_THUMB(1); // thumb mode
				else
					CPSR_SET_THUMB(0); // arm mode
			}
			else
				ARM_R(i) = data_buffer[start>>2];
		}

	}
}

 void bkpt()
{
}

 void stmia()
{
	int32u_t i, j, start;
	j = 0, start = 0;

	for(i = 0; i < 8; i++)
		j += ((ins->imm >> i) & 0x1);

	start = ARM_RN;
	ARM_RN += j*4;

	for(i = 0; i < 8; i++, start += 4)
	{
		if((ins->imm >> i) & 0x1)
			data_buffer[start>>2] = ARM_R(i);
	}
}

 void ldmia()
{
	int32u_t i, j, start;
	j = 0, start = 0;

	for(i = 0; i < 8; i++)
		j += ((ins->imm >> i) & 0x1);

	start = ARM_RN;
	ARM_RN += j*4;

	for(i = 0; i < 8; i++, start += 4)
	{
		if((ins->imm >> i) & 0x1)
			ARM_R(i) = data_buffer[start>>2];
	}
}

 void b1()
{
	if(CPSR_GET_NZCV==ins->cond) // condition pass check
		ARM_PC += (SIGN_EXTENDED32(ins->imm, 8) << 1);

}

 void b2()
{
	ARM_PC += (SIGN_EXTENDED32(ins->imm, 11) << 1);
}

 void bl_h2()
{
	ARM_LR = ARM_PC + (SIGN_EXTENDED32(ins->imm, 11) << 12);
}

 void bl_h3()
{
	int32u_t temp;
	temp = ARM_PC;

	ARM_PC = ARM_LR + (ins->imm << 1);
	ARM_LR = (temp+2)|1;
}

