  bptr EQU <byte ptr>
  wptr EQU <word ptr>


  COMP_TEXT segment byte public 'CODE'

  DGROUP group _DATA, _BSS
assume cs:COMP_TEXT, ds:DGROUP, ss:DGROUP

  COMP_TEXT ends

  _DATA segment word public 'DATA'
  _DATA ends

  _BSS segment word public 'BSS'
  _BSS ends

  COMP_TEXT segment byte public 'CODE'





  _movebytes proc far          ; O.K. !!
  mov dx, bp
  mov bp, sp
  push si
  push di
  push ds
  push es
  pushf
  mov cx, wptr [bp + 12]
  cmp cx, 0
  jle short MB_exit
  mov si, wptr [bp + 4]
  mov ds, wptr [bp + 6]        ; ds:si = source
  mov di, wptr [bp + 8]
  mov es, wptr [bp + 10]       ; es:di = dest
  mov ax, es
  mov bx, ds
  cmp ax, bx
  jg short MB_d_low
  jl short MB_s_low            ; compare HiWords
  cmp di, si
  jl short MB_d_low            ; compare LoWords
MB_s_low:
  mov bx, cx                   ; MOVE: DOWNWARD
  dec bx
  add si, bx                   ; si + = count - 1
  add di, bx                   ; di + = count - 1
  std                          ; set direction flag
  rep movsb                    ; move it !
  jmp short MB_exit
MB_d_low:
  cld                          ; MOVE: UPWARD
  rep movsb                    ; move it !
MB_exit:
  popf
  pop es
  pop ds                       ; restore all regs
  pop di
  pop si
  mov bp, dx
  ret
  _movebytes endp










  _movewords proc far          ; O.K. !!
  mov dx, bp
  mov bp, sp
  push si
  push di
  push ds
  push es
  pushf
  mov cx, wptr [bp + 12]
  cmp cx, 0
  jle short MW_exit
  mov si, wptr [bp + 4]
  mov ds, wptr [bp + 6]        ; ds:si = source
  mov di, wptr [bp + 8]
  mov es, wptr [bp + 10]       ; es:di = dest
  mov ax, es
  mov bx, ds
  cmp ax, bx
  jg short MW_d_low
  jl short MW_s_low            ; compare HiWords
  cmp di, si
  jl short MW_d_low            ; compare LoWords
MW_s_low:
  mov bx, cx                   ; MOVE: DOWNWARD
  dec bx
  shl bx, 1
  add si, bx                   ; si + = count - 1
  add di, bx                   ; di + = count - 1
  std                          ; set direction flag
  rep movsw                    ; move it !
  jmp short MW_exit
MW_d_low:
  cld                          ; MOVE: UPWARD
  rep movsw                    ; move it !
MW_exit:
  popf
  pop es                       ; restore all regs
  pop ds
  pop di
  pop si
  mov bp, dx
  ret
  _movewords endp








  _abs proc far                ; O.K. !
  mov cx, bp                   ; save bp
  mov bp, sp
  mov ax, word ptr [bp + 4]    ; get n:int
  cwd
  xor ax, dx
  sub ax, dx
  mov bp, cx
  ret
  _abs endp






  _labs proc far               ; O.K. !
  mov bx, bp
  mov bp, sp
  mov ax, wptr [bp + 6]
  mov cx, wptr [bp + 4]
  cwd
  xor cx, dx
  xor ax, dx
  sub cx, dx
  sbb ax, dx
  mov dx, ax
  mov ax, cx
  mov bp, bx
  ret
  _labs endp








  _rotate proc far             ; O.K. !
  mov cx, bp                   ; save bp in cx
  mov bp, sp
  mov ax, word ptr [bp + 4]
  imul word ptr [bp + 6]       ; dx:ax = a * b
  rcl ax, 1
  rcl dx, 1
  rcl ax, 1
  rcl dx, 1
  mov ax, dx                   ; ax = (a * b >> 14)
  mov bp, cx
  ret
  _rotate endp







  _mult proc far               ; O.K. !
  mov cx, bp                   ; save bp in cx
  mov bp, sp
  mov ax, word ptr [bp + 4]
  imul word ptr [bp + 6]       ; dx:ax = a * b
  mov bp, cx
  ret
  _mult endp








  _multdiv proc far            ; O.K. !
  mov cx, bp
  mov bp, sp
  mov ax, wptr [bp + 4]
  imul wptr [bp + 6]
  mov bx, wptr [bp + 8]
  idiv bx                      ; IDIV COMMAND MUST BE 2 BYTES LONG !!

  nop
  nop

  mov bp, cx
  ret
  _multdiv endp








  _ratio proc far              ; O.K. !
  push bp
  mov bp, sp
  mov bx, wptr [bp + 10]
  sar bx, 1
  mov cx, 0                    ; cx:bx = c / 2
  mov ax, wptr [bp + 6]
  imul wptr [bp + 8]           ; dx:ax = a * b
  cmp dx, 0
  jge RA_round
  neg bx
  mov cx, - 1                  ; cx:bx = (a * b > 0) c / 2 : - c / 2
RA_round:
  add ax, bx
  adc dx, cx                   ; dx:ax = a * b + - c / 2
  mov bx, wptr [bp + 10]       ; IDIV MUST BE 2 BYTES LONG !!
  idiv bx                      ; ax = (a * b + - c / 2) / c
  pop bp
  ret
  _ratio endp








  _rot1 proc far               ; O.K. !
  push bp
  mov bp, sp
  mov ax, wptr [bp + 8]
  imul wptr [bp + 10]          ; dx:ax = y * sin
  mov bx, ax
  mov cx, dx                   ; save dx:ax in cx:bx
  mov ax, wptr [bp + 6]
  imul wptr [bp + 12]          ; dx:ax = x * cos
  sub ax, bx
  sbb dx, cx                   ; dx:ax = x * cos - y * sin
  rcl ax, 1
  rcl dx, 1
  rcl ax, 1
  rcl dx, 1
  mov ax, dx                   ; ax = (dx:ax >> 14)
  pop bp
  ret
  _rot1 endp








  _rot2 proc far               ; O.K. !
  push bp
  mov bp, sp
  mov ax, word ptr [bp + 6]
  imul word ptr [bp + 10]      ; dx:ax = x * sin
  mov bx, ax
  mov cx, dx                   ; save dx:ax in cx:bx
  mov ax, word ptr [bp + 8]
  imul word ptr [bp + 12]      ; dx:ax = y * sin
  add ax, bx
  adc dx, cx                   ; dx:ax = x * sin + y * cos
  rcl ax, 1
  rcl dx, 1
  rcl ax, 1
  rcl dx, 1
  mov ax, dx                   ; ax = (dx:ax >> 14)
  pop bp
  ret
  _rot2 endp








  _RotXY proc far
  push bp
  mov bp, sp
  push si
  push di
  push ds
  push es
  mov ax, wptr [bp + 12]       ; dx:ax = dest
  mov dx, wptr [bp + 14]
  cmp wptr [bp + 6], 0         ; if (count <= 0)
  jle short RXY_exit           ; return
  mov ds, dx
  mov di, ax                   ; ds:di = dest
  mov es, wptr [bp + 18]
  mov si, wptr [bp + 16]       ; es:si = source
RXY_loop:
mov ax, wptr es:[si + 2]
  imul wptr [bp + 10]          ; dx:ax = y * SIN
  mov bx, ax
  mov cx, dx                   ; save dx:ax in cx:bx
mov ax, wptr es:[si]
  imul wptr [bp + 8]           ; dx:ax = x * COS
  sub ax, bx
  sbb dx, cx                   ; dx:ax = x * COS - y * SIN
  rcl ax, 1
  rcl dx, 1
  rcl ax, 1
  rcl dx, 1
  push dx
mov ax, wptr es:[si]
  imul wptr [bp + 10]          ; dx:ax = x * SIN
  mov bx, ax
  mov cx, dx                   ; save dx:ax in cx:bx
mov ax, wptr es:[si + 2]
  imul wptr [bp + 8]           ; dx:ax = y * COS
  add ax, bx
  adc dx, cx                   ; dx:ax = x * SIN + y * COS
  rcl ax, 1
  rcl dx, 1
  rcl ax, 1
  rcl dx, 1
  pop ax
  mov wptr [di], ax            ; dest[0] = x'
  mov wptr [di + 2], dx        ; dest[1] = y' = (dx:ax >> 14)
mov ax, wptr es:[si + 4]
  mov wptr [di + 4], ax        ; dest[2] = source[2]
  add si, 6                    ; source + = 3
  add di, 6                    ; dest + = 3

  dec wptr [bp + 6]            ; }
  jg short RXY_loop

  mov ax, di
  mov dx, ds                   ; return dx:as as (int far * dest)
RXY_exit:
  pop es
  pop ds
  pop di
  pop si
  pop bp
  ret
  _RotXY endp









  _RotXZ proc far
  push bp
  mov bp, sp
  push si
  push di
  push ds
  push es
  mov ax, wptr [bp + 12]       ; dx:ax = dest
  mov dx, wptr [bp + 14]
  cmp wptr [bp + 6], 0         ; if (count <= 0)
  jle short RXZ_exit           ; return
  mov ds, dx
  mov di, ax                   ; ds:di = dest
  mov es, wptr [bp + 18]
  mov si, wptr [bp + 16]       ; es:si = source
RXZ_loop:
mov ax, wptr es:[si + 4]
  imul wptr [bp + 10]          ; dx:ax = z * SIN
  mov bx, ax
  mov cx, dx                   ; save dx:ax in cx:bx
mov ax, wptr es:[si]
  imul wptr [bp + 8]           ; dx:ax = x * COS
  sub ax, bx
  sbb dx, cx                   ; dx:ax = x * COS - z * SIN
  rcl ax, 1
  rcl dx, 1
  rcl ax, 1
  rcl dx, 1
  push dx
mov ax, wptr es:[si]
  imul wptr [bp + 10]          ; dx:ax = x * SIN
  mov bx, ax
  mov cx, dx                   ; save dx:ax in cx:bx
mov ax, wptr es:[si + 4]
  imul wptr [bp + 8]           ; dx:ax = z * COS
  add ax, bx
  adc dx, cx                   ; dx:ax = x * SIN + z * COS
  rcl ax, 1
  rcl dx, 1
  rcl ax, 1
  rcl dx, 1
  pop ax
  mov wptr [di], ax            ; dest[0] = x'
  mov wptr [di + 4], dx        ; dest[2] = z' = (dx:ax >> 14)
mov ax, wptr es:[si + 2]
  mov wptr [di + 2], ax        ; dest[1] = source[1]
  add si, 6                    ; source + = 3
  add di, 6                    ; dest + = 3

  dec wptr [bp + 6]            ; }
  jg short RXZ_loop

  mov ax, di
  mov dx, ds                   ; return dx:as as (int far * dest)
RXZ_exit:
  pop es
  pop ds
  pop di
  pop si
  pop bp
  ret
  _RotXZ endp








  _RotYZ proc far
  push bp
  mov bp, sp
  push si
  push di
  push ds
  push es
  mov ax, wptr [bp + 12]       ; dx:ax = dest
  mov dx, wptr [bp + 14]
  cmp wptr [bp + 6], 0         ; if (count <= 0)
  jle short RYZ_exit           ; return
  mov ds, dx
  mov di, ax                   ; ds:di = dest
  mov es, wptr [bp + 18]
  mov si, wptr [bp + 16]       ; es:si = source
RYZ_loop:
mov ax, wptr es:[si + 4]
  imul wptr [bp + 10]          ; dx:ax = z * SIN
  mov bx, ax
  mov cx, dx                   ; save dx:ax in cx:bx
mov ax, wptr es:[si + 2]
  imul wptr [bp + 8]           ; dx:ax = y * COS
  sub ax, bx
  sbb dx, cx                   ; dx:ax = y * COS - z * SIN
  rcl ax, 1
  rcl dx, 1
  rcl ax, 1
  rcl dx, 1
  push dx
mov ax, wptr es:[si + 2]
  imul wptr [bp + 10]          ; dx:ax = y * SIN
  mov bx, ax
  mov cx, dx                   ; save dx:ax in cx:bx
mov ax, wptr es:[si + 4]
  imul wptr [bp + 8]           ; dx:ax = z * COS
  add ax, bx
  adc dx, cx                   ; dx:ax = y * SIN + z * COS
  rcl ax, 1
  rcl dx, 1
  rcl ax, 1
  rcl dx, 1
  mov wptr [di + 4], dx        ; dest[2] = z' = (dx:ax >> 14)
  pop ax
  mov wptr [di + 2], ax        ; dest[1] = y'
mov ax, wptr es:[si]
  mov wptr [di], ax            ; dest[0] = source[0]
  add si, 6                    ; source + = 3
  add di, 6                    ; dest + = 3

  dec wptr [bp + 6]            ; }
  jg short RYZ_loop

  mov ax, di
  mov dx, ds                   ; return dx:as as (int far * dest)
RYZ_exit:
  pop es
  pop ds
  pop di
  pop si
  pop bp
  ret
  _RotYZ endp







  _cosinus proc far            ; O.K. !
  push bp
  mov bp, sp
  add wptr [bp + 6], 4 * 90
  jmp short SI_start           ; return( sinus(arg + 4 * 90))


  _sinus proc far              ; O.K. !
  push bp
  mov bp, sp
SI_start:
  sub sp, 2                    ; allocate space: flag
  mov bptr [bp - 1], 0         ; flag = FALSE
  mov ax, wptr [bp + 6]        ; ax = arg
  or ax, ax
  jge short SI_arg             ; if (arg < 0)
  mov bptr [bp - 1], - 1       ; flag = TRUE
  neg ax                       ; arg = - arg
SI_arg:
  cmp ax, 4 * 360
  jl short SI_m1
  mov bx, 4 * 360
  cwd                          ; if (arg > 4 * 360)
  idiv bx                      ; arg %= 4 * 360
  mov ax, dx
SI_m1:
  cmp ax, 4 * 180
  jle short SI_m2              ; if (arg > 4 * 180)
  not bptr [bp - 1]            ; arg - = 4 * 180
  sub ax, 4 * 180              ; flag ^= TRUE
SI_m2:
  cmp ax, 4 * 90
  jle short SI_m3              ; if (arg > 4 * 90)
  neg ax                       ; arg = 4 * 180 - arg
  add ax, 4 * 180
SI_m3:
  mov bx, ax
  shl bx, 1
mov ax, wptr DGROUP:_SineData[bx]
  cmp bptr [bp - 1], 0         ; ax = SineData[arg]
  je short SI_neg              ; if (flag)
  neg ax                       ; ax = - ax
SI_neg:
  mov sp, bp
  pop bp
  ret
  _sinus endp
  _cosinus endp










  _arcsin proc far             ; O.K. !!
  push bp
  mov bp, sp
  sub sp, 6                    ; alloc space: sign, sin, angle
  push si
  push di
  mov bptr [bp - 5], 0         ; sign = FALSE
  cmp wptr [bp + 6], 0
  jge short AS_testa           ; if (a < 0)
  neg wptr [bp + 6]            ; a = - a
  mov bptr [bp - 5], - 1       ; sign = TRUE
AS_testa:
  mov ax, wptr [bp + 6]
  cmp ax, wptr [bp + 8]
  jle short AS_testb           ; if (a > b)
  xor ax, ax                   ; return(0)
  jmp short AS_quit
AS_testb:
  mov dx, wptr [bp + 6]
  xor ax, ax
  sar dx, 1
  rcr ax, 1
  sar dx, 1
  rcr ax, 1                    ; dx:ax = (16384 * a)
  mov bx, wptr [bp + 8]
  idiv bx                      ; ax = (16384 * a) / b
  mov di, ax                   ; sin = (16384 * a) / b
  mov cx, 4 * 90
  imul cx
  mov ax, dx
  shl ax, 1
  shl ax, 1
  mov wptr [bp - 4], ax        ; angle = mult( sin, 4 * 90) >> 14
  mov cx, ax                   ; cx = angle

  mov si, ax
  shl si, 1
add si, offset DGROUP:_SineData
  mov bx, wptr [si]            ; p = SineData + angle
  mov ax, 1
  cmp bx, di
  jl short AS_getadd
  mov ax, - 1
AS_getadd:
  mov bx, ax                   ; add = (p[0] < sin) ? 1 : - 1
  mov dx, ax
  shl dx, 1
AS_loop:
  mov ax, wptr [si]            ; for (;;)
  cmp ax, di                   ; { if (p[0] <= sin && sin <= p[1])
  jg short AS_cont             ; break
  mov ax, wptr [si + 2]
  cmp ax, di
  jge short AS_break
AS_cont:
  add cx, bx                   ; angle + = add
  add si, dx                   ; p + = add
  jmp short AS_loop            ; }
AS_break:
  mov ax, cx
  cmp bptr [bp - 5], 0
  je short AS_quit             ; return( (sign) ? - angle : angle)
  neg ax
AS_quit:
  pop di
  pop si
  mov sp, bp
  pop bp
  ret
  _arcsin endp










  _project proc far            ; O.K. !!
  push bp
  push si
  mov bp, sp
  mov bx, wptr [bp + 12]
  or bx, bx
  jne short PR_nozero
  mov bx, 1
PR_nozero:
  mov si, wptr [bp + 14]
  mov ax, wptr [bp + 8]
  cwd
  mov dh, dl
  mov dl, ah
  mov ah, al
  mov al, 0
  idiv bx                      ; IDIV MUST BE 2 BYTES LONG !!
  mov [si], ax                 ; p[0] = x' = (x << 8) / z
  mov ax, wptr [bp + 10]
  cwd
  mov dh, dl
  mov dl, ah
  mov ah, al
  mov al, 0
  idiv bx                      ; IDIV MUST BE 2 BYTES LONG !!
  neg ax
  mov [si + 2], ax             ; p[1] = y' = - (y << 8) / z
  pop si
  pop bp
  ret
  _project endp










  _ProjectSmall proc far       ; O.K. !!
  push bp
  mov bp, sp
  sub sp, 2                    ; allocate space:
  push si
  push di
  mov si, wptr [bp + 10]
  mov di, wptr [bp + 8]
  mov cx, wptr [bp + 6]
  cmp cx, 0
  jle short PS_exit            ; for (; - - count >= 0;)
PS_loop:
  mov bx, wptr [si + 4]        ; { z = source[2]
  mov ax, wptr [si]            ; x = source[0]
  cwd
  mov dh, dl
  mov dl, ah
  mov ah, al
  xor al, al
  idiv bx                      ; IDIV MUST BE 2 BYTES LONG !!
  mov wptr [di], ax            ; dest[0] = x' = (x << 8) / z
  mov ax, wptr [si + 2]
  cwd                          ; y = source[1]
  mov dh, dl
  mov dl, ah
  mov ah, al
  xor al, al
  idiv bx                      ; IDIV MUST BE 2 BYTES LONG !!
  neg ax
  mov wptr [di + 2], ax        ; dest[1] = y' = - (y << 8) / z
  add si, 2 * 3
  add di, 2 * 2
  loop PS_loop                 ; }
PS_exit:
  mov ax, di                   ; return( (int * ) dest)
  pop di
  pop si
  mov sp, bp
  pop bp
  ret
  _ProjectSmall endp











  _IsVisible proc far
  push bp
  mov bp, sp
  push si
  push di
  mov bx, wptr [bp + 6]
  mov si, bx
  add si, bx
  add si, bx
  shl si, 1
  add si, wptr [bp + 10]       ; si points to &v[6 * i]
  mov bx, wptr [bp + 8]
  mov di, bx
  add di, bx
  add di, bx
  shl di, 1
  add di, wptr [bp + 10]       ; di points to &v[6 * normal]
  mov ax, [si]
  imul wptr [di]
  mov bx, ax
  mov cx, dx                   ; save (v[0] * p[0])
  mov ax, wptr [si + 2]
  imul wptr [di + 2]
  add bx, ax
  adc cx, dx                   ; save (v[0] * p[0]) + (v[1] * p[1])
  mov ax, wptr [si + 4]
  imul wptr [di + 4]
  add ax, bx
  adc dx, cx                   ; dx:ax = v[0] * p[0] + v[1] * p[1] + v[2] * p[2]
  mov ax, dx
  rol ax, 1                    ; roll bit 31 of dx:ax down
  and ax, 01h                  ; return( (dx:ax < 0))
  pop di
  pop si
  mov sp, bp
  pop bp
  ret
  _IsVisible endp







  _AddBCD proc far             ; O.K. !
  mov dx, bp
  mov bp, sp
  mov bx, wptr [bp + 8]
  mov al, bptr [bx]
  add al, bptr [bp + 4]
  daa
  mov bptr [bx], al            ; add 1st byte
  mov al, bptr [bx + 1]
  adc al, bptr [bp + 5]
  daa
  mov bptr [bx + 1], al        ; add 2nd byte
  mov al, bptr [bx + 2]
  adc al, bptr [bp + 6]
  daa
  mov bptr [bx + 2], al        ; add 3rd byte
  mov al, bptr [bx + 3]
  adc al, bptr [bp + 7]
  daa
  mov bptr [bx + 3], al        ; add 4th byte
  mov bp, dx
  ret
  _AddBCD endp







  _SubBCD proc far
  mov dx, bp
  mov bp, sp
  mov bx, wptr [bp + 8]
  mov al, bptr [bx]
  sub al, bptr [bp + 4]
  das
  mov bptr [bx], al            ; sub 1st byte
  mov al, bptr [bx + 1]
  sbb al, bptr [bp + 5]
  das
  mov bptr [bx + 1], al        ; sub 2nd byte
  mov al, bptr [bx + 2]
  sbb al, bptr [bp + 6]
  das
  mov bptr [bx + 2], al        ; sub 3rd byte
  mov al, bptr [bx + 3]
  sbb al, bptr [bp + 7]
  das
  mov bptr [bx + 3], al        ; sub 4th byte
  mov bp, dx
  ret
  _SubBCD endp






  _IncTime proc far
mov al, bptr DGROUP:_Time
  add al, 05h
  daa
mov bptr DGROUP:_Time, al
mov al, bptr DGROUP:_Time + 1
  adc al, 00h
  daa
mov bptr DGROUP:_Time + 1, al
  cmp al, 59h
  jbe short IT_quit
  xor al, al
mov bptr DGROUP:_Time + 1, al
mov al, bptr DGROUP:_Time + 2
  add al, 01h
  daa
mov bptr DGROUP:_Time + 2, al
  cmp al, 59h
  jbe short IT_quit
mov bptr DGROUP:_Time + 2, 0
IT_quit:
  ret
  _IncTime endp







  _DecCountDown proc far       ; O.K. !
mov al, bptr DGROUP:_TimeOut
  sub al, 05h
  das
mov bptr DGROUP:_TimeOut, al  ; subtract 1 / 100 seconds
mov al, bptr DGROUP:_TimeOut + 1
  sbb al, 00h
  das
mov bptr DGROUP:_TimeOut + 1, al ; subtract 1 / 1 seconds
  cmp al, 59h
  jbe short DC_quit
mov bptr DGROUP:_TimeOut + 1, 59h
mov bptr DGROUP:_TimeOut, 99h
sub bptr DGROUP:_TimeOut + 2, 1 ; subtract minutes
  jns short DC_quit
mov bptr DGROUP:_TimeOutFlag, - 1 ; TimeOutFlag = TRUE
  xor ax, ax
mov wptr DGROUP:_TimeOut, ax  ; TimeOut = 0L
mov wptr DGROUP:_TimeOut + 2, ax
DC_quit:
  ret
  _DecCountDown endp







  _DecOffRoad proc far         ; O.K. !
mov al, bptr DGROUP:_OffRoadOut
  sub al, 05h
  das
mov bptr DGROUP:_OffRoadOut, al ; subtract 1 / 100 seconds
mov al, bptr DGROUP:_OffRoadOut + 1
  sbb al, 00h
  das
mov bptr DGROUP:_OffRoadOut + 1, al ; subtract 1 / 1 seconds
  ret
  _DecOffRoad endp







  _AddTime proc far
  mov dx, bp
  mov bp, sp
  mov bx, wptr [bp + 8]        ; bx points to time
  mov al, bptr [bx + 1]
  add al, bptr [bp + 4]
  daa
  mov bptr [bx + 1], al        ; add seconds
  pushf
  mov al, bptr [bx + 2]
  adc al, bptr [bp + 5]
  daa
  mov bptr [bx + 2], al        ; add minutes
  popf                         ; Overflow on seconds ?
  jc short AT_adjust
  cmp bptr [bx + 1], 59h       ; if (seconds > 0x59)
  jbe short AT_quit            ; {
  add al, 01h                  ; minutes + +
  daa                          ; }
  mov bptr [bx + 2], al
AT_adjust:
  mov al, bptr [bx + 1]        ; if (overflow || (seconds > 0x59))
  sub al, 60h                  ; {
  daa                          ; seconds - = 0x60
  mov bptr [bx + 1], al        ; }
AT_quit:
  mov bp, dx
  ret
  _AddTime endp






  COMP_TEXT ends
  _DATA segment word public 'DATA'
  _DATA ends


extrn _SineData:word
extrn _Time:word
extrn _TimeOut:word
extrn _OffRoadOut:word
extrn _TimeOutFlag:byte

  COMP_TEXT segment byte public 'CODE'
  COMP_TEXT ends

  public _ProjectSmall
  public _DecCountDown
  public _DecOffRoad
  public _movewords
  public _movebytes
  public _multdiv
  public _rotate
  public _project
  public _cosinus
  public _IsVisible
  public _sinus
  public _arcsin
  public _ratio
  public _IncTime
  public _mult
  public _labs
  public _AddTime
  public _abs
  public _RotYZ
  public _RotXZ
  public _RotXY
  public _SubBCD
  public _AddBCD
  public _rot2
  public _rot1
  end
