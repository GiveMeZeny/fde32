	include 'win32w.inc'

  ; structure representing any instruction

struct fde32s
  len	      rb 1
  prefix      rb 1
  prefix.lock rb 1
  prefix.rep  rb 1
  prefix.seg  rb 1
  prefix.66   rb 1
  prefix.67   rb 1
  vex	      rb 1
  vex2	      rb 1
  vex3	      rb 1
  vex.r       rb 1
  vex.x       rb 1
  vex.b       rb 1
  vex.m_mmmm  rb 1
  vex.w       rb 1
  vex.vvvv    rb 1
  vex.l       rb 1
  vex.pp      rb 1
  opcode.len  rb 1
  opcode      rb 1
  opcode2     rb 1
  opcode3     rb 1
  modrm       rb 1
  modrm.mod   rb 1
  modrm.reg   rb 1
  modrm.rm    rb 1
  sib	      rb 1
  sib.scale   rb 1
  sib.index   rb 1
  sib.base    rb 1
  union
    disp8     rb 1
    disp16    rw 1
    disp32    rd 1
  ends
  union
    imm8      rb 1
    imm16     rw 1
    imm32     rd 1
  ends
  union
    imm8_2    rb 1
    imm16_2   rw 1
  ends
  flags       rd 1
ends

  ; flags

  F_NONE	      = 00000000h
  F_MODRM	      = 00000001h
  F_SIB 	      = 00000002h
  F_DISP8	      = 00000004h
  F_DISP16	      = 00000008h
  F_DISP32	      = 00000010h
  F_DISP	      = F_DISP8+F_DISP16+F_DISP32
  F_IMM8	      = 00000020h
  F_IMM16	      = 00000040h
  F_IMM32	      = 00000080h
  F_IMM 	      = F_IMM8+F_IMM16+F_IMM32
  F_RELATIVE	      = 00000100h
  F_GROUP	      = 00000200h	; index specified in modrm.reg
  F_VEX_BAD_PREFIX    = 00000400h
  F_ERROR_LOCK	      = 00000800h	; lock-prefix not possible
  F_ERROR_LENGTH      = 00001000h
  F_ERROR_OPCODE      = 00002000h	; undefined opcode

  ; prefix flags

  PRE_NONE	      = 000h
  PRE_LOCK	      = 001h
  PRE_REP	      = 002h
  PRE_SEG	      = 004h
  PRE_66	      = 008h
  PRE_67	      = 010h
  PRE_VEX	      = 020h		; partly encoded in complements to distinguish from les/lds
  PRE_ALL	      = PRE_LOCK+PRE_REP+PRE_SEG+PRE_66+PRE_67
  PRE_ALL32	      = PRE_ALL+PRE_VEX

  ; vex-prefix m-mmmm

  M_MMMM_0F	      = 1
  M_MMMM_0F_38	      = 2
  M_MMMM_0F_3A	      = 3

  ; vex-prefix l

  L_SCALAR	      = 0
  L_128_VECTOR	      = 0
  L_256_VECTOR	      = 1

  ; vex-prefix pp

  PP_NONE	      = 0
  PP_66 	      = 1
  PP_F3 	      = 2
  PP_F2 	      = 3

  ; modr/m mod

  MOD_NODISP	      = 0
  MOD_DISP8	      = 1
  MOD_DISP16	      = 2
  MOD_DISP32	      = 2
  MOD_REG	      = 3

  ; modr/m reg

  ; GPRs
  REG_EAX	      = 0
  REG_ECX	      = 1
  REG_EDX	      = 2
  REG_EBX	      = 3
  REG_ESP	      = 4
  REG_EBP	      = 5
  REG_ESI	      = 6
  REG_EDI	      = 7
  REG_AL	      = REG_EAX
  REG_AH	      = REG_ESP
  REG_CL	      = REG_ECX
  REG_CH	      = REG_EBP
  REG_DL	      = REG_EDX
  REG_DH	      = REG_ESI
  REG_BL	      = REG_EBX
  REG_BH	      = REG_EDI
  ; special
  REG_CR0	      = REG_EAX
  REG_CR2	      = REG_EDX
  REG_CR3	      = REG_EBX
  REG_CR4	      = REG_ESP
  REG_DR0	      = REG_EAX
  REG_DR1	      = REG_ECX
  REG_DR2	      = REG_EDX
  REG_DR3	      = REG_EBX
  REG_DR4	      = REG_ESP 	; refers to DR6 if CR4.DE is cleared
  REG_DR5	      = REG_EBP 	; refers to DR7 ^
  REG_DR6	      = REG_ESI
  REG_DR7	      = REG_EDI
  ; MMX/XMM/YMM
  REG_SIMD0	      = REG_EAX
  REG_SIMD1	      = REG_ECX
  REG_SIMD2	      = REG_EDX
  REG_SIMD3	      = REG_EBX
  REG_SIMD4	      = REG_ESP
  REG_SIMD5	      = REG_EBP
  REG_SIMD6	      = REG_ESI
  REG_SIMD7	      = REG_EDI
  ; FPU
  REG_ST0	      = REG_EAX
  REG_ST1	      = REG_ECX
  REG_ST2	      = REG_EDX
  REG_ST3	      = REG_EBX
  REG_ST4	      = REG_ESP
  REG_ST5	      = REG_EBP
  REG_ST6	      = REG_ESI
  REG_ST7	      = REG_EDI
  ; Sregs
  SEG_ES	      = REG_EAX
  SEG_CS	      = REG_ECX
  SEG_SS	      = REG_EDX
  SEG_DS	      = REG_EBX
  SEG_FS	      = REG_ESP
  SEG_GS	      = REG_EBP

  ; modr/m r/m

  RM_SIB	      = REG_ESP
  RM_DISP16	      = REG_ESI
  RM_DISP32	      = REG_EBP

  ; sib scale

  SCALE_1	      = 0
  SCALE_2	      = 1
  SCALE_4	      = 2
  SCALE_8	      = 3

  ; prefixes

  PREFIX_SEGMENT_CS   = 02Eh
  PREFIX_SEGMENT_SS   = 036h
  PREFIX_SEGMENT_DS   = 03Eh
  PREFIX_SEGMENT_ES   = 026h
  PREFIX_SEGMENT_FS   = 064h
  PREFIX_SEGMENT_GS   = 065h
  PREFIX_LOCK	      = 0F0h
  PREFIX_REPNZ	      = 0F2h
  PREFIX_REP	      = 0F3h
  PREFIX_OPERAND_SIZE = 066h
  PREFIX_ADDRESS_SIZE = 067h
  PREFIX_VEX_2_BYTE   = 0C5h
  PREFIX_VEX_3_BYTE   = 0C4h
