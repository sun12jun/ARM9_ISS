#ifndef _ARM_H_
#define	_ARM_H_

#include "common.h"


// the source context block
typedef struct _CPUState {
	int32u_t regs[16];      // R0-R15 of SYS and USR mode
	int32u_t r13[5];        // R13 of 5 exception modes
	int32u_t r14[5];        // R14 of 5 exception modes
	// 0-SVR, 1-ABT, 2-UND, 3-IRQ, 4-FIQ
	int32u_t fiq_reg[5];    // R8-R12 of FIQ mode
	int32u_t cpsr;          // CPSR
	int32u_t spsr[5];  		// SPSR for 5 exception modes
	int32u_t mode;       	// Processor working modes
} CPUState_t;

// the data structure
typedef struct _instruction {
	int32u_t opcode_arm; 			// opcode
	int32u_t opcode_thumb; 			// opcode
	int32u_t de1,de2; 			// results of the two-stage decoding
	int32u_t* rd; 				// point to corresponding registers
	int32u_t* rm; 				// point to corresponding registers
	int32u_t* rs; 				// point to corresponding registers
	int32u_t* rn; 				// point to corresponding registers
	int32u_t imm; 				// immediate number
	int32u_t cond; 				// condition field
	void (*exec)(); 			// point to the emulation routine
	int32u_t (*test_cond)(); 	// point to the test function
} Instruction_t;

CPUState_t *env;
Instruction_t *ins;

/*	ARM Mode
 *	SYS Privileged mode, for OS
 *	USR User mode, for user programs
 *	FIQ Fast interrupt, used for high speed data transfer
 *	IRQ Used for general external interrupts
 *	SVC Protected mode used by the OS
 *	ABT Used for virtual memory management
 *	UND Used for occurrence of undeﬁned instructions
 */

#define MODE_USR	0x10
#define MODE_FIQ	0x11
#define MODE_IRQ	0x12
#define MODE_SVR	0x13
#define MODE_ABT	0x17
#define MODE_UND	0x1B
#define MODE_SYS	0x1F

#define CPSR_GET_NZCV			(USUBBITS((env->cpsr), 31, 28))
#define CPSR_GET_N				(BIT((env->cpsr), 31))
#define CPSR_GET_Z				(BIT((env->cpsr), 30))
#define CPSR_GET_C				(BIT((env->cpsr), 29))
#define CPSR_GET_V				(BIT((env->cpsr), 28))
#define CPSR_GET_MODE			(USUBBITS((env->cpsr), 4, 0))
#define CPSR_GET_FLAG			(USUBBITS((env->cpsr), 31, 27))
#define CPSR_GET_IRQ			(BIT((env->cpsr), 7))
#define CPSR_GET_FIQ			(BIT((env->cpsr), 6))
#define CPSR_GET_THUMB			(BIT((env->cpsr), 5))

#define CPSR_SET_N(x)			(SET((env->cpsr), 31, (x)))
#define CPSR_SET_Z(x)			(SET((env->cpsr), 30, (x)))
#define CPSR_SET_C(x)			(SET((env->cpsr), 29, (x)))
#define CPSR_SET_V(x)			(SET((env->cpsr), 28, (x)))
#define CPSR_SET_MODE(x)		(SET_BITS((env->cpsr), 4, 0, (x)))
#define CPSR_SET_FLAG(x)		(SET_BITS((env->cpsr), 31, 27, (x)))
#define CPSR_SET_IRQ(x)			(SET((env->cpsr), 7, (x)))
#define CPSR_SET_FIQ(x)			(SET((env->cpsr), 6, (x)))
#define CPSR_SET_THUMB(x)		(SET((env->cpsr), 5, (x)))


#define ARM_R0	(env->regs[0])
#define ARM_R1	(env->regs[1])
#define ARM_R2	(env->regs[2])
#define ARM_R3	(env->regs[3])
#define ARM_R4	(env->regs[4])
#define ARM_R5	(env->regs[5])
#define ARM_R6	(env->regs[6])
#define ARM_R7	(env->regs[7])
#define ARM_R8	(env->regs[8])
#define ARM_R9	(env->regs[9])
#define ARM_R10	(env->regs[10])
#define ARM_R11	(env->regs[11])
#define ARM_R12	(env->regs[12])
#define ARM_R13	(env->regs[13])
#define ARM_R14	(env->regs[14])
#define ARM_R15	(env->regs[15])
#define ARM_SP	ARM_R13
#define ARM_LR	ARM_R14
#define ARM_PC	ARM_R15

#define ARM_R(x)	(env->regs[i])

#define ARM_RD (*(int32u_t*)(ins->rd))
#define ARM_RM (*(int32u_t*)(ins->rm))
#define ARM_RN (*(int32u_t*)(ins->rn))
#define ARM_RS (*(int32u_t*)(ins->rs))

int init_ARM_env();
int init_ARM_ins();

int display_ARM_env();

#endif
