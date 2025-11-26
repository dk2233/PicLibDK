wdg_reset_reason  equ   1

;if return value in WREG is 1 - wdg reset is reset reason
detect_watchdog_happens  macro
    LOCAL detect_watchdog_happens_0, detect_watchdog_happens_1
    BANKSEL PCON 

    movlw 0
    btfss  PCON,NOT_POR
    goto  detect_watchdog_happens_0

    btfss  PCON,SBOREN 
    goto  detect_watchdog_happens_1

    btfss  PCON,NOT_BOR
    goto  detect_watchdog_happens_0
detect_watchdog_happens_1
    movlw wdg_reset_reason
detect_watchdog_happens_0
    endm    
