;to calculate tmr0 period
;4/freq * 256 * prescaler (if assigned to tmr0)
configure_tmr0   macro  prescaler_bits , internal_clock 
        LOCAL option_bits = 0
        if (prescaler_bits != 0)
option_bits = prescaler_bits 
option_bits = option_bits | (0 << PSA) ; PSA on 
        else
        VARIABLE   tmr0_period = tmr0_time
        endif

        if (internal_clock == 0)
option_bits = option_bits | (1 << T0CS)

        endif
        BANKSEL OPTION_REG
        movlw   b'11010000'
        andwf    OPTION_REG,w

        addlw   option_bits 
        movwf   OPTION_REG
        endm



configure_tmr2 macro prescaler_bits, postscaler_bits, period_reg
     BANKSEL  T2CON 
     LOCAL  timer2_con = prescaler_bits
    LOCAL  a = postscaler_bits << TOUTPS0
    movlw postscaler_bits
    movlw a
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