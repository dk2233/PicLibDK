;initialization for pic16fxxx
; supports:
; pic16f877
; pic16f913
; pic16f873





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
        IFDEF CMCON0
        LOCAL cmc_reg_name = CMCON0
        ELSE 
        LOCAL cmc_reg_name = CMCON
        ENDIF 
        BANKSEL cmc_reg_name
    ;comparator module off
	movlw	b'00000111'
	movwf	cmc_reg_name
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

