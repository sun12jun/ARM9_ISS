#ifndef _THUMB1_H_
#define _THUMB1_H_

#include <common.h>

#define OVF_ADD 0
#define OVF_SUB 1


#define SIGN_EXTENDED32(i,n) ((signed)(((unsigned)(i))<<(32-(n)))>>(32-(n)) )
#define ZERO_EXTENDED32(i,n) ( (unsigned)(i) )

void thumb1_fetch();
void thumb1_decode();

inline int32u_t carryfrom(int32u_t a, int32u_t b);
inline int32u_t overflowfrom(int32u_t a, int32u_t b, int32u_t nadd_sub);
inline int32u_t notborrowfrom(int32u_t a, int32u_t b);


char buffer[4];

void lsl1();
void lsr1();
void asr1();
void add3(); // Carryfrom, overflowfrom
void sub3(); // NOTBorrowfrom, overflowfrom
void mov2();
void add1(); // Carryfrom, overflowfrom
void sub1(); // NotBorrowfrom, overflowfrom
void mov1();
void cmp1(); // NotBorrowfrom, overflowfrom
void add2(); // Carryfrom, overflowfrom
void sub2(); // NotBorrowfrom, overflowfrom
void and();
void eor();
void lsl2();
void lsr2();
void asr2();
void adc();   // Carryfrom, overflowfrom
void sbc();	 // Notborrowfrom, overflowfrom
void ror();
void tst();
void neg();   // Notborrowfrom, overflowfrom
void cmp2();  // Notborrowfrom, overflowfrom
void cmn();	 // Carryfrom, overflowfrom
void orr();
void mul();
void bic();
void mvn();
void add4();
void cmp3();  // Notborrowfrom, overflowfrom
void mov3();
void ldr3();
void str2();  // cp15 modeling is needed
void strh2(); // cp15 modeling is needed
void strb2(); // cp15 modeling is needed
void ldrsb(); // sign extended
void ldr2();  // cp15 modeling is needed
void ldrh2(); // zero extend, cp15 modeling is needed
void ldrb2();  // zero extended
void ldrsh(); // sign extended, cp15 modeling is needed
void str1();  // cp15 modeling is needed
void ldr1();  // cp15 modeling is needed
void strb1();
void ldrb1(); // zero extended
void strh1(); // cp15 modeling is needed
void ldrh1(); // cp15 modeling is needed
void str3();  // cp15 modeling is needed
void ldr4();  // cp15 modeling is needed
void add5();
void add6();
void add7();
void sub4();
void push();
void pop();
void bkpt();
void stmia();
void ldmia();
void b1();	 // sign extend
void b2();	 // sign extend
void bl_h2(); // sign extend
void bl_h3();



#endif
