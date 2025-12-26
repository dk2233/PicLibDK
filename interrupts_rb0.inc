    
check_rb0_int   macro func_to_process_rb0

    btfsc INTCON,INTF 
    goto func_to_process_rb0
    endm 

rb0_int_enable macro 
    bcf INTCON, INTF
    bsf INTCON, INTE
    endm 

rb0_int_disable macro 
    bcf INTCON, INTE
    bcf INTCON, INTF
    endm 