    
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


check_tmr1_isr macro  func_to_process_tmr1
    LOCAL check_tmr1_isr_exit
    BANKSEL PIE1

    btfss PIE1,TMR1IE
    goto check_tmr1_isr_exit

    BANKSEL PIR1
    btfsc PIR1,TMR1IF
    goto  func_to_process_tmr1

check_tmr1_isr_exit
    endm

check_tmr2_isr macro  func_to_process_tmr2
    LOCAL check_tmr2_isr_exit
    BANKSEL PIE1

    btfss PIE1,TMR2IE
    goto check_tmr2_isr_exit

    BANKSEL PIR1
    btfsc PIR1,TMR2IF
    goto  func_to_process_tmr2

check_tmr2_isr_exit
    endm