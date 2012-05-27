#include <stdio.h>
#include <stdlib.h>

#include <common.h>
#include <arm.h>

#include "../arm/armv5/armv5.h"
#include "../thumb/thumb1/thumb1.h"
#include "handler.h"

extern int display_ARM_env();

void handler(hparam_t *param)
{
	//if(GET_CPSR_THUMB)
	//	int16u_t inst;
	//else
		//int32u_t inst;

	debug("im in handler\n");

	//while((ARM_PC < param->breakp) && (ARM_PC < param->stop))
	while((ARM_PC < param->stop))
	{
		// fetch
		//inst = thumb1_fetch();
		thumb1_fetch();

		// decode
		thumb1_decode();

		// execute
		ins->exec();

		// PC is increased except for branch
		if( !(ins->exec==b1) && !(ins->exec==b2) && !(ins->exec==bl_h2) && !(ins->exec==bl_h3) )
			ARM_PC += param->step;

	}

	// display
	if(param->verbose)
		display_ARM_env();
}

