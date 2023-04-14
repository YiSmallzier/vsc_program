DATAS SEGMENT
    mess1	db	'Enter Keyword:','$'
    mess2	db	'Enter Sentence:','$'
    mess3	db	'Match at location:','$'
    mess4   db  'No match!',13,10,'$'
    mess5	db	'H of the sentence',13,10,'$'
    change	db	13,10,'$';;\n
    ;
    stoknin1	label	byte
    max1	db	10
    act1	db	?
    stokn1	db	10	dup(?)
    ;
    stoknin2	label	byte
    max2	db	50
    act2	db	?
    stokn2	db	50	dup(?)
DATAS ENDS

CODES SEGMENT
    ASSUME CS:CODES,DS:DATAS
    ;
START:
    MOV 	AX,DATAS
    MOV		DS,AX
        ;
   	lea		dx,mess1
   	mov		ah,09
   	int		21h ;; "Enter Keyword:"
   	
   	lea		dx,stoknin1
   	mov		ah,0ah
   	int		21h 
	;; Press in Keyword
   	cmp		act1,0
   	je		nec  ;; check value: ZF
   	;; If Keyword = None, exit
a10:
	lea		dx,change
	mov		ah,09
	int		21h
	lea		dx,mess2
	mov		ah,09
	int		21h ; "\nEnter Sentence:"
	
   	lea		dx,stoknin2
   	mov		ah,0ah
   	int		21h 
	;; Press in Sentence
	cmp		act2,0
	je		nec

	mov		al,act1
	cbw
	mov		cx,ax
    push    cx
	mov		al,act2
	cmp		al,0
	je		nomat 
	sub		al,act1
	js		nomat
	inc		al 
	;LEN = act2 - act1 + 1
	cbw
	lea		bx,stokn2 ;
	mov		di,0
	mov		si,0
a20:    ;outside
	mov		ah,[bx+di]
	cmp		ah,stokn1[si] ;
	jne		a30
	inc		di
	inc		si
	dec		cx 
	;;
	cmp		cx,0
	je		a40
	jmp		a20
a30:    ;inside
	inc		bx
	dec		al
	cmp		al,0
	je		nomat; end of sentence
	mov		di,0
	mov		si,0
	pop		cx
	push	cx
	jmp		a20
nec:
	je		exit
a40:
	sub		bx,offset stokn2
	inc		bx
	lea		dx,change
	mov		ah,09
	int		21h ;
	lea		dx,mess3
	mov		ah,09
	int		21h ;
	call	btoh
	lea		dx,mess5
	mov		ah,09
	int		21h
	jmp		a10
btoh	proc	near
	mov		ch,4	;count
	rotate:	
	mov cl,4
	ROL		bx,cl
	mov		al,bl
	and		al,0fh
	add		al,30h
	cmp		al,3ah	;0~9
	jl		printit
	add		al,7h	;A~F
	printit:
	mov		dl,al
	mov		ah,2
	int		21h
	dec		ch
	jnz		rotate
	ret
btoh	endp
nomat: 
	;no match!g
	lea		dx,change
	mov		ah,09
	int		21h
	lea		dx,mess4 

	mov		ah,09
	int		21h

	jmp		a10
exit:  
	mov ax,4c00H
	int 21H

CODES ENDS
    END START


