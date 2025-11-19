    ;include ../../libs/math_macros.inc 

;call this 
;giving input variables
;operandl - mutiplier
;number_l - multiplied
;temp_var - required to store higher byte during shift

func_multiply8bitx8bit
    BANKSEL  temp_l
    macro_mul_16f var2, A_low, B_low, temp_lh, temp_l
    return


;call this 
;giving input variables
;operandl - mutiplier
;operandh - 
;number_l - multiplied
;number_h - required to store higher byte during shift
func_multiply16bitx16bit
;PP_ll = operandl * number_l into [15:0] bits
    BANKSEL  A_low
    movf  operandl,W
    movwf A_low
    movf  number_l,w
    movwf B_low 

    PAGESEL func_multiply8bitx8bit
    call func_multiply8bitx8bit
    
    BANKSEL temp_l
    movf temp_l,W
    movwf result_ll
    movf temp_lh,w 
    movwf result_lh

    clrf  result_hl
    clrf  result_H

    ;PP_lh = A_low * B_high     // bits [23:8]
    movf operandl,w 
    movwf A_low
    movf   number_h,w 
    movwf  B_low 
    PAGESEL func_multiply8bitx8bit
    call func_multiply8bitx8bit

    BANKSEL temp_l
    movf temp_l,W   ;add to bits [16:8] that is result_lh
    addwf result_lh,f
    btfss STATUS,C 
    goto func_multiply16bitx16bit_2

    incf result_hl,f 
    btfsc STATUS,Z 
    incf result_H,f 


func_multiply16bitx16bit_2
    movf temp_lh,w 
    addwf result_hl,f

    btfsc STATUS,C 
    incf  result_H,f
; PP_hl = A_high * B_low     // bits [23:8]  

    movf operandh,w 
    movwf A_low
    movf   number_l,w 
    movwf  B_low 
    PAGESEL func_multiply8bitx8bit
    call func_multiply8bitx8bit

    BANKSEL temp_l
    movf temp_l,W
    addwf result_lh,f
    btfss STATUS,C 
    goto func_multiply16bitx16bit_3

    incf result_hl,f 
    btfsc STATUS,Z 
    incf result_H,f 


func_multiply16bitx16bit_3
    movf temp_lh,w 
    addwf result_hl,f

    btfsc STATUS,C 
    incf  result_H,f

;PP_hh = A_high * B_high    // bits [31:16]

    movf operandh,w 
    movwf A_low
    movf   number_h,w 
    movwf  B_low 
    PAGESEL func_multiply8bitx8bit
    call func_multiply8bitx8bit

    BANKSEL temp_l
    movf temp_l,W
    addwf result_hl,f
    btfsc STATUS,C 
    incf result_H,f 

func_multiply16bitx16bit_4
    movf temp_lh,w 
    addwf result_H,f

    return


    
