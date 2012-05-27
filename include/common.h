#ifndef _COMMON_H_
#define	_COMMON_H_

typedef unsigned char int8u_t;
typedef unsigned short int16u_t;
typedef unsigned int int32u_t;

typedef char int8s_t;
typedef short int16s_t;
typedef int int32s_t;

// instruction buffer

// handler parameter
typedef struct _handler_param {
	int32u_t start;
	int32u_t stop;
	int32u_t breakp;
	int32u_t step;

	int32u_t verbose;
} hparam_t;

// instruction buffer
int32u_t *inst_buffer;
// data memory
int32u_t data_buffer[1024];

// handler parameter
hparam_t *hparam;

int init_logo();
int init_buffer();
int init_handler_param();

void mem_display(int32u_t, int32u_t);

#ifdef DEBUG
#define debug(fmt, args...) printf(fmt ,##args)
#else
#define debug(fmt, args...) 
#endif

// utility
#define WORD_SIZE 32

#define ALIGN(x, align) ( ( (x) >> (align >> 1) ) << (align >> 1) )

#define GET(A,i)   			 ((A>>i)&0x00000001)
#define SET(A,i,x) 			 A=((A>>(i+1))<<(i+1))|x<<i|((A<<(32-i))>>(32-i))
#define SET_BITS(A,i,j,x)  	 A=((A>>(i+1))<<(i+1))|x<<j|((A<<(32-j))>>(32-j))
#define SUBBITS(a, i, j) 	 (((signed)(a)<<((WORD_SIZE-1)-(i)))>>((WORD_SIZE-1)-(i)+(j)))
#define USUBBITS(a, i, j) 	 (((unsigned)(a)<<((WORD_SIZE-1)-(i)))>>((WORD_SIZE-1)-(i)+(j)))
#define BIT(a, i)			 (SUBBITS((a), (i), (i)) & 0x1)


#endif
