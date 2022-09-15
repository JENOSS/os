[ORG 0x00]          ; 코드의 시작 어드레스를 0x00으로 설정
[BITS 16]           ; 이하의 코드는 16비트 코드로 설정

SECTION .text       ; text 섹션(세그먼트)을 정의
TOTALSECTORCOUNT:   dw  1
jmp 0x07E0:START    ; CS 세그먼트 레지스터에 0x07E0을 복사하면서, START 레이블로 이동


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
    mov ax, cs  ; 부트 로더의 시작 어드레스(0x7E00)를 세그먼트 레지스터 값으로 변환
    mov ds, ax      ; DS 세그먼트 레지스터에 설정
    mov ax, 0xB800  ; 비디오 메모리의 시작 어드레스(0xB800)를 세그먼트 레지스터 값으로 변환
    mov es, ax      ; ES 세그먼트 레지스터에 설정


CAL:
	pop si
	pop bx
	pop dx
	mov cx, 1900
	mov di, dx
	add di, -1900
	
CALDAYOFWEEK:
	cmp cx, dx	
	je CALMONTH  
	mov ax, cx 
	push di
	push dx
	mov di, 400	
	mov dx, 0
	div di
	cmp dx, 0
	pop dx
	pop di
	jne DIVBYFOUR
	add di, 1	
	add cx, 1 
	jmp CALDAYOFWEEK

DIVBYFOUR:
	mov ax, cx
	push di
	push dx
	mov di, 4	
	mov dx, 0
	div di
	cmp dx, 0
	pop dx
	pop di
	jne NOLEAPYEAR 	
	mov ax, cx
	push di
	push dx		
	mov di, 100	
	mov dx, 0
	div di
	cmp dx, 0
	pop dx	
	pop di
	je NOLEAPYEAR	
	add di, 1
	add cx, 1
	jmp CALDAYOFWEEK

NOLEAPYEAR:
	add cx, 1
	jmp CALDAYOFWEEK
	
CALMONTH:
	push bx		
	cmp bx,1 
	je CALDATE
	cmp bx,2
	je FEB
	cmp bx,3
	je MAR
	cmp bx,4
	je APR
	cmp bx,5
	je MAY
	cmp bx,6
	je JUN
	cmp bx,7
	je JUL
	cmp bx,8
	je AUG
	cmp bx,9
	je SEP
	cmp bx,10
	je OCT
	cmp bx,11
	je NOV
	jmp DEC
FEB:
	mov bx,31
	jmp CALDATE
MAR:
	mov bx,59
	jmp CALDATE
APR:
	mov bx,90
	jmp CALDATE
MAY:
	mov bx,120
	jmp CALDATE
JUN:
	mov bx,151
	jmp CALDATE
JUL:
	mov bx,181
	jmp CALDATE
AUG:
	mov bx,212
	jmp CALDATE
SEP:
	mov bx,243
	jmp CALDATE
OCT:
	mov bx,273
	jmp CALDATE
NOV:
	mov bx,304
	jmp CALDATE
DEC:
	mov bx,334

CALDATE:  
	pop ax		
	cmp ax, 3 
	jl NOLEAPYEAR2
	mov ax, dx ;current year
	push si
	push dx
	mov si, 400	
	mov dx, 0
	div si
	cmp dx, 0
	pop dx
	pop si
	jne DIVBYFOUR2
	add bx, 1 
	jmp NOLEAPYEAR2
DIVBYFOUR2:
	mov ax, dx
	push si
	push dx
	mov si, 4	
	mov dx, 0
	div si
	cmp dx, 0
	pop dx
	pop si
	jne NOLEAPYEAR2
	mov ax, dx
	push si
	push dx
	mov si, 100	
	mov dx, 0
	div si
	cmp dx, 0
	pop dx
	pop si
	je NOLEAPYEAR2
	add bx, 1

NOLEAPYEAR2:

	mov cx,di
	add bx, si
	add bx, -1
	mov di, 7
	mov dx, 0
	mov ax, bx
	div di
	add cx, dx
	mov ax, cx
	mov dx, 0
	div di

	mov dh ,0 ;; dl = value
	push dx ;; final value 


 
RESETDISK:                          ; 디스크를 리셋하는 코드의 시작
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; BIOS Reset Function 호출
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 서비스 번호 0, 드라이브 번호(0=Floppy)

    mov ax, 0
    mov dl, 0x00            
    int 0x13     
    ; 에러가 발생하면 디스크를 다시 리셋
    jc  RESETDISK 
        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 디스크에서 섹터를 읽음
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 디스크의 내용을 메모리로 복사할 어드레스(ES:BX)를 0x08000으로 설정
    mov si, 0x0800                  ; OS 이미지를 복사할 어드레스(0x08000)를 
                                    ; 세그먼트 레지스터 값으로 변환
    mov es, si                      ; ES 세그먼트 레지스터에 값 설정
    mov bx, 0x0000                  ; BX 레지스터에 0x0000을 설정하여 복사할 
                                    ; 어드레스를 0x0800:0000(0x08000)으로 최종 설정

    mov di, word [ TOTALSECTORCOUNT ] ; 복사할 OS 이미지의 섹터 수를 DI 레지스터에 설정

READDATA:                           ; 디스크를 읽는 코드의 시작

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; BIOS Read Function 호출
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov ah, 0x02                        ; BIOS 서비스 번호 2(Read Sector)
    mov al, 0x01                       ; 읽을 섹터 수는 1
    mov ch, 0x00        ; 읽을 트랙 번호 설정
    mov cl, 0x03       ; 읽을 섹터 번호 설정
    mov dh, 0x00         ; 읽을 헤드 번호 설정
    mov dl, 0x00                        ; 읽을 드라이브 번호(0=Floppy) 설정
    int 0x13                            ; 인터럽트 서비스 수행
    jc HANDLEDISKERROR                ; 에러가 발생했다면 HANDLEDISKERROR로 이동

	
READEND:   
	jmp 0x0800:0x0000

HANDLEDISKERROR:
	call RESETDISK




times 512 - ( $ - $$ )    db    0x00    



