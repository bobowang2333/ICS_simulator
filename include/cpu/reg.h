#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
    /* change the structure to union to make sure that the data in same memory*/
	union{    
        struct{
            union{
		        uint32_t _32;
		        uint16_t _16;
		        uint8_t _8[2];
           // uint32_t eax,ecx,edx,ebx,esp,ebp,esi,edi;
                };   
	    } gpr[8];

        struct{
        uint32_t eax,ecx,edx,ebx,esp,ebp,esi,edi;
        };

        struct {
        uint16_t ax, panding1,cx,panding2,dx,panding3,bx,panding4,sp,panding5,bp,panding6,si,panding7,di,panding8;
        };

        struct {
        uint8_t al,ah;
        uint16_t panding9;
        uint8_t cl,ch;
        uint16_t panding10;
        uint8_t dl,dh;
        uint16_t panding11;
        uint8_t bl,bh;
        uint16_t panding12;
        };
      };

	/* Do NOT change the order of the GPRs' definitions. */
    /* redifine the register in union */
	   // uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
	swaddr_t eip;
    union{
        uint32_t reg;
        struct{
            uint32_t CF:1;
            uint32_t panding1:1; /* the bit NOT USED*/
            uint32_t PF:1;
            uint32_t panding2:1;
            uint32_t AF:1;
            uint32_t panding3:1;
            uint32_t ZF:1;
            uint32_t SF:1;
            uint32_t TF:1;
            uint32_t IF:1;
            uint32_t DF:1;
            uint32_t OF:1;
            uint32_t IOPL:2;
            uint32_t NT:1;
            uint32_t panding4:1;
            uint32_t RF:1;
            uint32_t VM:1;
        };
    } eflags;

    uint8_t current_sreg;
} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

#endif