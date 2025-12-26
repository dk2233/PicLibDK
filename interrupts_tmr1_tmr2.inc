    

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
