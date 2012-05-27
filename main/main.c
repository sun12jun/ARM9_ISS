#include <stdio.h>
#include <stdlib.h>

#include <arm.h>
#include <common.h>
#include <lib_string.h>
#include <handler.h>

#include "init.h"


#define MAX_CMD_SIZE 50
#define MAX_TOKEN_NUM 6
#define MAX_TOKEN_SIZE 16

typedef int (init_fnc_t) (void);

init_fnc_t *init_sequence[] = {
	init_ARM_env,
	init_ARM_ins,
	init_buffer,
	init_handler_param,
	init_logo,
	display_ARM_env,

	NULL
};


int main()
{
	// initial setting
	int32u_t running;
	int32s_t i, num;

	running = 1;

	// buffer for user command input
	int8s_t cmd[MAX_CMD_SIZE], string[MAX_TOKEN_NUM][MAX_TOKEN_SIZE], *token[MAX_TOKEN_NUM];
	for(i = 0; i < MAX_TOKEN_NUM; i++) token[i]=string[i];

	// initialization
	init_fnc_t **init_fnc_ptr;

	for(init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr)
		if((*init_fnc_ptr)() != 0)
			printf("initialization error\n");

	// ISS main loop
	while(running) 
	{
		// update ARM status
		if(CPSR_GET_THUMB) // this is thumb mode
			hparam->step = 2;
		else			// this arm mode
			hparam->step = 4;

		printf("ISS > ");

		// get user input
		if(fgets(cmd, sizeof(cmd), stdin)==NULL)
			printf("please, re-enter your command\n");
		else
			num = tokenize(token, cmd);

		if(num==1) // this is 's' or 'r' or 'q' or CR
		{
			if(!strcmp(token[0], "s\n") || !strcmp(token[0], "\n"))
			{
				debug("I'm in 's'\n");

				hparam->start = ARM_PC;
				hparam->stop = ARM_PC + hparam->step;
				hparam->verbose = 1;

			}
			else if(!strcmp(token[0], "r\n"))
			{
				debug("I'm in 'r'\n");

				hparam->start = ARM_PC;
				//hparam->stop = ARM_PC + hparam->step;
				hparam->stop = ( (hparam->breakp != 0) && (ARM_PC < hparam->breakp) )? hparam->breakp : (sizeof(data_buffer)/ sizeof(data_buffer[0])) *hparam->step;
				hparam->verbose = 1;
			}
			else if(!strcmp(token[0], "q\n"))
			{
				debug("quit ISS\n");
				running = 0;
			}
			else
			{
				debug("not matched with s, r, q\n");
				continue;
			}
					
		}
		else if(num==2) // this is breakp
		{
			if(!strcmp(token[0], "bp"))
			{
				debug("I'm in 'bp'\n");

				hparam->start = ARM_PC;
				if(str2int(token[1], 10) < ARM_PC)
				   printf("please re-type bp instruction with valid bp address\n");
				else
					hparam->breakp = str2int(token[1], 10);
				hparam->verbose = 1;
			}
			else
			{
				printf("please re-type valid command\n");
				continue;
			}
		}
		else if(num==3) // this is dm
		{
			if(!strcmp(token[0], "dm"))
			{
				debug("I'm in 'dm'\n");

				if(str2int(token[1], 10) > str2int(token[2], 10))
				   printf("please re-type dm instruction with valid dm address\n");
				else
				{
					if( (str2int(token[1], 10) & 0x3 != 0)  || (str2int(token[2], 10) & 0x3 != 0 ) )
						printf("please re-type dm instruction with 4-byte align address\n");
					else
						mem_display(str2int(token[1], 10), str2int(token[2], 10));
				}

			}
			else
				printf("please re-type valid command\n");

			continue;
		}
		else
		{
			printf("invalid command, please re-enter the command\n");
			continue;
		}

		// handler call
		// performs Fetch, Decode, Execute
		handler(hparam);

	}

	free (inst_buffer);
	return 0;
}

int display_ARM_env()
{
	int i;

	printf("-------------------------------------------------------------------------------\n");
	printf("	Mode : %s 			PROCESSING MODE: %s\n",\
			CPSR_GET_THUMB? "THUMB":"ARM",\  
			(CPSR_GET_MODE==MODE_USR)? "USR":\
			(CPSR_GET_MODE==MODE_FIQ)? "FIQ":\
			(CPSR_GET_MODE==MODE_IRQ)? "IRQ":\
			(CPSR_GET_MODE==MODE_SVR)? "SVR":\
			(CPSR_GET_MODE==MODE_ABT)? "ABT":\
			(CPSR_GET_MODE==MODE_UND)? "UND":\
			(CPSR_GET_MODE==MODE_SYS)? "SYS": "ERR");
	printf("	NZCV : %1d%1d%1d%1d                      IRQ FIQ: %1d%1d\n",\
			CPSR_GET_N, CPSR_GET_Z, CPSR_GET_C, CPSR_GET_V, CPSR_GET_IRQ, CPSR_GET_FIQ);
	printf("-------------------------------------------------------------------------------\n");
	printf("	GPR_PC: %08x(address for next fetched and executed) 	INST:%04x\n", ARM_PC, ins->opcode_thumb);
	printf("-------------------------------------------------------------------------------\n");

	for(i = 0; i < 16; i++)
	{
		printf("GPR[%02d] : %08x  ", i, env->regs[i]);

		if((i % 4)==3) printf("\n");
	}
	printf("-------------------------------------------------------------------------------\n");

	return 0;
}

void mem_display(int32u_t from, int32u_t to)
{
	int32u_t i, base;

	base = from;

	printf("------------------------------ \n");
	printf("address   : data( LSB -> MSB ) \n");
	printf("------------------------------ \n");
	for(i = 0; i < (to-from)>>2; i++, base += 4)
		printf("0x%08x: 0x%08x\n", base, data_buffer[base>>2]);	
	printf("------------------------------ \n");
}

