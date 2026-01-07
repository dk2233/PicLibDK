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