format PE GUI 4.0 at 400000h
entry start

include 'win32a.inc'

section '.code' code readable executable

  start:
	mov	edx,cmd
	mov	ecx,decode
    .loop:
	call	decode
	movzx	eax,[cmd.len]
	add	ecx,eax
	cmp	[cmd.opcode],0C3h
	jnz	.loop

	sub	ecx,decode
	push	ecx
	push	_fmt
	push	buf
	call	[wsprintf]
	add	esp,0Ch

	push	MB_OK
	push	_title
	push	buf
	push	0
	call	[MessageBox]

	push	0
	call	[ExitProcess]

	include 'fde32.inc'

section '.data' data readable writeable

  _title db 'fde32 demo',0
  _fmt db 'decode''s length is %d bytes (without tables).',0

  buf rb 256
  cmd fde32s

section '.idata' import data readable

  library kernel32,'KERNEL32.DLL',\
	  user32,'USER32.DLL'

  include 'api\kernel32.inc'
  include 'api\user32.inc'

section '.reloc' data readable discardable fixups
