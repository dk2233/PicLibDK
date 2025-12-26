
tmr1_interrupt_disable macro
        BANKSEL PIR1
        bcf PIR1,TMR1IF
        BANKSEL PIE1 
        bcf PIE1, TMR1IE
        endm 

tmr1_interrupt_enable macro
        BANKSEL PIR1
        bcf PIR1,TMR1IF
        BANKSEL PIE1 
        bsf PIE1, TMR1IE
        endm 
config_tmr1_as_timer   macro   prescaler_bit, interrupt_on 
        LOCAL bits = 1 << TMR1ON
bits = bits | (prescaler_bit << T1CKPS0 )
        movlw bits 
        BANKSEL T1CON
        movwf T1CON

        if (interrupt_on == 1)
        BANKSEL PIE1
        bsf PIE1, TMR1IE 
        endif 
        endm

configure_tmr2 macro prescaler_bits, postscaler_bits, period_reg
     BANKSEL  T2CON 
     LOCAL  timer2_con = prescaler_bits
    LOCAL  a = postscaler_bits << TOUTPS0
    movlw postscaler_bits
;here is an error in macro parsing in gpasm
;such thing will not work 
;timer2_con |= (postscaler_bits << TOUTPS0) 
timer2_con = timer2_con | (postscaler_bits << TOUTPS0) 
timer2_con |= (1 << TMR2ON)

     movlw  timer2_con
     movwf  T2CON

    BANKSEL PR2
     movlw  period_reg
     movwf  PR2

     endm

wait_until_tmr2_period_pass  macro 
        LOCAL  loop_wait_for_PIR_mark

        BANKSEL PIR1
loop_wait_for_PIR_mark
        btfss   PIR1,TMR2IF
        goto    loop_wait_for_PIR_mark
        endm