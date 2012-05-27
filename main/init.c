#include <stdio.h>
#include <stdlib.h>

#include <arm.h>
#include <common.h>

#include "init.h"

int init_ARM_ins()
{
	debug("ARM instruction context is initialized\n");

	ins = (Instruction_t *)malloc(sizeof(Instruction_t));

	if(ins==NULL)
		return 1;

	ins->opcode_arm = 0;
	ins->opcode_thumb = 0;
	ins->de1 = 0;
	ins->de2 = 0;
	ins->rd = NULL;
	ins->rm = NULL;
	ins->rs = NULL;
	ins->rn = NULL;
	ins->imm = 0;
	ins->cond = 0;
	ins->exec = NULL;
	ins->test_cond = NULL;

	return 0;
}

int init_ARM_env()
{
	debug("ARM environment context is initialized\n");

	int i;
	env	= (CPUState_t*)malloc(sizeof(CPUState_t));

	if(env==NULL)
		return 1;

	for(i = 0; i < 16; i++)
		env->regs[i] = 0;

	for(i = 0; i < 5; i++)
	{
		env->r13[i] = 0;
		env->r14[i] = 0;
		env->fiq_reg[i] = 0;
		env->spsr[i] = 0;
	}

	// cpsr setting
	env->cpsr = 0;
	CPSR_SET_MODE(MODE_USR);
	CPSR_SET_FLAG(0);
	CPSR_SET_IRQ(1);
	CPSR_SET_FIQ(1);
	CPSR_SET_THUMB(1);

	env->mode = MODE_USR;

	// stack pointer setting
	env->regs[13] = 0x100;
	return 0; 
}

int init_buffer()
{
	FILE *pFile; 
	int32u_t result, lSize, i;

	pFile = fopen ("./data_proc.bin" , "rb" );

	if (pFile==NULL)
	{
		fputs ("File error\n", stderr);
		return 1;
	}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	inst_buffer = (int32u_t*) malloc (sizeof(int32u_t)*lSize);
	if (inst_buffer == NULL) 
	{
		fputs ("Memory error\n", stderr);
		return 1;
	}

	// copy the file into the buffer:
	result = fread(inst_buffer, sizeof(int32u_t), lSize >> 2, pFile);
	if (result == 0)
	{
		fputs ("Reading error\n", stderr); 
		return 1;
	}

	fclose (pFile);

	// data buffer initialization
	for(i = 0; i < (sizeof(data_buffer)/sizeof(data_buffer[0])); i++)
		data_buffer[i] = 0;

	return 0;
}

int init_handler_param()
{
	hparam = (hparam_t*)malloc(sizeof(hparam_t));

	if(hparam==NULL)
		return 0;

	hparam->start = 0;
	hparam->stop = 0;
	hparam->breakp = 0;
	hparam->step = 0;

	hparam->verbose = 0;

	return 0;
}

int init_logo()
{
	printf("\n");
	printf("\n");
	printf("9yy#                yeeeeeeeeee#yW               DKeeeeeeeeeey5,               \n");  
	printf("9eeeee            eeeeeeeeeeeeeeeeeez           eeeeeeeeeeeeeeeeee             \n");   
	printf("5eeeee          ,eeeeeee      eeeeee#         zeeeeee5     ,eeeeee             \n");   
	printf("5eeeee         DeeeeeX           Keee        eeeeee            zee             \n");   
	printf("5eeeee         eeeeeu                        eeeee                             \n");   
	printf("5eeeee          eeeeeeee                     ueeeeeeez                         \n");   
	printf("5eeeee           eeeeeeeeeey                   eeeeeeeeeG                      \n");   
	printf("5eeeee             eeeeeeeeeeee                  eeeeeeeeeeeE                  \n");   
	printf("5eeeee                eeeeeeeeeeee,                ,eeeeeeeeeeeG               \n");   
	printf("5eeeee                   zeeeeeeeeee                  uEeeeeeeeeeE             \n");   
	printf("5eeeee                           Xeeeee                        eeeee           \n");   
	printf("5eeeee                           eeeeeE                       eeeeee           \n");   
	printf("5eeeee       Xee               Keeeeee     eeX               #eeeeey           \n");   
	printf("5eeeee       zeeeeee5        eeeeeeee      eeeeeee        #eeeeeeeG            \n");   
	printf("Xeeeee       zeeeeeeeeeeeeeeeeeeeee9       eeeeeeeeeeeeeeeeeeeeee,             \n");   
	printf("Xeeee            ueeeeeeeeeeeeeW               Xeeeeeeeeeeee              YDJ  \n");
	printf("\n");
	printf("\n");

	return 0;
}

