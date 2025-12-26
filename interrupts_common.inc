    
context_store16f  macro 
    movwf w_temp
    swapf STATUS,W
    clrf STATUS
    movwf status_temp
    movf  PCLATH,w
    movwf pclath_temp
    movf  FSR,w 
    movwf fsr_temp
    endm


context_restore16f macro
    movf  pclath_temp,w 
    movwf PCLATH
    movf  fsr_temp,w 
    movwf FSR 
    swapf status_temp,w
    movwf STATUS
    swapf w_temp,f
    swapf w_temp,w
    endm

check_tmr0_isr macro func_to_process_tmr0 
    LOCAL check_tmr0_isr_exit

    btfss INTCON,TMR0IE
    goto  check_tmr0_isr_exit

    btfsc INTCON,TMR0IF
    goto func_to_process_tmr0

check_tmr0_isr_exit
    endm

disable_all_isr macro 
    bcf     INTCON,GIE 
    endm

enable_all_isr  macro 
    bsf     INTCON,GIE 
    endm
