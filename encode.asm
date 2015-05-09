use32

	include 'common.inc'

  ; encoding routine

encode:
    virtual at esi
	.in	fde32s
    end virtual
	pushad

	; prepare stuff
	mov	edi,ecx
	mov	esi,edx
	push	ecx

	; write prefixes
	mov	cl,[.in.prefix]
	test	cl,PRE_SEG
	je	.no_seg
	mov	al,[.in.prefix.seg]
	stosb
    .no_seg:
	test	cl,PRE_67
	je	.no_67
	mov	al,PREFIX_ADDRESS_SIZE
	stosb
    .no_67:
	test	cl,PRE_VEX
	jnz	.wr_vex
	test	cl,PRE_LOCK
	je	.no_lock
	mov	al,PREFIX_LOCK
	stosb
    .no_lock:
	test	cl,PRE_REP
	je	.no_rep
	mov	al,[.in.prefix.rep]
	stosb
    .no_rep:
	test	cl,PRE_66
	je	.no_66
	mov	al,PREFIX_OPERAND_SIZE
	stosb
    .no_66:

	; write vex prefix
	test	cl,PRE_VEX
	je	.no_vex
    .wr_vex:
	mov	al,[.in.vex]
	stosb
	mov	ah,al
	cmp	[.in.vex.r],-1
	jnz	.re_vex_2byte
	mov	al,[.in.vex2]
	stosb
	cmp	ah,PREFIX_VEX_3_BYTE
	jnz	.no_vex
	mov	al,[.in.vex3]
	stosb
	jmp	.no_vex
    .re_vex_2byte:
	cmp	ah,PREFIX_VEX_2_BYTE
	jnz	.re_vex_3byte
	mov	al,[.in.vex.r]
	mov	ah,[.in.vex.vvvv]
	mov	cl,[.in.vex.l]
	mov	ch,[.in.vex.pp]
	not	al
	not	ah
	shl	al,7
	shl	ah,3
	shl	cl,2
	or	al,ah
	or	cl,ch
	or	al,cl
	stosb
	mov	al,[.in.opcode2]
	jmp	.wr_opcode
    .re_vex_3byte:
	mov	ax,word [.in.vex.r]
	mov	cx,word [.in.vex.b]
	not	ax
	not	cl
	shl	al,7
	shl	ah,6
	shl	cl,5
	or	al,ah
	or	cl,ch
	or	al,cl
	stosb
	mov	ax,word [.in.vex.w]
	mov	cx,word [.in.vex.l]
	not	ah
	shl	al,7
	shl	ah,3
	shl	cl,2
	or	al,ah
	or	cl,ch
	or	al,cl
	stosb
	mov	ah,[.in.vex.m_mmmm]
	mov	al,[.in.opcode2]
	cmp	ah,M_MMMM_0F
	je	.wr_opcode
	mov	al,[.in.opcode3]
	jmp	.wr_opcode
    .no_vex:

	; write opcodes
	mov	al,[.in.opcode]
    .wr_opcode:
	stosb
	test	[.in.prefix],PRE_VEX
	jnz	.opcode_ok
	cmp	al,0Fh
	jnz	.opcode_ok
	mov	al,[.in.opcode2]
	stosb
	cmp	al,38h
	je	.3_byte
	cmp	al,3Ah
	jnz	.opcode_ok
    .3_byte:
	mov	al,[.in.opcode3]
	stosb
    .opcode_ok:

	; write modr/m-byte
	mov	ecx,[.in.flags]
	test	ecx,F_MODRM
	je	.no_modrm
	cmp	[.in.modrm.mod],-1
	jnz	.re_modrm
	mov	al,[.in.modrm]
	stosb
	jmp	.no_modrm
    .re_modrm:
	mov	ax,word [.in.modrm.mod]
	mov	dl,[.in.modrm.rm]
	shl	al,6
	shl	ah,3
	or	al,ah
	or	al,dl
	stosb
    .no_modrm:

	; write sib-byte
	test	ecx,F_SIB
	je	.no_sib
	cmp	[.in.sib.scale],-1
	jnz	.re_sib
	mov	al,[.in.sib]
	stosb
	jmp	.no_sib
    .re_sib:
	mov	ax,word [.in.sib.scale]
	mov	dl,[.in.sib.base]
	shl	al,6
	shl	ah,3
	or	al,ah
	or	al,dl
	stosb
    .no_sib:

	; write displacement
	test	ecx,F_DISP32
	je	.no_disp32
	mov	eax,[.in.disp32]
	stosd
	jmp	.no_disp8
    .no_disp32:
	test	ecx,F_DISP16
	je	.no_disp16
	mov	ax,[.in.disp16]
	stosw
	jmp	.no_disp8
    .no_disp16:
	test	ecx,F_DISP8
	je	.no_disp8
	mov	al,[.in.disp8]
	stosb
    .no_disp8:

	; write immediate-bytes
	test	ecx,F_IMM32
	je	.no_imm32
	mov	eax,[.in.imm32]
	stosd
    .no_imm32:
	test	ecx,F_IMM16
	je	.no_imm16
	mov	ax,[.in.imm16]
	stosw
    .no_imm16:
	test	ecx,F_IMM8
	je	.no_imm8
	mov	al,[.in.imm8]
	stosb
    .no_imm8:

	; finish stuff
	pop	eax
	xchg	eax,edi
	sub	eax,edi
	mov	[.in.len],al

	mov	[esp+1Ch],eax
	popad
	retn
