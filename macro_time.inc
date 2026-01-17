;to calculate tmr0 period
;4/freq * 256 * prescaler (if assigned to tmr0)
configure_tmr0   macro  prescaler_bits , internal_clock 
        LOCAL option_bits = 0
        if (prescaler_bits != 0)
option_bits = prescaler_bits 
option_bits = option_bits | (0 << PSA) ; PSA on 
        endif

        if (internal_clock == 0)
option_bits = option_bits | (1 << T0CS)

        endif
        BANKSEL OPTION_REG
        movf    OPTION_REG,w
        andlw   b'11010000'

        addlw   option_bits 
        movwf   OPTION_REG
        endm

tmr0_interrupt_enable macro

        bsf INTCON, TMR0IE
        endm

tmr0_interrupt_disable macro

        bcf INTCON, TMR0IE
        endm

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

;good for short time period = simple while loop - no timer needed
;run specific number of time
;you need to give an register that will be used to count loop pass
wait_specific_time_no_tmr_us  macro  mcu_freq , time_to_wait_us, reg 
        LOCAL  how_many_cycle_one_loop = .4
        LOCAL  how_many_wait_loop = .4 * .1000 / how_many_cycle_one_loop , nops_loop 
        LOCAL  freq = mcu_freq/ .1000

how_many_wait_loop = time_to_wait_us * freq/ how_many_wait_loop 

        BANKSEL reg
        if (how_many_wait_loop < .256)
        movlw  how_many_wait_loop
        movwf  reg 
        else
        movel_2bytes how_many_wait_loop/3, reg ;reg is Lower byte 
        endif

nops_loop        
if (how_many_wait_loop < .256)
        ;fill (nop), .7
        nop    ; 1 cycle
        decfsz   reg,f  ; 1 cycle
else 
        nop  ; 1 cycle
        decrement_16bit_value reg, .1 ;8- 11 cycles
endif
        SKPZ  ; 1 cycle
        goto     nops_loop ; 2 cycle
        endm

;sometimes loops have to call something therefore just prepare a loop variable
calculate_loop_number_to_reach_period_us   macro  mcu_freq, time_to_wait_us, one_loop_cycles
        LOCAL number = .4 * .1000000
number = time_to_wait_us * mcu_freq / number / one_loop_cycles         
        movlw number
        endm

wait_until_tmr2_period_pass  macro 
        LOCAL  loop_wait_for_PIR_mark

        BANKSEL PIR1
loop_wait_for_PIR_mark
        btfss   PIR1,TMR2IF
        goto    loop_wait_for_PIR_mark
        endm



        