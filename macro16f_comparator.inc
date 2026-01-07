
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