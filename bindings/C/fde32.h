//
// un[f]ancy [d]isassembler [e]ngine (fde32) v0.01
//

#ifndef FDE32_H
#define FDE32_H

// flags

#define F_NONE		    0x00000000
#define F_MODRM 	    0x00000001
#define F_SIB		    0x00000002
#define F_DISP8 	    0x00000004
#define F_DISP16	    0x00000008
#define F_DISP32	    0x00000010
#define F_DISP		    F_DISP8+F_DISP16+F_DISP32
#define F_IMM8		    0x00000020
#define F_IMM16 	    0x00000040
#define F_IMM32 	    0x00000080
#define F_IMM		    F_IMM8+F_IMM16+F_IMM32
#define F_RELATIVE	    0x00000100
#define F_GROUP 	    0x00000200	     // index specified in modrm.reg
#define F_VEX_BAD_PREFIX    0x00000400
#define F_ERROR_LOCK	    0x00000800	     // lock-prefix not possible
#define F_ERROR_LENGTH	    0x00001000
#define F_ERROR_OPCODE	    0x00002000	     // undefined opcode

// prefix flags

#define PRE_NONE	    0x00h
#define PRE_LOCK	    0x01h
#define PRE_REP 	    0x02h
#define PRE_SEG 	    0x04h
#define PRE_66		    0x08h	     // ignored if rex.w=1
#define PRE_67		    0x10h
#define PRE_VEX 	    0x20h	     // partly encoded in complements to distinguish from les/lds
#define PRE_ALL 	    PRE_LOCK+PRE_REP+PRE_SEG+PRE_66+PRE_67
#define PRE_ALL32	    PRE_ALL+PRE_VEX

// vex-prefix m-mmmm

#define M_MMMM_0F	    1
#define M_MMMM_0F_38	    2
#define M_MMMM_0F_3A	    3

// vex-prefix l

#define L_SCALAR	    0
#define L_128_VECTOR	    0
#define L_256_VECTOR	    1

// vex-prefix pp

#define PP_NONE 	    0
#define PP_66		    1
#define PP_F3		    2
#define PP_F2		    3

// modr/m mod

#define MOD_NODISP	    0
#define MOD_DISP8	    1
#define MOD_DISP32	    2
#define MOD_REG 	    3

// modr/m reg

/* GPRs */
#define REG_EAX 	    0
#define REG_ECX 	    1
#define REG_EDX 	    2
#define REG_EBX 	    3
#define REG_ESP 	    4
#define REG_EBP 	    5
#define REG_ESI 	    6
#define REG_EDI 	    7
#define REG_AL		    REG_EAX
#define REG_AH		    REG_ESP
#define REG_CL		    REG_ECX
#define REG_CH		    REG_EBP
#define REG_DL		    REG_EDX
#define REG_DH		    REG_ESI
#define REG_BL		    REG_EBX
#define REG_BH		    REG_EDI
/* special */
#define REG_DR0 	    REG_EAX
#define REG_DR1 	    REG_ECX
#define REG_DR2 	    REG_EDX
#define REG_DR3 	    REG_EBX
#define REG_DR4 	    REG_ESP	     // refers to DR6 if CR4.DE is cleared
#define REG_DR5 	    REG_EBP	     // refers to DR7 ^
#define REG_DR6 	    REG_ESI
#define REG_DR7 	    REG_EDI
#define REG_CR0 	    REG_EAX
#define REG_CR2 	    REG_EDX
#define REG_CR3 	    REG_EBX
#define REG_CR4 	    REG_ESP
/* MMX/XMM/YMM */
#define REG_SIMD0	    REG_EAX
#define REG_SIMD1	    REG_ECX
#define REG_SIMD2	    REG_EDX
#define REG_SIMD3	    REG_EBX
#define REG_SIMD4	    REG_ESP
#define REG_SIMD5	    REG_EBP
#define REG_SIMD6	    REG_ESI
#define REG_SIMD7	    REG_EDI
/* FPU */
#define REG_ST0 	    REG_EAX
#define REG_ST1 	    REG_ECX
#define REG_ST2 	    REG_EDX
#define REG_ST3 	    REG_EBX
#define REG_ST4 	    REG_ESP
#define REG_ST5 	    REG_EBP
#define REG_ST6 	    REG_ESI
#define REG_ST7 	    REG_EDI
/* Sregs */
#define SEG_ES		    REG_EAX
#define SEG_CS		    REG_ECX
#define SEG_SS		    REG_EDX
#define SEG_DS		    REG_EBX
#define SEG_FS		    REG_ESP
#define SEG_GS		    REG_EBP

// modr/m r/m

#define RM_SIB		    REG_ESP
#define RM_DISP32	    REG_EBP

// sib scale

#define SCALE_1 	    0
#define SCALE_2 	    1
#define SCALE_4 	    2
#define SCALE_8 	    3

// prefixes

#define PREFIX_SEGMENT_CS   0x2E
#define PREFIX_SEGMENT_SS   0x36
#define PREFIX_SEGMENT_DS   0x3E
#define PREFIX_SEGMENT_ES   0x26
#define PREFIX_SEGMENT_FS   0x64
#define PREFIX_SEGMENT_GS   0x65
#define PREFIX_LOCK	    0xF0
#define PREFIX_REPNZ	    0xF2
#define PREFIX_REP	    0xF3
#define PREFIX_OPERAND_SIZE 0x66
#define PREFIX_ADDRESS_SIZE 0x67
#define PREFIX_VEX_2_BYTE   0xC5
#define PREFIX_VEX_3_BYTE   0xC4

// structure representing any instruction

#pragma pack(push, 1)
struct fde32s {
	unsigned char len;
	unsigned char prefix;
	unsigned char prefix_lock;
	unsigned char prefix_rep;
	unsigned char prefix_seg;
	unsigned char prefix_66;
	unsigned char prefix_67;
	unsigned char vex;
	unsigned char vex2;
	unsigned char vex3;
	unsigned char vex_r;
	unsigned char vex_x;
	unsigned char vex_b;
	unsigned char vex_m_mmmm;
	unsigned char vex_w;
	unsigned char vex_vvvv;
	unsigned char vex_l;
	unsigned char vex_pp;
	unsigned char opcode_len;
	unsigned char opcode;
	unsigned char opcode2;
	unsigned char opcode3;
	unsigned char modrm;
	unsigned char modrm_mod;
	unsigned char modrm_reg;
	unsigned char modrm_rm;
	unsigned char sib;
	unsigned char sib_scale;
	unsigned char sib_index;
	unsigned char sib_base;
	union {
		char disp8;
		short disp16;
		int disp32;
	};
	union {
		char imm8;
		short imm16;
		int imm32;
	};
	union {
		char imm8_2;
		short imm16_2;
	};
	unsigned int flags;
};
#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif

int encode(void *dest, struct fde32s *cmd);
int decode(const void *src, struct fde32s *cmd);

#ifdef __cplusplus
}
#endif

#endif /* FDE32_H */
