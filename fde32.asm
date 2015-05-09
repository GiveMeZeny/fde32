format coff

public encode
public encode as '_encode'

encode:
	mov	edx,[esp+8]
	mov	ecx,[esp+4]

  file 'bin\encode.bin'

public decode
public decode as '_decode'

decode:
	mov	edx,[esp+8]
	mov	ecx,[esp+4]

  file 'bin\decode.bin'
