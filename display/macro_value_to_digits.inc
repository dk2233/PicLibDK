; it is enough to give valueL, valueL+1 has higher byte
;digit1    is to store value how many 1
;digit10   is expected to be in address digit1+1
;digit100  is expected to be in address digit1+2
;digit1000 is expected to be in address digit1+3
macro_16bits_into_4dec macro   valueL, digit1 
    BANKSEL digit1
    move_2bytes_into_2bytes  valueL, result_ll
    clrf  result_hl
    movel_2bytes  .1000, operandl
    call func_div_24bit_16bit
    movf    result_ll,W
    BANKSEL digit1
    movwf   digit1+3

    
    move_2bytes_into_2bytes fraction_l, result_ll 
    clrf result_hl
    movel_2bytes .100, operandl 
    call func_div_24bit_16bit 
    movf result_ll,w 
    BANKSEL digit1
    movwf  digit1+2

    move_2bytes_into_2bytes fraction_l, result_ll
    clrf result_hl
    movel_2bytes .10, operandl
    call func_div_24bit_16bit
    movf  result_ll,w
    BANKSEL digit1
    movwf digit1+1

    movf fraction_l,w 
    movwf digit1

    endm 


; it is enough to give valueL, valueL+1 has higher byte
;digit1    is to store value how many 1
;digit10   is expected to be in address digit1+1
;digit100  is expected to be in address digit1+2
;digit1000 is expected to be in address digit1+3
;first divider should depends on how many digits we have 
;here larger than 5 digits is not supported
;if 5 digits = then 10000
;if 4 digits = then 1000
;if 3 digis = 100 
;if 2 digits = 10 
macro_16bits_into_N_dec macro   valueL, digit1, number_of_digits 
    LOCAL n_digit = number_of_digits - 1, x = number_of_digits, first_divider = 1
first_divider = 1
    while ( x > 1 )
first_divider = first_divider * .10
x = x - 1
    endw
    LOCAL  divider = first_divider
    BANKSEL digit1
    move_2bytes_into_2bytes  valueL, result_ll
    clrf  result_hl
    movel_2bytes  first_divider, operandl
    call func_div_24bit_16bit
    movf    result_ll,W
    BANKSEL digit1
    movwf   digit1+n_digit

    while (n_digit > 1)
divider = divider / .10
    move_2bytes_into_2bytes fraction_l, result_ll 
    clrf result_hl
    movel_2bytes  divider, operandl 
    call func_div_24bit_16bit 
    movf result_ll,w 
    BANKSEL digit1
n_digit = n_digit - 1
    movwf  digit1+n_digit
    endw

    movf fraction_l,w 
    movwf digit1

    endm 
