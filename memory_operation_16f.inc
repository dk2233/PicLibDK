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
	BANKSEL var
    movlw   literal & 0xff
	movwf   var  
    movlw   (literal >> 8) & 0xff
    movwf   var+1
	endm

;move literal 24bits into ram variable (var , var + 1, var+2)
movel_3bytes macro literal , var
	BANKSEL var
    movlw   literal & 0xff
	movwf   var  
    movlw   (literal >> 8) & 0xff
    movwf   var+1
    movlw   (literal >> .16) & 0xff
    movwf   var+2

	endm

movel_4bytes macro literal , var
	;LOCAL x = literal >> .16
	BANKSEL var
	;movel_2bytes (literal & 0xffff) , var 
	;movel_2bytes (x & 0xffff) , var+2

    movlw   literal & 0xff
	movwf   var  
    movlw   (literal >> 8) & 0xff
    movwf   var+1
    movlw   (literal >> .16) & 0xff
    movwf   var+2
    movlw   (literal >> .24) & 0xff
    movwf   var+3

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
	BANKSEL sourceL
	movf sourceL,W
	BANKSEL destinationL
	movwf destinationL 
	BANKSEL sourceL
	movf sourceL+1, W 
	BANKSEL destinationL
	movwf  destinationL+1
	endm

move_3bytes_into_3bytes macro sourceL, destinationL
	BANKSEL sourceL
	movf sourceL,W
	BANKSEL destinationL
	movwf destinationL 
	BANKSEL sourceL
	movf sourceL+1, W 
	BANKSEL destinationL
	movwf  destinationL+1 
	BANKSEL sourceL
	movf sourceL+2, W 
	BANKSEL destinationL
	movwf  destinationL+2 
	endm

mem_cpy_FSR  macro sourceL, destinationL, temp_var, byte_number
	LOCAL mem_cpy_FSR_loop

mem_cpy_FSR_loop
	BANKISEL sourceL
	movlw   LOW sourceL
	BANKSEL byte_number
	addwf   byte_number,w
	movwf   FSR 
	decf    FSR,f
	movf    INDF,w 
	BANKSEL temp_var
	movwf   temp_var  
	movlw   LOW destinationL
	BANKSEL byte_number
	addwf   byte_number,w
	movwf    FSR 
	decf  FSR,f
	BANKSEL temp_var
	movf   temp_var,w  
	BANKISEL destinationL
	movwf    INDF 


	decfsz  byte_number,f 
	goto mem_cpy_FSR_loop


	endm

compareNbyte_return_W  macro  a_address_h, a_address_l, b_address_h, b_address_l, n_bytes , temp
	LOCAL compareNbyte_return_W_loop, compareNbyte_return_W_exit, compareNbyte_return_W_still_same

compareNbyte_return_W_loop
	;if same return 0
	movf a_address_l, w 
	addwf n_bytes,w
	movwf FSR 
	movlw 1 
	subwf FSR,f
	movf a_address_h,w 
	bcf STATUS,IRP
	SKPZ 
	bsf STATUS,IRP	
	movf  INDF,w 
	movwf temp 
	movf b_address_l,w 
	addwf n_bytes,w
	movwf FSR
	movlw 1 
	subwf FSR,f
	movf b_address_h,w
	bcf STATUS,IRP
	SKPZ 
	bsf STATUS,IRP

	movf temp,w 
	xorwf INDF,w 
	PAGESEL compareNbyte_return_W_still_same
	SKPNZ ;if not same 
	goto  compareNbyte_return_W_still_same

	movlw 1 
	CLRZ
	goto compareNbyte_return_W_exit

compareNbyte_return_W_still_same
	;check if more byte to be checked
	decfsz n_bytes,f 
	goto  compareNbyte_return_W_loop

	clrw ;same return W
	SETZ
compareNbyte_return_W_exit
	endm 
;macro usage:
; on array we have 8 bytes 4 times and we need to know if data given in address address_data_searched pattern already exists on array
mem_search_data_on_array macro array_start_address_literal, array_h, array_l, end_array, number_of_bytes_compared, address_data_searched_h, address_data_searched_l, temp1, temp2,  return_counter
	LOCAL mem_search_data_on_array_loop, mem_search_data_on_array_exit, mem_search_data_on_array_loop_not_found, mem_search_data_on_array_loop_1
	clrf return_counter
	movlw  LOW array_start_address_literal
	movwf  array_l 
	BANKISEL array_start_address_literal
	clrw 
	btfsc STATUS,IRP 
	movlw 1 
	movwf  array_h

mem_search_data_on_array_loop
	movlw number_of_bytes_compared
	movwf temp1
	compareNbyte_return_W  array_h, array_l, address_data_searched_h, address_data_searched_l, temp1, temp2
	SKPZ ;if return 0 means - data are same 
	goto mem_search_data_on_array_loop_not_found
	BANKSEL return_counter
	incf return_counter,f
	movlw number_of_bytes_compared
	addwf array_l,f
	goto mem_search_data_on_array_loop_1

mem_search_data_on_array_loop_not_found
	incf array_l,f ; here increase by 1 or by + number_of_bytes_compared in case of success found

mem_search_data_on_array_loop_1
	movf  end_array,w 
	xorwf array_l,w 
	BNZ   mem_search_data_on_array_loop

	;finish comparision - if we got here - 
mem_search_data_on_array_exit 	
	movf return_counter,w 
	endm

compare_two_registers_returns_minus_1  equ 0xfe 



;compare reg1 with reg2 
;if reg1 == req2  returns 0
;if reg1 > reg2 returns 1
;if reg1 < reg2 returns 0xfe
compare_two_registers  macro  reg1, reg2 
	LOCAL compare_two_registers_equal, compare_two_registers_exit
	BANKSEL reg2
	movf reg2,w 
	BANKSEL reg1
	subwf reg1,w  ; reg1 - reg2
	BZ compare_two_registers_equal
	BC compare_two_registers_greater ;reg1 > reg2 
	;reg1< reg2 
	movlw compare_two_registers_returns_minus_1
	CLRZ 
	b compare_two_registers_exit

compare_two_registers_greater
	movlw 1 
	CLRZ 
	b compare_two_registers_exit	 

compare_two_registers_equal
	clrw 
	SETZ
compare_two_registers_exit
	endm 

;this will copy address two variables
;high address is represented by 0 or 1 - like IRP bit
;to be used by FSR and INDF
store_address_of_variable_irp  macro  var_address, address_l
	movlw LOW var_address
	movwf address_l

	BANKISEL var_address
	clrw 
	btfsc STATUS,IRP 
	movlw 1 
	movwf address_l+1
	endm 


copy_data_from_ROM macro str_ptr, str_ram , string_label
	LOCAL COPY_LOOP, COPY_LOOP_exit
;=============================================
; Copy null-terminated string from ROM to RAM
; Input: W = low byte of ROM address
;        PCLATH = high byte of ROM address
; Output: str_ram contains the string
; =============================================
	BANKSEL PCLATH
	movlw  HIGH string_label
	movwf PCLATH

    clrf   str_ptr        ; Save ROM pointer

	BANKISEL str_ram
    movlw   LOW str_ram        ; Destination address
    movwf   FSR          ; Use FSR as destination pointer
    
COPY_LOOP
    movf    str_ptr,W     ; Get current ROM address
    incf    str_ptr,F     ; Increment for next byte
	PAGESEL string_label
    call    string_label   ; Read byte from ROM
    
	PAGESEL COPY_LOOP_exit
    movwf   INDF          ; Store in RAM
	movf INDF,w
	SKPNZ 
	goto COPY_LOOP_exit
	
    incf    FSR,F       ; Next RAM location
    goto    COPY_LOOP
COPY_LOOP_exit  
    endm