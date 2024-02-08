

	bptr	EQU	<byte ptr>
	wptr	EQU	<word ptr>

	LBUTTON	EQU	07Eh
	RBUTTON	EQU	07Dh




SYSTEM_TEXT	segment	byte public 'CODE'

DGROUP	group	_DATA,_BSS
	assume	cs:SYSTEM_TEXT,ds:DGROUP,ss:DGROUP

SYSTEM_TEXT	ends

_DATA	segment word public 'DATA'


;
;	joystick calibration
;

j0xmin		dw 0FFFFh
j0xmid		dw 0
j0xmax 		dw 0

j0ymin		dw 0FFFFh
j0ymid		dw 0
j0ymax		dw 0

j1xmin		dw 0FFFFh
j1xmid		dw 0
j1xmax		dw 0

j1ymin		dw 0FFFFh
j1ymid		dw 0
j1ymax		dw 0

;
;	depends on speed of machine
;

max		dw 300
alter		dw 0


_DATA	ends




SYSTEM_TEXT	segment	byte public 'CODE'





_DivZero	proc	far
	push	ds			;
	push	bp			; save ds,bp
	mov	bp,DGROUP		;
	mov	ds,bp			;
	mov	bp,sp			;
	inc	bptr _OverflowFlag	; OverflowFlag++;
	add	wptr [bp+4],2		; ip += 2;
	mov	ax,32767		; ax  = MaxInt;
	pop	bp			;
	pop	ds			;
	iret				; restore ds,bp
_DivZero	endp






_EmptyFunction	proc	far		;
	iret				;
_EmptyFunction	endp			;






_CalibrateJoystick	proc	far	;
	push	bp			;
	push	si			;
	push	di			;
	call	Joystick		;
	mov     ax,_latch1		;
	mov	j0xmid,ax		;
	sub	ax,1*5			;
	mov	j0xmin,ax		;
	add	ax,2*5			;
	mov	j0xmax,ax		;
	mov	ax,_latch2		;
	mov	j0ymid,ax		;
	sub	ax,1*5			;
	mov	j0ymin,ax		;
	add	ax,2*5			;
	mov	j0ymax,ax		;
	mov	ax,_latch3		;
	mov	j1xmid,ax		;
	sub	ax,1*5			;
	mov	j1xmin,ax		;
	add	ax,2*5			;
	mov	j1xmax,ax		;
	mov	ax,_latch4		;
	mov	j1ymid,ax		;
	sub	ax,1*5			;
	mov	j1ymin,ax		;
	add	ax,2*5			;
	mov	j1ymax,ax		;
	mov	bptr _JoyCode,0		;
	pop	di			;
	pop	si			;
	pop	bp			;
	ret				;
_CalibrateJoystick	endp








 ReadMouse:
	cmp	bptr DGROUP:_MouseDriverFlag,0
	je	short RM_quit		;

	mov	ax,003h			; Select mouse interrupt
	int	33h			; start mouse interrupt
	add	dx,dx			;
	mov	wptr DGROUP:_SteerX,cx	;
	mov	wptr DGROUP:_SteerY,dx	;
	mov	cx,bx			;
	shl	cx,1			;
	shl	cx,1			;
	and	cx,4			;
	shr	bx,1			;
	and	bx,1			;
	or	bx,cx			;
	mov	wptr DGROUP:_ibutton,bx	; set left button flag
 RM_quit:
	test	bptr key[LBUTTON],1	;
	jz	short RM_m1		;
	or	wptr DGROUP:_ibutton,4	;
 RM_m1:	test	bptr key[RBUTTON],1	;
	jz	short RM_m2		;
	or	wptr DGROUP:_ibutton,1	;
 RM_m2:
	ret				;









 Joystick:
	cli				;
	mov dx,0201h                    ;
	in al,dx                        ;
	and ax,11110000b                ;
	mov cl,4			;
	not al				;
	shr al,cl			;
	mov DGROUP:_latch0,ax           ;

	mov ax,0                        ;
	mov di,ax                       ;
	mov si,ax                       ;
	mov bp,ax                       ;
	mov bx,ax                       ;

	mov al,1111b                    ;
	mov dx,0201h                    ;
	out dx,al                       ;
	mov cx,max                      ;

muldo:	in al,dx                        ;
	shr al,1                        ;
	adc bx,0                        ;
	shr al,1                        ;
	adc bp,0                        ;
	shr al,1                        ;
	adc si,0                        ;
	shr al,1                        ;
	adc di,0                        ;
	loop muldo                      ;

	neg bx                          ;
	add bx,max                      ;
	mov DGROUP:_latch1,bx           ;
	neg bp                          ;
	add bp,max                      ;
	mov DGROUP:_latch2,bp           ;
	neg si                          ;
	add si,max                      ;
	mov DGROUP:_latch3,si           ;
	neg di                          ;
	add di,max                      ;
	mov DGROUP:_latch4,di           ;
	sti				;

	cmp bx,j0xmin			; Joystick 0, X-axis
	ja  short JOY_x01		;
	mov j0xmin,bx			;
JOY_x01:
	cmp bx,j0xmax			;
	jb  short JOY_x02		;
	mov j0xmax,bx			;
JOY_x02:
	mov ax,bp			; Joystick 1, Y-axis
	cmp ax,j0ymin			;
	ja  short JOY_y01		;
	mov j0ymin,ax			;
JOY_y01:
	cmp ax,j0ymax			;
	jb  short JOY_y02		;
	mov j0ymax,ax			;
JOY_y02:
	mov ax,si			; Joystick 2, X-axis
	cmp ax,j1xmin			;
	ja  short JOY_x11		;
	mov j1xmin,ax			;
JOY_x11:
	cmp ax,j1xmax			;
	jb  short JOY_x12		;
	mov j1xmax,ax			;
JOY_x12:
	mov ax,di			; Joystick 2, Y-axis
	cmp ax,j1ymin			;
	ja  short JOY_y11		;
	mov j1ymin,ax			;
JOY_y11:
	cmp ax,j1ymax			;
	jb  short JOY_y12		;
	mov j1ymax,ax			;
JOY_y12:
	xor cl,cl			; cx contains direction flags
	mov ax,j0xmid			;
	add ax,j0xmin			;
	shr ax,1			;
	cmp ax,_latch1			;
	jb  short JOY_l1		;
	or  cx,01h			;
	jmp short JOY_r1		;
JOY_l1:	mov ax,j0xmid			;
	add ax,j0xmax			;
	shr ax,1			;
	cmp ax,_latch1			;
	ja  short JOY_r1		;
	or  cx,02h			;
JOY_r1:
	mov ax,j0ymid			;
	add ax,j0ymin			;
	shr ax,1			;
	cmp ax,_latch2			;
	jb  short JOY_u1		;
	or  cl,04h			;
	jmp short JOY_d1		;
JOY_u1:	mov ax,j0ymid			;
	add ax,j0ymax			;
	shr ax,1			;
	cmp ax,_latch2			;
	ja  short JOY_d1		;
	or  cl,08h			;
JOY_d1:
	mov ax,j1xmid			;
	add ax,j1xmin			;
	shr ax,1			;
	cmp ax,_latch3			;
	jb  short JOY_l2		;
	or  cl,10h			;
	jmp short JOY_r2		;
JOY_l2:	mov ax,j1xmid			;
	add ax,j1xmax			;
	shr ax,1			;
	cmp ax,_latch3			;
	ja  short JOY_r2		;
	or  cl,20h			;
JOY_r2:
	mov ax,j1ymid			;
	add ax,j1ymin			;
	shr ax,1			;
	cmp ax,_latch4			;
	jb  short JOY_u2		;
	or  cl,40h			;
	jmp short JOY_d2		;
JOY_u2:	mov ax,j1ymid			;
	add ax,j1ymax			;
	shr ax,1			;
	cmp ax,_latch4			;
	ja  short JOY_d2		;
	or  cl,80h			;
JOY_d2:
	mov bptr _JoyCode,cl		; store all direction flags
	ret                             ;






_Vblank	proc	far
	push	ax				;
	push	bx                      	;
	push	cx                      	;
	push	dx                      	;
	push	es             	        	;
	push	ds                      	;
	push	si                      	;
	push	di                      	;
	push	bp                      	; Save all registers
	mov	bp,DGROUP               	;
	mov	ds,bp                   	;

						; FRAME INDEPENDENT TIMER
						; IS DISABLED !!

	;cmp	bptr DGROUP:_TimeFlag,0         ; if (TimeFlag)
	;je	short VB_NoTime			; {
	;call	far ptr _IncTime                ;       IncTime();
	;call	far ptr _DecCountDown           ;	DecCountDown();
	;cmp	wptr DGROUP:_OffRoadOut,0	;	if (OffRoadOut)
	;jz	short VB_NoTime			;		DecOffRoad();
	;call	far ptr _DecOffRoad		; }
VB_NoTime:

	cmp	bptr _SteerSelect,3		;
	jne	short VB_readjoy		; if (GearSelect < 3 || SteerSelect < 3)
	cmp	bptr _GearSelect,3		;	ReadJoystick();
	je	short VB_nojoy			;	ibutton = 0;
VB_readjoy:
	call	Joystick			;
	mov	wptr DGROUP:_ibutton,0		;
VB_nojoy:
	mov	al,bptr DGROUP:_SteerSelect	; switch (SteerSelect)
	cmp	al,1				; { case 1:
	jne	short VB_case2			;
VB_j0l:	test	_JoyCode,02h			;		if (code & 1)
	jz	short VB_j0r			;			SteerX -= 5;
	sub	_SteerX,15			;
	jns	short VB_j0u			;
	mov	_SteerX,0			;
	jmp	short VB_j0u			;
VB_j0r:	test	_JoyCode,01h			;		if (code & 2)
	jz	short VB_j0u			;			SteerX += 5;
	add	_SteerX,15			;
	cmp	_SteerX,639			;
	jb	short VB_j0u			;
	mov	_SteerX,639			;
VB_j0u:	test	_JoyCode,04h			;		if (code & 4)
	jz	short VB_j0d			;			button = 1;
	mov	wptr DGROUP:_ibutton,1		;
	jmp	short VB_j0x			;
VB_j0d:	test	_JoyCode,08h			;		if (code & 8)
	jz	short VB_j0x			;			button = 4;
	mov	wptr DGROUP:_ibutton,4		;
VB_j0x:	mov	ax,_latch0			;		if (JoyButton)
	test	ax,1+2				;			button = 1+4;
	jz	VB_endcase			;
	mov	wptr  DGROUP:_ibutton,1+4	;
	jmp	VB_endcase			;
VB_case2:
	cmp	al,2				;   case 2:
	jne	short VB_case3			;
VB_j1l:	test	_JoyCode,20h			;		if (code & 1)
	jz	short VB_j1r			;			SteerX -= 5;
	sub	_SteerX,15			;
	jns	short VB_j1u			;
	mov	_SteerX,0			;
	jmp	short VB_j1u			;
VB_j1r:	test	_JoyCode,10h			;		if (code & 2)
	jz	short VB_j1u			;			SteerX += 5;
	add	_SteerX,15			;
	cmp	_SteerX,639			;
	jb	short VB_j1u			;
	mov	_SteerX,639			;
VB_j1u:	test	_JoyCode,40h			;		if (code & 4)
	jz	short VB_j1d			;			button = 1;
	mov	wptr  DGROUP:_ibutton,1		;
	jmp	short VB_j1x			;
VB_j1d:	test	_JoyCode,80h			;		if (code & 8)
	jz	VB_j1x				;			button = 4;
	mov	wptr  DGROUP:_ibutton,4		;
VB_j1x:	mov	ax,_latch0			;		if (JoyButton)
	test	ax,4+8				;			button = 1+4;
	jz	VB_endcase			;
	mov	wptr  DGROUP:_ibutton,1+4	;
	jmp	VB_endcase			;
VB_case3:
	cmp	al,3                            ;   case 3:	ReadMouse();
	jne	short VB_endcase		;		break;
	call	ReadMouse			; }
VB_endcase:
	test	 DGROUP:_ibutton,4		;
	jz	short VB_b1                     ; if (ibutton & 4)
	mov	al,bptr DGROUP:_BlockButtons	; {
	not	al				;
	test	al,4                            ;
	jz	short VB_b2                     ;	if (~BlockButtons & 4)
	or	wptr DGROUP:_button,4           ;       	button |= 4;
	jmp	short VB_b2                     ; }
VB_b1:
	and	bptr DGROUP:_BlockButtons,-5	; else	BlockButtons &= ~4;
VB_b2:
	test	wptr  DGROUP:_ibutton,1		;
	jz	short VB_b3                     ; if (ibutton & 1)
	mov	al,bptr DGROUP:_BlockButtons	; {
	not	al				;
	test	al,1                            ;
	jz	short VB_b4                     ;       if (~BlockButtons & 1)
	or	wptr DGROUP:_button,1           ;               button |= 1;
	jmp	short VB_b4                     ; }
VB_b3:
	and	bptr DGROUP:_BlockButtons,-2	; else	BlockButtons &= ~1;
VB_b4:
	cmp	_GearSelect,3			;
	jae	short VB_quit			; if (GearSelect < 3)
	mov	al,bptr _JoyCode		; {
	cmp	_GearSelect,1			;
	je	short VB_b5			;
	mov	cl,4				;
	shr	al,cl				;	Get joycode of stick 0/1
VB_b5:
	test	ax,1+2				;
	jz	short VB_noshift		;
	test	ax,4+8				;
	jz	short VB_noshift		;
	xor	bx,bx				;	dx = 0;
	xor	cx,cx				;	cx = 0;
VB_l:	test	al,01h				;
	jz	short VB_u			;
	mov	bx,2				;
VB_u:   mov	cx,2				;
	test	ax,08h				;
	jz	short VB_d			;
	mov	cx,1				;
VB_d:	mov	ax,bx				;
	add	ax,cx				;	h = dx + dy;
	cmp	al,bptr _ManualShift		;
	je	short VB_noshift		;	if (h && (ManualShift != h))
	mov	bptr _ManualShift,al		;		ManualShift = h;
VB_noshift:
VB_quit:
	pop	bp                              ;
	pop	di                              ;
	pop	si                              ;
	pop	ds                              ;
	pop	es                              ;
	pop	dx                              ;
	pop	cx                              ;
	pop	bx                              ; Restore all registers
	pop	ax                              ;
	iret                                    ;
_Vblank	endp




SYSTEM_TEXT	ends

_DATA	segment word public 'DATA'
_DATA	ends

_BSS	segment word public 'BSS'
_BSS	ends


	extrn	_SwitchFlag:byte
	extrn	_TimeFlag:byte
	extrn	_MouseDriverFlag:byte
	extrn	_SteerSelect:byte
	extrn	_GearSelect:byte
	extrn	_BlockButtons:byte
	extrn	_ManualShift:byte
	extrn	_JoyCode:byte
	extrn	_OverflowFlag:byte
	extrn	_SteerX:word
	extrn	_SteerY:word
	extrn	_button:word
	extrn	_latch0:word
	extrn	_latch1:word
	extrn	_latch2:word
	extrn	_latch3:word
	extrn	_latch4:word
	extrn	_frames:word
	extrn	_ibutton:word
	extrn	_OffRoadOut:word

	extrn	_IncTime:far
	extrn	_DecCountDown:far
	extrn	_DecOffRoad:far

	extrn	key

SYSTEM_TEXT	segment	byte public 'CODE'
SYSTEM_TEXT	ends

	public	_EmptyFunction
	public	_DivZero
	public	_CalibrateJoystick
	public	_Vblank
	end
