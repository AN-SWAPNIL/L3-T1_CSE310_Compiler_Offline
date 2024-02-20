.MODEL SMALL
.STACK 1000H
.DATA
	number DB "00000$"
	arr DW 5 DUP (0000H)
	a DW 1 DUP (0000H)
.CODE
f PROC
	PUSH BP
	MOV BP, SP
	MOV AX, [BP+8]
	CALL print_output
	CALL new_line
L8:
	MOV AX, [BP+6]
	CALL print_output
	CALL new_line
L7:
	MOV AX, [BP+4]
	CALL print_output
	CALL new_line
L6:
	MOV CX, 441
	PUSH CX
	MOV CX, 1
	LEA SI, arr
	ADD SI, CX
	ADD SI, CX
	PUSH BP
	MOV BP, SI
	POP AX
	POP CX
	MOV [BP], CX
	MOV BP, AX
L5:
	MOV CX, 555
	PUSH CX
	MOV CX, 0
	LEA SI, arr
	ADD SI, CX
	ADD SI, CX
	PUSH BP
	MOV BP, SI
	POP AX
	POP CX
	MOV [BP], CX
	MOV BP, AX
L4:
	MOV CX, 1
	LEA SI, arr
	ADD SI, CX
	ADD SI, CX
	PUSH BP
	MOV BP, SI
	MOV CX, [BP]
	POP BP
	MOV [BP+6], CX
L3:
	MOV AX, [BP+6]
	CALL print_output
	CALL new_line
L2:
	MOV CX, 0
	LEA SI, arr
	ADD SI, CX
	ADD SI, CX
	PUSH BP
	MOV BP, SI
	MOV CX, [BP]
	POP BP
	JMP f_L
L1:
f_L:
	POP BP
	RET
	f ENDP
recursive PROC
	PUSH BP
	MOV BP, SP
	MOV CX, [BP+4]
	PUSH CX
	MOV CX, 1
	POP AX
	CMP AX, CX
	JE L12
	JMP L11
L12:
	MOV CX, 1
	JMP recursive_L
L11:
	MOV CX, [BP+4]
	PUSH CX
	MOV CX, 0
	POP AX
	CMP AX, CX
	JE L13
	JMP L10
L13:
	MOV CX, 0
	JMP recursive_L
L10:
	MOV CX, [BP+4]
	PUSH CX
	MOV CX, 1
	POP AX
	SUB CX, AX
	NEG CX
	PUSH CX
	CALL recursive
	ADD SP, 2
	PUSH CX
	MOV CX, [BP+4]
	PUSH CX
	MOV CX, 2
	POP AX
	SUB CX, AX
	NEG CX
	PUSH CX
	CALL recursive
	ADD SP, 2
	POP AX
	ADD CX, AX
	JMP recursive_L
L9:
recursive_L:
	POP BP
	RET
	recursive ENDP
v PROC
	PUSH BP
	MOV BP, SP
	MOV CX, 3
	MOV a, CX
L16:
	MOV CX, a
	CMP CX, 0
	JE L15
	JMP L17
L17:
	SUB SP, 2
L20:
	MOV CX, 1
	MOV [BP-2], CX
L19:
	MOV AX, [BP-2]
	CALL print_output
	CALL new_line
L18:
	ADD SP, 2
L15:
	MOV AX, a
	CALL print_output
	CALL new_line
L14:
v_L:
	POP BP
	RET
	v ENDP
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 2
	SUB SP, 10
L70:
	MOV CX, 5
	MOV [BP-8], CX
L69:
	CALL v
L68:
	MOV AX, [BP-8]
	CALL print_output
	CALL new_line
L67:
	MOV CX, 0
	MOV [BP-2], CX
L71:
	MOV CX, [BP-2]
	PUSH CX
	MOV CX, 5
	POP AX
	CMP AX, CX
	JL L72
	JMP L66
L72:
	MOV CX, [BP-2]
	PUSH CX
	MOV CX, 1
	POP AX
	ADD CX, AX
	PUSH CX
	MOV CX, [BP-2]
	PUSH BP
	MOV BX, CX
	ADD BX, BX
	ADD BX, -18
	ADD BP, BX
	POP AX
	POP CX
	MOV [BP], CX
	MOV BP, AX
L74:
	MOV CX, [BP-2]
	MOV AX, CX
	INC CX
	MOV [BP-2], CX
	MOV CX, AX
	JMP L71
L66:
	MOV CX, 4
	MOV [BP-2], CX
L65:
L75:
	MOV CX, [BP-2]
	MOV AX, CX
	DEC CX
	MOV [BP-2], CX
	MOV CX, AX
	CMP CX, 0
	JE L64
	JMP L76
L76:
	MOV CX, [BP-2]
	PUSH BP
	MOV BX, CX
	ADD BX, BX
	ADD BX, -18
	ADD BP, BX
	MOV CX, [BP]
	POP BP
	MOV [BP-4], CX
L78:
	MOV AX, [BP-4]
	CALL print_output
	CALL new_line
L77:
	JMP L75
L64:
	MOV CX, 2
	MOV [BP-6], CX
L63:
	MOV CX, [BP-6]
	PUSH CX
	MOV CX, 0
	POP AX
	CMP AX, CX
	JG L80
	JMP L79
L80:
	MOV CX, [BP-6]
	MOV AX, CX
	INC CX
	MOV [BP-6], CX
	MOV CX, AX
	JMP L62
L79:
	MOV CX, [BP-6]
	MOV AX, CX
	DEC CX
	MOV [BP-6], CX
	MOV CX, AX
L62:
	MOV AX, [BP-6]
	CALL print_output
	CALL new_line
L61:
	MOV CX, 2
	NEG CX
	MOV [BP-6], CX
L60:
	MOV CX, [BP-6]
	PUSH CX
	MOV CX, 0
	POP AX
	CMP AX, CX
	JL L82
	JMP L81
L82:
	MOV CX, [BP-6]
	MOV AX, CX
	INC CX
	MOV [BP-6], CX
	MOV CX, AX
	JMP L59
L81:
	MOV CX, [BP-6]
	MOV AX, CX
	DEC CX
	MOV [BP-6], CX
	MOV CX, AX
L59:
	MOV AX, [BP-6]
	CALL print_output
	CALL new_line
L58:
	MOV CX, 121
	MOV [BP-6], CX
L57:
	MOV CX, [BP-6]
	NEG CX
	MOV [BP-6], CX
L56:
	MOV CX, 5
	MOV [BP-2], CX
L55:
	MOV CX, [BP-2]
	PUSH CX
	MOV CX, [BP-6]
	POP AX
	ADD CX, AX
	MOV [BP-6], CX
L54:
	MOV AX, [BP-6]
	CALL print_output
	CALL new_line
L53:
	MOV CX, 4
	NEG CX
	MOV [BP-6], CX
L52:
	MOV CX, [BP-6]
	PUSH CX
	MOV CX, 4
	POP AX
	IMUL CX
	MOV CX, AX
	MOV [BP-6], CX
L51:
	MOV AX, [BP-6]
	CALL print_output
	CALL new_line
L50:
	MOV CX, 19
	MOV [BP-4], CX
L49:
	MOV CX, 4
	MOV [BP-2], CX
L48:
	MOV CX, [BP-4]
	PUSH CX
	MOV CX, [BP-2]
	POP AX
CWD
	IDIV CX
	MOV CX, AX
	MOV [BP-6], CX
L47:
	MOV AX, [BP-6]
	CALL print_output
	CALL new_line
L46:
	MOV CX, [BP-4]
	PUSH CX
	MOV CX, [BP-2]
	POP AX
CWD
	IDIV CX
	MOV AX, DX
	MOV CX, AX
	MOV [BP-6], CX
L45:
	MOV AX, [BP-6]
	CALL print_output
	CALL new_line
L44:
	MOV CX, 111
	PUSH CX
	MOV CX, 222
	PUSH CX
	MOV CX, 333
	PUSH CX
	CALL f
	ADD SP, 6
	PUSH CX
	MOV CX, 444
	POP AX
	SUB CX, AX
	NEG CX
	MOV [BP-6], CX
L43:
	MOV AX, [BP-6]
	CALL print_output
	CALL new_line
L42:
	MOV CX, 5
	PUSH CX
	CALL recursive
	ADD SP, 2
	MOV [BP-6], CX
L41:
	MOV AX, [BP-6]
	CALL print_output
	CALL new_line
L40:
	MOV CX, 2
	MOV [BP-6], CX
L39:
	MOV CX, 1
	MOV [BP-2], CX
L38:
	MOV CX, [BP-2]
	PUSH CX
	MOV CX, [BP-6]
	POP AX
	CMP AX, 0
	JE L84
	JMP L85
L84:
	CMP CX, 0
	JE L86
L85:
	MOV CX, 1
L86:
	MOV [BP-4], CX
L37:
	MOV AX, [BP-4]
	CALL print_output
	CALL new_line
L36:
	MOV CX, [BP-2]
	PUSH CX
	MOV CX, [BP-6]
	POP AX
	CMP AX, 0
	JE L88
	CMP CX, 0
	JE L88
	JMP L89
L88:
	MOV CX, 0
	JMP L90
L89:
	MOV CX, 1
L90:
	MOV [BP-4], CX
L35:
	MOV AX, [BP-4]
	CALL print_output
	CALL new_line
L34:
	MOV CX, 2
	MOV [BP-6], CX
L33:
	MOV CX, 0
	MOV [BP-2], CX
L32:
	MOV CX, [BP-2]
	PUSH CX
	MOV CX, [BP-6]
	POP AX
	CMP AX, 0
	JE L92
	JMP L93
L92:
	CMP CX, 0
	JE L94
L93:
	MOV CX, 1
L94:
	MOV [BP-4], CX
L31:
	MOV AX, [BP-4]
	CALL print_output
	CALL new_line
L30:
	MOV CX, [BP-2]
	PUSH CX
	MOV CX, [BP-6]
	POP AX
	CMP AX, 0
	JE L96
	CMP CX, 0
	JE L96
	JMP L97
L96:
	MOV CX, 0
	JMP L98
L97:
	MOV CX, 1
L98:
	MOV [BP-4], CX
L29:
	MOV AX, [BP-4]
	CALL print_output
	CALL new_line
L28:
	MOV CX, [BP-6]
	CMP CX, 0
	JNE L99
	MOV CX, 1
	JMP L100
L99:
	MOV CX, 0
L100:
	MOV [BP-4], CX
L27:
	MOV AX, [BP-4]
	CALL print_output
	CALL new_line
L26:
	MOV CX, [BP-4]
	CMP CX, 0
	JNE L101
	MOV CX, 1
	JMP L102
L101:
	MOV CX, 0
L102:
	MOV [BP-4], CX
L25:
	MOV AX, [BP-4]
	CALL print_output
	CALL new_line
L24:
	MOV CX, 12
	PUSH CX
	MOV CX, 2
	PUSH CX
	MOV CX, 89
	POP AX
CWD
	IDIV CX
	MOV CX, AX
	PUSH CX
	MOV CX, 3
	PUSH CX
	MOV CX, 33
	POP AX
	SUB CX, AX
	NEG CX
	PUSH CX
	MOV CX, 64
	PUSH CX
	MOV CX, 2
	POP AX
	IMUL CX
	MOV CX, AX
	POP AX
	ADD CX, AX
	POP AX
CWD
	IDIV CX
	MOV AX, DX
	MOV CX, AX
	POP AX
	ADD CX, AX
	PUSH CX
	MOV CX, 3
	POP AX
	SUB CX, AX
	NEG CX
	PUSH CX
	MOV CX, 3
	PUSH CX
	MOV CX, 59
	PUSH CX
	MOV CX, 9
	POP AX
CWD
	IDIV CX
	MOV CX, AX
	PUSH CX
	MOV CX, 2
	POP AX
	IMUL CX
	MOV CX, AX
	POP AX
	ADD CX, AX
	PUSH CX
	MOV CX, 4
	POP AX
	SUB CX, AX
	NEG CX
	POP AX
	ADD CX, AX
	MOV [BP-4], CX
L23:
	MOV AX, [BP-4]
	CALL print_output
	CALL new_line
L22:
	MOV CX, 0
	JMP main_L
L21:
main_L:
	MOV AX, 4CH
	INT 21H
	ADD SP, 18
	POP BP
	main ENDP
new_line proc
	push ax
	push dx
	mov ah, 2
	mov dl, 0Dh
	int 21h
	mov ah, 2
	mov dl, 0Ah
	int 21h
	pop dx
	pop ax
	ret
	new_line endp
print_output proc
	push ax
	push bx
	push cx
	push dx
	push si
	lea si, number
	mov bx, 10
	add si, 4
	cmp ax, 0
	jnge negate
print:
	xor dx, dx
	div bx
	mov [si], dl
	add [si], '0'
	dec si
	cmp ax, 0
	jne print
	inc si
	lea dx, si
	mov ah, 9
	int 21h
	pop si
	pop dx
	pop cx
	pop bx
	pop ax
	ret
negate:
	push ax
	mov ah, 2
	mov dl, '-'
	int 21h
	pop ax
	neg ax
	jmp print
	print_output endp
END main
