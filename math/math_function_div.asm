;in number_h|number_l place number to be dividend

;in operandl - number that will be divisor
;result - quotient in
; resultlh | resultll 

;reminder in number_l 


func_div_16bit_8bit 
    BANKSEL result_ll
    macro_division_16f  number_h, number_l, operandl, result_lh, result_ll 
    return

func_div_24bit_16bit 
    BANKSEL result_hl
    macro_division_16f_24bit_16bit  result_hl, result_lh, result_ll, operandh, operandl, fraction_h, fraction_l, result_001 
    
    return