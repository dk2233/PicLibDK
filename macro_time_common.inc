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




        