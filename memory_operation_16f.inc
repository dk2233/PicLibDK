clear_memory macro start, size
	local nextm
	BANKISEL start
    movlw	LOW(start)
	movwf	FSR
nextm
	clrf	INDF
	INCF	FSR,f
	movlw	LOW(start + size)
	xorwf	FSR,w
	BTFSS	STATUS,Z
	goto	nextm

	endm


;set error if not same
compare2bytes  macro  a, a1_l, err, bit
	BANKSEL err
	; high is in a1_l second byte
    movlw   (a >> 8) & 0xff
	;movlw UPPER a
	xorwf   a1_l+1,W
    SKPZ
    bsf     err,bit

    movlw   a & 0xff  
    xorwf   a1_l,W
    SKPZ	
    bsf     err,bit
	   
	endm

; a is a 3 bytes
;
compare3bytes  macro  a, a1,  err, bit
	BANKSEL err
    movlw   ( a >> .16 ) & 0xff
    xorwf   a1+2,W
    SKPZ	
    bsf     err,bit
	   
    movlw   (a >> 8) & 0xff
	xorwf   a1+1,W
    SKPZ
    bsf     err,bit
	   
    movlw   a & 0xff
	xorwf   a1,W
    SKPZ
    bsf     err,bit
	endm


compare4bytes  macro a, a1_ll, err, bit
	compare2bytes ((a >> .16) & 0xffff) , a1_ll+2, err, bit 

	compare2bytes (a & 0xffff), a1_ll, err, bit


	endm

;set output bits if not same
compare1byte macro a, a1 , err, bit_err  

	BANKSEL a1
    movlw   a 
    xorwf   a1,W
	BANKSEL err
    SKPZ
	bsf    err, bit_err
	endm 

;set output bits if not same
compare1byte_set_when_same macro a, a1 , reg , bit_same  

	BANKSEL a1
    movlw   a 
    xorwf   a1,W
	BANKSEL reg
    SKPZ
	bcf    reg, bit_same
	SKPNZ
	bsf    reg, bit_same
	endm 
;move literal 2 bytes into ram variable (var , var + 1)
movel_2bytes macro literal , var

    movlw   literal & 0xff
	movwf   var  
    movlw   (literal >> 8) & 0xff
    movwf   var+1
	endm

;move literal 24bits into ram variable (var , var + 1, var+2)
movel_3bytes macro literal , var

    movlw   literal & 0xff
	movwf   var  
    movlw   (literal >> 8) & 0xff
    movwf   var+1
    movlw   (literal >> 16) & 0xff
    movwf   var+2

	endm
;in case of Z set
;skip next line
check_if_0_Z_skip_next   macro  reg 
	movf   reg, w 
	SKPZ
	endm 

check_if_0_NZ_skip_next_line macro reg 
	movf    reg,W
	SKPNZ 
	endm

check_number_skip_if_even macro  reg
	movlw    1 
	andwf    reg,W
	btfsc    STATUS,Z
	endm 

check_number_skip_if_not_even macro  reg
	movlw    1 
	andwf    reg,W
	btfss    STATUS,Z
	endm 

check16bit_if_0_Z_skip_next_line macro reg
	local 	check16bit_if_0_Z_skip_next_line1
	movf reg,W
	btfss STATUS,Z ; if not zero just return - next line will be normally processed
	goto  check16bit_if_0_Z_skip_next_line1

	movf reg+1,w 
	btfss STATUS,Z
check16bit_if_0_Z_skip_next_line_exit 
	endm



move_2bytes_into_2bytes macro sourceL, destinationL
	movf sourceL,W
	movwf destinationL 
	movf sourceL+1, W 
	movwf  destinationL+1
	endm

move_3bytes_into_3bytes macro sourceL, destinationL
	movf sourceL,W
	movwf destinationL 
	movf sourceL+1, W 
	movwf  destinationL+1 
	movf sourceL+2, W 
	movwf  destinationL+2 
	endm