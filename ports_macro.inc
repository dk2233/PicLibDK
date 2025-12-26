
configure_ports_16f  macro  port_reg, tris_state 
        BANKSEL  port_reg
        clrf     port_reg
        ;change to bank1 to change tris for specific port 
        bsf      STATUS,RP0

        movlw    tris_state
        movwf    port_reg
        endm
        
disable_pull_up  macro 
        BANKSEL OPTION_REG
        bsf OPTION_REG,NOT_RBPU
        endm

enable_pull_up  macro value_for_wpub
       BANKSEL OPTION_REG
        bcf OPTION_REG,NOT_RBPU

        movlw value_for_wpub
        movwf WPUB
        endm