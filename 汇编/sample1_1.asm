datarea segment
    string1     db  20 dup(?)
    string2     db  20 dup(?)
    ;
    mess1   db  'Match.',13,10,'$'
    mess2   db  'No match.',13,10,'$'
    mess3   db  'input key:',13,10,'$'
    mess4   db  'input sentence:',13,10,'$'

datarea ends

prognam segment
main    proc    far

start:
    push    ds
    sub     ax,ax
    push    ax

    mov     ax, datarea
    mov     ds, ax
    mov     es, ax

    lea dx, mess3
    mov ah,09h
    int 21h
    lea si, string1
    mov ah,0ah
    int 21h

    mov dx,0
    ; sub dx,dx

    lea dx, mess4
    mov ah,09h
    int 21h
    lea di, string2
    mov ah, 0ah
    int 21h
    
    cld

    mov cx,25
    repz    cmpsb
    jz  match
    lea dx,mess2
    jmp short disp

match:
    lea dx,mess1
disp:
    mov ah,09
    int 21h
    ret
main endp
prognam ends
    end     start   
