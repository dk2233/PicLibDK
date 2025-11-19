;initialization for pic16fxxx
; supports:
; pic16f877
; pic16f913
; pic16f873




configure_ports_16f  macro  port_reg, tris_state 
        BANKSEL  port_reg
        clrf     port_reg
        ;change to bank1 to change tris for specific port 
        bsf      STATUS,RP0

        movlw    tris_state
        movwf    port_reg
        endm


config_lcd_off  macro 
   
       ;disable LCD module 
        BANKSEL  LCDCON
        clrf LCDCON

;all segments pins set as digital pins 
	clrf	LCDSE0
	clrf	LCDSE1

    #IFDEF   __16F914 
	clrf	LCDSE2
    #ENDIF
    #IFDEF   __16F917 
	clrf	LCDSE2
    #ENDIF
        endm

config_analog_off macro 

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

config_comparator_off  macro
        BANKSEL CMCON0
    ;comparator module off
	movlw	b'00000111'
	movwf	CMCON0
        endm

config_ccp_off macro 
        BANKSEL CCP1CON
         clrf CCP1CON

    #IFDEF   __16F914
         clrf CCP2CON
    #ENDIF
    #IFDEF   __16F917
         clrf CCP2CON
    #ENDIF

        endm

;macro to help 
;change all pins related to analog
;to use them as normal digit I/O
config_porta_digit_16f  macro 
        config_lcd_off

        config_analog_off

        config_comparator_off

        endm


;wdg configuration macro
;if wdg rate is given 15 -
;means 2**15 is set as wdg prescaler rate
config_watchdog   macro prescaler_rate , is_on
;keep timeout value in ms in 
        local wdt_time = 32
        BANKSEL WDTCON 
        LOCAL x = is_on 
        LOCAL r = prescaler_rate - 5

        movlw x | (r << 1)
        movwf WDTCON

        WHILE r > 0
        local wdt_time = wdt_time*2
        LOCAL r =  r-1
        ENDW 

wdt_timeout equ wdt_time
test_var equ wdt_time
        

        endm


config_osccon   macro  freq_bits, external_clock_startup, clock_select
        LOCAL bits = freq_bits << IRCF0 
bits = bits | (external_clock_startup << OSTS)

bits = bits | (b'11' << LTS )
bits = bits | (clock_select << SCS)
        BANKSEL OSCCON
        movlw bits 
        movwf OSCCON

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
  