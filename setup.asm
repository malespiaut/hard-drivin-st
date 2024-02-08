

	bptr	EQU	<byte ptr>
	wptr	EQU	<word ptr>




SETUP_TEXT	segment	byte public 'CODE'

DGROUP	group	_DATA,_BSS
	assume	cs:SETUP_TEXT,ds:DGROUP,ss:DGROUP

SETUP_TEXT	ends



FX_TEXT	segment para public 'CODE'
FX_TEXT ends





_DATA	segment word public 'DATA'



;
;sound variables
;

colcnt		db 10,0
cnt2		db 1,1		;interrupt toggle
csound		db 0,0		;current sound
adur		db 10,0		;duration of current sound
offlatch	db 10,0
offdelay	db 5,0
onlatch		db 1,0
ondelay		db 5,0
freqa		dw 13600    	;start freq
freqi		dw 0		;change in freq

cnt4		dw 0

timervec	equ 4*008h
v80vec		equ 4*080h
v81vec		equ 4*081h
keyvec		equ 4*009h

key		db 256 dup(0)

;
;vector storage
;

old80vec	dw 0,0
old81vec	dw 0,0
oldtimervec	dw 0,0
oldkeyvec     	dw 0,0


_DATA	ends



_BSS	segment word public 'BSS'
_BSS	ends



SETUP_TEXT	segment	byte public 'CODE'




SPACE		equ 57
O		equ 24
A		equ 30
G		equ 34
J		equ 36
Y		equ 21
U		equ 22
B		equ 48
N		equ 49
H		equ 35
CR		equ 28
ESCAPE		equ  1
M		equ 50
C		equ 46
Q		equ 16
D		equ 32
S		equ 31
P		equ 25
UP		equ 72
DOWN		equ 80
LEFT		equ 75
RIGHT		equ 77
N1		equ  2
N2		equ  3
N3		equ  4
N4		equ  5
N5		equ  6
LBUTTON		equ 07Eh
RBUTTON		equ 07Dh



_SetVecs	proc	far
		push	ds		;
		cli	    		; disable interrupts

		xor ax,ax 		; change segment regesters
		mov es,ax               ;
		mov ax,DGROUP           ;
		mov ds,ax               ;

		call _InstallSound	;

		mov ax,es:timervec 	; save old timer vector
		mov wptr cs:timepoke,ax ;
		mov wptr oldtimervec,ax	;
		mov ax,es:timervec+2    ;
		mov wptr cs:timepoke+2,ax
		mov wptr oldtimervec+2,ax

		mov ax,offset timer	; change timer int vector
		mov es:timervec,ax	;
		mov ax,cs		;
		mov es:timervec+2,ax	;

		mov al,036h		;
		out 043h,al		; selects timer

		mov bx,11932		; 100 htz interrupt
		mov al,bl		;
		out 040h,al		;
		mov al,bh		;
		out 040h,al		;

		mov ax,es:keyvec 	; save old key vector
		mov oldkeyvec,ax        ;
		mov ax,es:keyvec+2      ;
		mov oldkeyvec+2,ax      ;

		mov ax,offset key_int 	; change key int vector
		mov es:keyvec,ax	;
		mov ax,cs		;
		mov es:keyvec+2,ax	;

		mov al,80h		;
		or key[LBUTTON],al	; Set the Repeat flag on certain keys
		or key[RBUTTON],al	;

		or key[LEFT],al		;
		or key[RIGHT],al	;
		or key[UP],al		;
		or key[DOWN],al		;

		mov ax,000Eh		;
		mov DGROUP:_PokePitch,ax   ;
		mov ax,FX_TEXT		   ;
		mov DGROUP:_PokePitch+2,ax ; set PokePitch to pitch variable

		mov ah,01h		;
		int 081h		; Start with clean sound

		sti                     ; enable interrupts
		pop	ds		;
		ret                     ;
_SetVecs	endp                    ;






timer:		push ds			;
		push es			;
		push ax 	        ;
		push bx			;
		push cx			;
		push dx         	;

		mov ax,DGROUP   	;
		mov ds,ax       	;
		inc wptr _frames	; frames++;

		mov ah,0 		; sound effects
		int 081h		;

tune2:		mov al,020h     	;
		out 020h,al		; clear interrupt flag

		dec cnt4        	;
		jns tm30        	; call BIOS routine every fifth
		mov cnt4,5		;
		pop dx          	; 'extended' return via the BIOS routine
		pop cx          	;
		pop bx          	;
		pop ax          	;
		pop es			;
		pop ds          	;
		db 0eah         	; FAR jump to BIOS routine poked in
timepoke:	dw 0,0          	;

tm30:		pop dx          	; 'normal' return
		pop cx          	;
		pop bx          	;
		pop ax          	;
		pop es			;
		pop ds          	;
		iret            	;






_CheckKeyboard	proc	far
		xor	bl,bl		;

		test	key[ESCAPE],1	; locked
		jz	short ck01	;
		mov     bl,27		;
		xor	key[ESCAPE],1	;
		jmp	ck_exit		;

ck01:		test	key[G],1	; repeat
		jz	short ck02	;
		mov	bl,'g'		;
		jmp	ck_exit		;

ck02:		test	key[J],1	; repeat
		jz	short ck03	;
		mov	bl,'j'		;
		jmp	ck_exit		;

ck03:		test	key[Y],1	; locked
		jz	short ck04	;
		mov	bl,'y'		;
		xor	key[Y],1	;
		jmp	ck_exit		;

ck04:		test	key[U],1	; locked
		jz	short ck05	;
		mov	bl,'u'		;
		xor	key[U],1	;
		jmp	ck_exit		;

ck05:		test	key[B],1	; repeat
		jz	short ck06	;
		mov	bl,'b'		;
		jmp	ck_exit		;

ck06:		test	key[N],1	; repeat
		jz	short ck07	;
		mov	bl,'n'		;
		jmp	ck_exit		;

ck07:		test	key[H],1	; repeat
		jz	short ck08	;
		mov	bl,'h'		;
		jmp	ck_exit		;

ck08:		and	key[C],1	; locked
		jz	short ck09	;
		mov	bl,'c'		;
		xor	key[C],1	;
		jmp	ck_exit		;

ck09:		and	key[Q],1	; locked
		jz	short ck10	;
		mov	bl,'q'		;
		xor	key[Q],1	;
		jmp	ck_exit		;

ck10:		and	key[CR],1	; locked
		jz	short ck11	;
		mov	bl,13		;
		xor	key[CR],1	;
		jmp	ck_exit		;

ck11:		test	key[A],1	; locked
		jz	short ck12	;
		mov	bl,'a'		;
		xor	key[A],1	;
		jmp	ck_exit		;

ck12:		test	key[O],1	; locked
		jz	short ck13	;
		mov	bl,'o'		;
		xor	key[O],1	;
		jmp	ck_exit		;

ck13:		test	key[SPACE],1	; locked
		jz	short ck14	;
		mov	bl,' '		;
		xor	key[SPACE],1	;
		jmp	ck_exit		;

ck14:		test	key[N1],1	; locked
		jz	short ck15	;
		mov	bl,'1'		;
		xor	key[N1],1	;
		jmp	ck_exit		;

ck15:		test	key[N2],1	; locked
		jz	short ck16	;
		mov	bl,'2'		;
		xor	key[N2],1	;
		jmp	ck_exit		;

ck16:		test	key[N3],1	; locked
		jz	short ck17	;
		mov	bl,'3'		;
		xor	key[N3],1	;
		jmp	ck_exit		;

ck17:		test	key[N4],1	; locked
		jz	short ck18	;
		mov	bl,'4'		;
		xor	key[N4],1	;
		jmp	ck_exit		;
ck18:
		test	key[N5],1	; locked
		jz	short ck19	;
		mov	bl,'5'		;
		xor	key[N5],1	;
		jmp	ck_exit		;
ck19:
		test	key[P],1	; locked
		jz	short ck20	;
		mov	bl,'p'		;
		xor     key[P],1	;
		jmp	ck_exit		;

ck20:           test	key[UP],1	; locked
		jz	short ck21	;
		mov	bl,1		;
		xor	key[UP],1	;
		jmp	ck_exit		;

ck21:		test	key[DOWN],1	; locked
		jz	short ck22	;
		mov	bl,2		;
		xor	key[DOWN],1	;
		jmp	short ck_exit	;

ck22:		test	key[LEFT],1	; locked
		jz	short ck23	;
		mov	bl,3		;
		xor	key[LEFT],1	;
		jmp	short ck_exit	;

ck23:		test	key[RIGHT],1	; locked
		jz	short ck_exit	;
		mov	bl,4		;
		xor	key[RIGHT],1	;

ck_exit:        xor	ah,ah		;
		mov	al,bl		;
		ret			;
_CheckKeyboard	endp









_ResetKeys	proc far
		mov	al,080h		;
		and	key[UP],al	;
		and	key[DOWN],al	;
		and	key[LEFT],al	;
		and	key[RIGHT],al	;
		ret			;
_ResetKeys	endp








_CheckSteeringKeys      proc far

csk18:          test    key[N1],1	; locked
		jz      short csk19	;
                mov     bl,'1'          ;
		jmp     short csk_exit	;

csk19:          test    key[N2],1	; locked
		jz      short csk20	;
		mov     bl,'2'		;
		jmp     short csk_exit	;

csk20:          test    key[N3],1	; locked
		jz      short csk21	;
		mov     bl,'3'		;
		jmp     short csk_exit	;

csk21:          test    key[N4],1	; locked
		jz      short csk22	;
		mov     bl,'4'		;
		jmp	short csk_exit	;

csk22:          mov     bl,80h		; start looking for simultaneous keys

csk13:          test    key[SPACE],1	; locked
		jz      short csk14	;
		or      bl,16		;

csk14:		cmp	_TimeFlag,0	; if (!TimeFlag)
		jz	short csk_exit	;	SKIP CURSOR-KEYS !

		test    key[UP],40h	; locked
		jz      short csk15	;
		or      bl,1		;

csk15:          test    key[DOWN],40h	; locked
		jz      short csk16	;
		or      bl,2		;

csk16:          test    key[LEFT],40h	; locked
		jz      short csk17	;
		or      bl,4		;

csk17:          test    key[RIGHT],40h	; locked
		jz      short csk_exit	;
		or      bl,8		;

csk_exit:       mov     al,bl		;
		xor     ah,ah		;
		ret
_CheckSteeringKeys	endp




;====================================================
;NAME	program to redirect keyboard interrupts
;DATE	3/11/86 mhg
;FUNC 	keyboard scan routine
;ENTRY	by key interrupt at vector 024h
;EXIT	if key pressed	   1 in key_table+scancode
;	if key not pressed 0 in key_table+scancode
;SUBS	none used
;====================================================


key_int:	push bx			;
		push ax			; save regs
		push ds			;
		mov ax,DGROUP		;
		mov ds,ax		;
		in al,60h		;

		push ax			; save scan code
		in al,61h		;
		mov ah,al		;
		or al,80h		;
		out 61h,al		; clear keyboard
		xchg ah,al		;
		out 61h,al		; reset keyboard
		pop bx			;
		xor bh,bh		;
		test bl,bl		;
		jns short keypressed	;

keyreleased:	and bl,7Fh		; if (key released)
		and key[bx],80h		;	clear bits 0,6
		jmp short over4		;

keypressed:     mov al,key[bx]		; if (key pressed)
		test al,80h		;
		jnz short ki_repeat	;       if (locked key)
		test al,40h		;       	if (key pressed)
		jnz short over4		;                      	skip
ki_repeat:      or al,41h		;	set bits 0,6
		mov key[bx],al		;

over4:		mov al,020h		;
		out 020h,al		; interrupt control port

		pop ds			;
		pop ax			; restore ax
		pop bx			;
		iret			;






_ClearVecs	proc	far		;
		cli			; reset key vector
		mov ax,0		;
		mov es,ax		;

		mov ax,old81vec		;
		mov es:v81vec,ax	;
		mov ax,old81vec+2	;
		mov es:v81vec+2,ax	;

		mov ax,oldkeyvec	;
		mov es:keyvec,ax	;
		mov ax,oldkeyvec+2	;
		mov es:keyvec+2,ax	;

		mov ax,oldtimervec	;
		mov es:timervec,ax	;
		mov ax,oldtimervec+2	;
		mov es:timervec+2,ax	;

		mov al,036h		;
		out 043h,al		;

		mov bx,0 		; 100 htz interrupt
		mov al,bl		;
		out 040h,al		;
		mov al,bh		;
		out 040h,al		;

		sti                     ;

		in al,061h              ;
		and al,0fch             ;
		out 061h,al		; kill speaker sound
		ret                     ;
_ClearVecs	endp





_InstallSound:  push es			;
		xor ax,ax		;
		mov es,ax		;

		mov ax,es:v81vec	;
		mov old81vec,ax		;
		mov ax,es:v81vec+2	;
		mov old81vec+2,ax	;


		xor ax,ax		;
		mov es,ax		;

		mov ax,01aeh		;
		mov es:v81vec,ax	;
		mov ax,FX_TEXT		;
		mov es:v81vec+2,ax	;

		;mov ax,offset nullit
		;mov es:v81vec,ax
		;mov ax,cs
		;mov es:v81vec+2,ax

		pop es			;
		ret			;




_SoundOff: 	cli			;
		xor ax,ax		;
		mov es,ax		;
		mov ax,offset cs:nullit ;
		mov es:v81vec,ax        ;
		mov es:v81vec+2,cs      ;
		sti                     ;
		ret                     ;


nullit:		iret                    ;





_SetAudioVector	proc	far		;
		xor	ax,ax		;
		mov	es,ax		;
		mov	ax,wptr DGROUP:_SoundModule+2
		mov	bx,00h		;
		mov	es:v80vec+0,bx	;
		mov	es:v80vec+2,ax	;
		ret			;
_SetAudioVector	endp






_InitTune	proc	far		;
		mov	cx,bp		;
		mov	bp,sp		;
		mov	dx,bp[4]	; dl = tune speed
		mov	ax,bp[6]	; al = tune number
		mov	ah,01h		;
		int	080h		; Init tune
		mov	sp,bp		;
		mov	bp,cx		;
		ret			;
_InitTune	endp






_RefreshTune	proc	far		;
		mov	ah,00h		;
		int	080h		; refresh tune
		ret			;
_RefreshTune	endp			;






SETUP_TEXT	ends

_DATA	segment word public 'DATA'
_DATA	ends



	extrn	_TimeFlag:byte
	extrn   _PokePitch:word
	extrn	_frames:word
	extrn	_SoundModule:word
	extrn	_ibutton:word


SETUP_TEXT	segment	byte public 'CODE'
SETUP_TEXT	ends


	public	key

	public	_ClearVecs
	public	_SetVecs
	public	_SetAudioVector
	public	_CheckKeyboard
	public	_InitTune
	public	_RefreshTune
	public  _CheckSteeringKeys
	public	_ResetKeys

	end
