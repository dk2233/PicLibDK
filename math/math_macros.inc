macro_mul_16f  macro  x_h, x, y,rh,rl
    

    ; x_h|x * y = rh|rl
    LOCAL   mult_loop, mult_done,shift_only
    
    ; Initialize
    clrf    rl       
    CLRF    rh       
    clrf    x_h
    
    movf    y,w
    ; Check for zero multiplier
    BTFSC   STATUS, Z
    GOTO    mult_done
    
mult_loop:
    ; Check LSB of multiplier
    ;if it is not even
    BTFSS   y, 0
    GOTO    shift_only
    
    ; Add multiplicand to result
    MOVF    x, W
    addwf   rl,f
    btfsc   STATUS,C
    incf   rh, f    ; Add to high byte
    movf  x_h,w 
    addwf rh,f
    
shift_only:
    ; Shift multiplicand left (multiply by 2)
    
    BCF     STATUS, C
    rlf     x,f
    rlf     x_h,f

    
    BCF     STATUS, C
    RRF     y, F
    
    ; Check if done
    MOVF    y, F
    BTFSS   STATUS, Z
    GOTO    mult_loop
    
mult_done:
    ENDM



;  division 
;  x_h|x / y
; important is to store all variables in same bank
macro_division_16f macro  x_h, x, y,  rh, rl
    LOCAL macro_division_16f_loop, macro_division_16f_loop_higher_zero, macro_division_16f_loop_1, macro_division_16f_loop_0
    clrf  rh
    clrf  rl 

    PAGESEL  macro_division_16f_loop_0
    check_if_0_NZ_skip_next_line y
    goto     macro_division_16f_loop_0

macro_division_16f_loop
    check_if_0_NZ_skip_next_line x_h
    goto    macro_division_16f_loop_higher_zero

    movf     y,w
    subwf    x , f  ;x - y = result to f only if we know there is no 0 in x_h 
    btfsc    STATUS,C ; if not C means we exceeds reg 
    goto     macro_division_16f_loop_1
    
    decf     x_h,f
    goto     macro_division_16f_loop_1
macro_division_16f_loop_higher_zero 
    movf     y,w
    subwf  x,w 
    btfss  STATUS,C  ; if cross byte here - do not really subtract - keep as a reminder
    goto   macro_division_16f_loop_0  ; finish operation

    movwf  x

macro_division_16f_loop_1       
    incf  rl,f 
    btfsc STATUS,Z
    incf  rh,f 
    goto  macro_division_16f_loop

macro_division_16f_loop_0
    ;in x_h|x there is a reminder

    endm 

;this is taken from http://piclist.com/techref/microchip/math/div/24by16.html
;author - Nicholai Golovchenko
;Inputs:
;   Dividend - AARGB0:AARGB1:AARGB2 (0 - most significant!)
;   Divisor  - BARGB0:BARGB1
;Temporary:
;   Counter  - LOOPCOUNT
;   Remainder- REMB0:REMB1
;Output:
;   Quotient - AARGB0:AARGB1:AARGB2
;
; Size: 27
; Max timing: 4+24*(6+6+4+3+5)-1+3+2=584 cycles (with return)
; Min timing: 4+24*(6+6+5+5)-1+3+2=536 cycles (with return)
;
;25-Sep-2000    Original version
;20-Oct-2001    Made the loop one instruction shorter, comments
;               review.
macro_division_16f_24bit_16bit macro  AARGB0, AARGB1, AARGB2, BARGB0, BARGB1, REMB0 , REMB1, LOOPCOUNT
    ; Input: 
;   x_h:x - 16-bit dividend
;   y_h:y_l - 16-bit divisor
; Output:
;   rh:rl - 16-bit quotient
;   REMAINDER_H:REMAINDER_L - 16-bit remainder

     LOCAL  LOOPU2416, UOK46LL

        CLRF REMB0
        CLRF REMB1
        MOVLW .24
        MOVWF LOOPCOUNT
LOOPU2416
        RLF AARGB2, W           ;shift dividend left to move next bit to remainder
        RLF AARGB1, F           ;
        RLF AARGB0, F           ;

        RLF REMB1, F            ;shift carry (next dividend bit) into remainder
        RLF REMB0, F

        RLF AARGB2, F           ;finish shifting the dividend and save  carry in AARGB2.0,
                                ;since remainder can be 17 bit long in some cases
                                ;(e.g. 0x800000/0xFFFF). This bit will also serve
                                ;as the next result bit.
         
        MOVF BARGB1, W          ;substract divisor from 16-bit remainder
        SUBWF REMB1, F          ;
        MOVF BARGB0, W          ;
        BTFSS STATUS, C         ;
        INCFSZ BARGB0, W        ;
        SUBWF REMB0, F          ;

;here we also need to take into account the 17th bit of remainder, which
;is in AARGB2.0. If we don't have a borrow after subtracting from lower
;16 bits of remainder, then there is no borrow regardless of 17th bit 
;value. But, if we have the borrow, then that will depend on 17th bit 
;value. If it is 1, then no final borrow will occur. If it is 0, borrow
;will occur. These values match the borrow flag polarity.

        SKPNC                   ;if no borrow after 16 bit subtraction
         BSF AARGB2, 0          ;then there is no borrow in result. Overwrite
                                ;AARGB2.0 with 1 to indicate no
                                ;borrow.
                                ;if borrow did occur, AARGB2.0 already
                                ;holds the final borrow value (0-borrow,
                                ;1-no borrow)

        BTFSC AARGB2, 0         ;if no borrow after 17-bit subtraction
         GOTO UOK46LL           ;skip remainder restoration.

        ADDWF REMB0, F          ;restore higher byte of remainder. (w 
                                ;contains the value subtracted from it
                                ;previously)
        MOVF BARGB1, W          ;restore lower byte of remainder
        ADDWF REMB1, F          ;

UOK46LL

        DECFSZ LOOPCOUNT, f     ;decrement counter
         GOTO LOOPU2416         ;and repeat the loop if not zero.

        endm

