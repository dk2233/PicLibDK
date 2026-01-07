config16f_analog_off macro 

;bank0
    BANKSEL  ADCON0
    ;disable a\d module
     clrf ADCON0

    BANKSEL ANSEL 
    ;all pins related to analog set as digit
     clrf   ANSEL
     ;cvref  off
     bcf    VRCON,VREN 
    
    endm



config16f_ansel    macro ansel_bits
        BANKSEL ANSEL 
        movlw ansel_bits
        movwf ANSEL 
        endm

config16f_analog_on  macro adfm_bit, v_ref0_bit, v_ref1_bit, clock_conversion_bits
        local x = adfm_bit << ADFM
        local x1 = clock_conversion_bits << ADCS0
ifdef __16F913 
x = x | (v_ref0_bit << VCFG0) 
x = x | (v_ref1_bit << VCFG1)
        BANKSEL ADCON1 
        movlw x1 
        movwf ADCON1
endif
        BANKSEL ADCON0
        movlw x 
        movwf ADCON0

        bsf ADCON0, ADON 

        endm

run_analog_conversion macro channel, mcu_freq, tmp_reg, resultL
        local adcon = channel << CHS0 
        local run_analog_conversion_loop
        BANKSEL ADCON0
        ;wait 5 us before conversion could be started
        movlw adcon 
        iorwf ADCON0,f 

        wait_specific_time_no_tmr_us mcu_freq, .20, tmp_reg 

        BANKSEL ADCON0
        bsf ADCON0, GO_DONE
run_analog_conversion_loop
        btfsc ADCON0, GO_DONE
        goto $-1

        BANKSEL ADRESL
        movf ADRESL,w 
        BANKSEL resultL
        movwf resultL
        BANKSEL ADRESH
        movf ADRESH,w 
        BANKSEL resultL
        movwf resultL+1


        
        endm 