datarea segment
        grade   dw      88,75,95,63,98,78,87,73,90,60      
        rank    dw      35 dup(?)
        ten     db      10
        three   db      3
        space   db      32
datarea ends

prognam segment

main    proc    far
        assume cs:prognam,ds:datarea
start:
        push    ds
        sub     ax,ax
        push    ax

        mov     ax,datarea
        mov     ds,ax
        lea     bx,rank

        mov     bx, 0800h
        mov     es,bx

        mov     di,10
        mov     bx,0
loop1:
        mov     ax,grade[bx]
        mov     dx,0
        mov     cx,10
        lea     si,grade
next:
        cmp     ax,[si]
        jg      no_count
        inc     dx
no_count:
        add     si,2
        loop    next
        mov     rank[bx], dx
        add     bx,2
        dec     di
        jne     loop1
        ;
        jmp     pnt
pnt:
        lea     dx,rank
        mov     ah,09h
        int     21h


main    endp

prognam ends

        end     start