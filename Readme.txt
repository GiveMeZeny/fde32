  un[f]ancy [d]isassembler [e]ngine (fde32) v0.01

  fde32 is an extended length disassembler engine
  it's written in x86 asm for use with x86 instructions
  and supports the following instruction sets:
    * general-purpose instructions
    * FPU, MMX, 3DNow!
    * SSE-SSE4.2, AVX
    * VMX, SMX


how to compile:

  use fasm (http://flatassembler.net/) to compile encode.asm and decode.asm,
  place the bin-files into bin/ and compile fde32.asm to a x86 coff obj-file.
  you can also compile and exec packtbl.c and redirect its output into a new
  decode.asm to get one that is a little smaller.


how to use:

    int encode(void *dest, fde32s *cmd);
    int decode(void *src,  fde32s *cmd);

    mov     edx,fde32s_struct
    mov     ecx,[destination]
    call    encode

    mov     edx,fde32s_struct
    mov     ecx,[source]
    call    decode

  encode returns the number of bytes written to the buffer
  decode returns either 1 if the instruction is valid or 0 if any error-flag is set
  both functions preserve all registers but eax


fde32s-struct explanation:

  len
    contains the instruction's size
  prefix
    specifies the prefix flags (PRE_LOCK, PRE_REP, PRE_SEG, PRE_66, PRE_67, PRE_VEX)
  prefix.lock
    equal to PREFIX_LOCK if PRE_LOCK is set, zero elsewise
  prefix.rep
    equal to PREFIX_REPNZ/PREFIX_REP if PRE_REP is set, zero elsewise
  prefix.seg
    equal to PREFIX_SEGMENT_XX if PRE_SEG is set, zero elsewise
  prefix.66
    equal to PREFIX_OPERAND_SIZE if PRE_66 is set, zero elsewise
  prefix.67
    equal to PREFIX_ADDRESS_SIZE if PRE_67 is set, zero elsewise
  vex
    vex escape-byte (C4h/C5h)
  vex2
    full second vex-byte
  vex3
    full third vex-byte
  vex.r
    extracted r-component of vex-byte
  vex.x
    extracted x-component of vex-byte
  vex.b
    extracted b-component of vex-byte
  vex.m_mmmm
    extracted m_mmmm-component of vex-byte
  vex.w
    extracted w-component of vex-byte
  vex.vvvv
    extracted vvvv-component of vex-byte
  vex.l
    extracted l-component of vex-byte
  vex.pp
    extracted pp-component of vex-byte
  opcode.len
    amount of opcode-bytes (1-3, necessary prefixes for various SSE instruction are not counted in)
  opcode
    first opcode
  opcode2
    second opcode
  opcode3
    third opcode
  modrm
    full modr/m-byte
  modrm.mod
    extracted mod-component of modr/m-byte
  modrm.reg
    extracted reg-component of modr/m-byte
  modrm.rm
    extracted rm-component of modr/m-byte
  sib
    full sib-byte
  sib.scale
    extracted scale-component of sib-byte
  sib.index
    extracted index-component of sib-byte
  sib.base
    extracted base-component of sib-byte
  dispXX
    displacement, check flags for size
  immXX
    immediate, check flags for size
  immXX_2
    second immediate, only relevant to call/jmp ptr16:32 and enter imm16, imm8
  flags
    see below


flags explanation:

  F_MODRM, F_SIB, F_DISP8/16/32, F_IMM8/16/32
    self-explanatory
  F_RELATIVE
    is for instructions with RIP-relative immediates like calls and jmps
  F_GROUP
    means modrm.reg is used as an opcode extension
  F_VEX_BAD_PREFIX
    error-flag: an avx instruction has illegally either a opsize-override-, rep- or lock-prefix
  F_ERROR_LOCK
    error-flag: the disassembled instruction is not allowed to have a lock-prefix
  F_ERROR_LENGTH
    error-flag: the instruction-size limit of 15 bytes is exceeded (the said instruction will cause an exception)
  F_ERROR_OPCODE
    error-flag: the disassembled opcode is undefined (aborts forthwith, so far disassembled prefixes are kept)

  if any of those error-flags is set, the disassembled instruction will #UD


notes:

* encode simply writes one byte after another to the buffer if appropriate flags are set.
  e.g. if fde32s.prefix specifies PRE_66 , 66h gets written to the buffer.
                                  PRE_REP, fde32s.prefix.rep gets written.
                                  PRE_VEX, either the full bytes (fde32s.vex2/.vex3) get written or the vex-bytes get reconstructed (see below).

       if fde32s.flags  specifies F_MODRM, either the modr/m-byte gets reconstructed by using fde32s.modrm.mod, fde32s.modrm.reg and fde32s.modrm.rm
                                           or fde32s.modrm gets written directly to the buffer if fde32s.modrm.mod is equal to -1/FFh.
       (same rule applies to the vex-prefix and the sib-byte according to fde32s.vex.r and fde32s.sib.scale)

* an instruction encoded with a vex-prefix actually only has one opcode, but leading escape opcodes are implied via the m_mmmm-component.
  for convenience, decode will write these to fde32s.opcode (and fde32s.opcode2 if necessary) so the actual instruction-opcode will be written to
  either fde32s.opcode2 or fde32s.opcode3 (so you can e.g. search for specific instructions using only the very opcode without ambiguities).
  encode also expects the actual opcode in either of those depending on the m_mmmm-component.

* F_ERROR_OPCODE: only the three opcode bytes get validated. however, opcode extensions like modrm.reg from F_GROUP-instructions,
  the imm8 following the modrm-byte from 3DNow!-instructions or opcode-prefix combinations are not checked for correctness.
  example: FE C0 gets decoded to PRE_NONE / F_GROUP | F_MODRM (modrm.reg=0 => "inc al")
           FE C8 gets decoded to PRE_NONE / F_GROUP | F_MODRM (modrm.reg=1 => "dec al")
           FE D0 gets decoded to PRE_NONE / F_GROUP | F_MODRM (modrm.reg=2 => #UD),
                 it's actually an erroneous opcode, F_ERROR_OPCODE won't be set, though, as such extensions are not being validated
           66 7D gets decoded to PRE_66 / F_MODRM (=> "hsubpd xmm1, xmm2/m128")
              7D gets decoded to          F_MODRM (=> #UD)
                 actually an erroneous opcode again, but prefixes are not checked for necessity, thus F_ERROR_OPCODE won't be set

* some instructions like enter have two immediates, the first one is contained in fde32s.imm32/fde32s.imm16 depending on the immediate's size,
  the second one in fde32s.imm16_2/fde32s.imm8_2 (there are no instructions with a second immediate bigger than 2 bytes).
  the flags, however, are not seperated means if an instruction had 2 word immediates like call ptr16:16 (66h-prefix) the flags would specify F_IMM16 which represents both immediates.
  encode is therefore not able to put such instructions together properly.










version 0.01

[!] Initial release (ported fde64 to x86 in like 15mins, may contain even more bugs)
