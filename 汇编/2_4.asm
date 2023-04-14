data SEGMENT
    mess1   DB  "Input name:",'$'
    mess2   DB  "Input a telephone number:",'$'
    mess3   DB  "Do you want a telephone number?(Y/N)",'$'
    mess4   DB  0ah,0dh,"name?",'$'
    mess5   DB  "not found",0dh,0ah,'$'
    mess6   DB  "number you want to store:",'$'
    mess7   DB  "end?(Y/N)"
    crlf    DB  0ah,0dh,'$'
    show    DB  'name               tel.',0dh,0ah,'$' 
    ;--- name
    stokin1 label   byte
    max1     DB  21
    act1     DB  ?
    stokn1   DB  21 dup(?)
    ;--- telephone
    stokin2 label   word
    max2     DB  9
    act2     DB  ?
    stokn2   DB  9 dup(?)
    ;---
    numtable    DB  50 dup(28 dup(?))
    name_count  dw  0
    ;--
    endaddr dw  ?
    swapped dw  ?
    totalnum    dw  ?
    savenp  DB  28 dup(?),0dh,0ah,'$'
    searchaddr  dw  ?
    flag    DB  ?
    flagb   DB  ?
    
data ENDS

code SEGMENT
    ASSUME ds:data, cs:code, es:data
main    proc    far
    MOV ax, data
    MOV ds, ax
    MOV es, ax
    LEA di, numtable
    ;LEA dx, mess6
    ;MOV AH, 09
    ;int 21h
    MOV bx, 0
newchar:
    ;MOV ah, 1
    ;int 21h
    ;sub al,30h
    ;jl  next
    ;cmp al,9
    ;jg  newchar
    ;cbw

    ; xchg    ax,bx
    ; MOV cx,10
    ; mul cx
    ; xchg    ax,bx
    ; add bx,ax
    ; jmp newchar
next:
    ;MOV totalnum,bx
    ; LEA dx,crlf
    ; MOV ah,09
    ; int 21h
a10:
    LEA dx,mess1
    MOV ah,09
    int 21h
    call    input_name
    ; inc name_count
    call    stor_name
    
    LEA dx,mess2
    MOV ah,09
    int 21h
    call    inphone
    call    stor_phone

    LEA dx,mess7
    mov ah,09
    int 21h
    mov ah,08
    int 21h
    cmp al,'y'
    jz  a10
    cmp al,'n'
    jz  a20
    
    ; cmp name_count,0
    ; je  exit
    ; MOV bx,totalnum
    ; cmp name_count,bx
    ; jnz a10
    ; call    name_sort
a20:
    LEA dx,mess3
    MOV ah,09
    int 21h
    MOV ah,08
    int 21h
    cmp al,'y'
    jz  a30
    cmp al,'n'
    jz  exit
    jmp a20
a30:
    MOV ah,09
    LEA dx,mess4
    int 21h
    call    input_name
a40:
    call    name_search
    jmp a20
exit:
    MOV ax,4c00h
    int 21h
main    endp

;---
input_name  proc    near
    MOV ah,0ah
    LEA dx,stokin1
    int 21h
    MOV ah,09
    LEA dx,crlf
    int 21h
    sub bh,bh
    MOV bl,act1
    MOV cx,21
    sub cx,bx
b10:
    MOV stokn1[bx],' '
    inc bx
    loop    b10
    ret
input_name  endp

;---
stor_name  proc    near
    LEA si,stokn1
    MOV cx,20
    rep movsb
    ret
stor_name  endp

;---
inphone     proc    near
    mov ah,0ah
    lea dx,stokin2
    int 21h
    mov ah,09
    lea dx,crlf
    int 21h
    sub bh,bh
    mov bl,act2
    mov cx,9
    sub cx,bx
c10:
    mov stokn2[bx],' '
    inc bx
    loop c10
    ret 
inphone     endp
stor_phone  proc near
    lea  si,stokn2
    mov  cx,8
    rep  movsb   ;movs 串传送指令 cmps 串比较操作
    ret
stor_phone endp
;---
input_search    proc    near


input_search    endp

;---
name_search     proc    near
    lea  bx,numtable
    mov  flag,0      ;;;立个flag
    ;mov  flagb,0  
d: 
    ;mov  flag,0      ;;;立个flag
    ;mov  flagb,0  
    mov  cx,20
    lea si,stokn1
    mov  di,bx
    repz cmpsb
    jz  d2
    ;jb  dexit
    add bx,28            ;下一条
    cmp  bx,endaddr   
    jbe  d             ;未结束，继续找
    ;add flagb,flag
    sub flag,0  ;要是没有找到的话
    jz  nof
    jmp  dexit         ;结束，退出
nof:  
    lea dx,mess5
    mov ah,09
    int 21h 
d2:
    mov searchaddr,bx
    ;mov flag,1         ;标志找到了
    inc flag
    call printline
    ; dec flag
    ;sub flag,0  ;要是没结束的话
    add bx,28            ;下一条
    cmp  bx,endaddr   
    jbe  d             ;未结束，继续找
    jmp  dexit         ;结束，退出
    jnz  d
dexit:
    ret
name_search     endp

;---
name_sort   proc    near
    sub  di,28
    mov  endaddr,di
 c1:
    mov  swapped,0
    lea  si,numtable
 c2:
    mov  cx,20
    mov  di,si
    add  di,28
    mov  ax,di
    mov  bx,si   ;cmpsb si-di  movsb di<-si
    repz cmpsb   ;repz 当为0时重复串操作；repe 当相等时重复串操作；rep 重复串操作直至计数寄存器count reg 的内容为0为止
    jbe  c3     ;CF与ZF=1，小于等于则转移
 ;chang order 
    mov si,bx
    lea di,savenp
    mov cx,28
    rep movsb
    mov cx,28
    mov di,bx
    rep movsb
    mov cx,28
    lea si,savenp
    rep movsb
    mov swapped,1
 c3:
    mov  si,ax
    cmp  si,endaddr
    jb   c2        ;CF=1，则转移，小于等于则转移
    cmp  swapped,0
    jnz  c1    ;结果不为0则转移
    ret
name_sort   endp

;---
printline   proc    near
    sub flag,0  ;要是没有找到的话
    jz  no
p10:
    mov ah,09
    lea dx,show
    int 21h
    mov cx,28
    mov si,searchaddr
    lea di,savenp
    rep movsb
    lea dx,savenp
    mov ah,09
    int 21h
    jmp fexit
no:    lea dx,mess5
    mov ah,09
    int 21h 
fexit:  
    ret
printline   endp

code ENDS

end main

