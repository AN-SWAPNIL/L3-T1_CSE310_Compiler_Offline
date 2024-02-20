;..........START..........

.MODEL SMALL
.STACK 1000H

.DATA
	number DB "00000$"

.CODE
;..........MAIN..........

main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
	SUB SP, 2
L6:
	MOV CX, [BP-2]		; Line 3
	PUSH CX
	MOV CX, 0		; Line 3
	POP AX
	CMP AX, CX
	JG L8
	JMP L9
L9:
	MOV CX, [BP-2]		; Line 3
	PUSH CX
	MOV CX, 10		; Line 3
	POP AX
	CMP AX, CX
	JL L8
	JMP L7
L8:
	MOV CX, 100		; Line 4
	MOV [BP-2], CX
	JMP L5
L7:
	MOV CX, 200		; Line 6
	MOV [BP-2], CX
	ADD SP, 0
L5:
	MOV CX, [BP-2]		; Line 8
	PUSH CX
	MOV CX, 20		; Line 8
	POP AX
	CMP AX, CX
	JG L12
	JMP L10
L12:
	MOV CX, [BP-2]		; Line 8
	PUSH CX
	MOV CX, 30		; Line 8
	POP AX
	CMP AX, CX
	JL L11
	JMP L10
L11:
	MOV CX, 300		; Line 9
	MOV [BP-2], CX
	JMP L4
L10:
	MOV CX, 400		; Line 11
	MOV [BP-2], CX
	ADD SP, 0
L4:
	MOV CX, [BP-2]		; Line 13
	PUSH CX
	MOV CX, 40		; Line 13
	POP AX
	CMP AX, CX
	JG L18
	JMP L17
L18:
	MOV CX, 1
	JMP L19
L17:
	MOV CX, 0
L19:
	PUSH CX
	MOV CX, [BP-2]		; Line 13
	PUSH CX
	MOV CX, 50		; Line 13
	POP AX
	CMP AX, CX
	JL L21
	JMP L20
L21:
	MOV CX, 1
	JMP L22
L20:
	MOV CX, 0
L22:
	POP AX
	CMP AX, 0
	JE L23
	CMP CX, 0
	JE L23
	JMP L24
L23:
	MOV CX, 0
	JMP L25
L24:
	MOV CX, 1
L25:
		; Line 13
	CMP CX, 0
	JE L15
	JMP L14
L15:
	MOV CX, [BP-2]		; Line 13
	PUSH CX
	MOV CX, 60		; Line 13
	POP AX
	CMP AX, CX
	JL L28
	JMP L27
L28:
	MOV CX, 1
	JMP L29
L27:
	MOV CX, 0
L29:
	PUSH CX
	MOV CX, [BP-2]		; Line 13
	PUSH CX
	MOV CX, 70		; Line 13
	POP AX
	CMP AX, CX
	JG L31
	JMP L30
L31:
	MOV CX, 1
	JMP L32
L30:
	MOV CX, 0
L32:
	POP AX
	CMP AX, 0
	JE L33
	CMP CX, 0
	JE L33
	JMP L34
L33:
	MOV CX, 0
	JMP L35
L34:
	MOV CX, 1
L35:
		; Line 13
	CMP CX, 0
	JE L13
	JMP L14
L14:
	MOV CX, 500		; Line 14
	MOV [BP-2], CX
	JMP L3
L13:
	MOV CX, 600		; Line 16
	MOV [BP-2], CX
	ADD SP, 0
L3:
	MOV AX, [BP-2]
	CALL print_output
	CALL new_line
L2:
	MOV CX, 0		; Line 19
	JMP main_L
L1:
main_L:
	MOV AX, 4CH
	INT 21H
	ADD SP, 2
	POP BP
main ENDP

;..........LIBRARY_FUNCTIONS..........

;..........new_line..........

new_line proc
	push ax
	push dx
	mov ah,2
	mov dl,0Dh
	int 21h
	mov ah,2
	mov dl,0Ah
	int 21h
	pop dx
	pop ax
	ret
	new_line endp
;..........print_output..........

print_output proc  ;print what is in ax
	push ax
	push bx
	push cx
	push dx
	push si
	lea si,number
	mov bx,10
	add si,4
	cmp ax,0
	jnge negate
	print:
	xor dx,dx
	div bx
	mov [si],dl
	add [si],'0'
	dec si
	cmp ax,0
	jne print
	inc si
	lea dx,si
	mov ah,9
	int 21h
	pop si
	pop dx
	pop cx
	pop bx
	pop ax
	ret
	negate:
	push ax
	mov ah,2
	mov dl,'-'
	int 21h
	pop ax
	neg ax
	jmp print
	print_output endp
END main
;..........END..........

