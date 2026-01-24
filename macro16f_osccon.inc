
osccon_internal_31khz equ 0 
osccon_internal_1MHZ equ b'100' 
osccon_internal_2MHZ equ b'101' 
osccon_internal_4MHZ equ b'110' 
osccon_internal_8MHZ equ b'111' 
osts_external_clock_startup  equ 1
osts_internal_clock_startup  equ 0
scs_internal_clock  equ 1
scs_fosc_clock  equ 0


config_osccon   macro  freq_bits, external_clock_startup, clock_select
        LOCAL bits = freq_bits << IRCF0 
bits = bits | (external_clock_startup << OSTS)

bits = bits | (b'11' << LTS )
bits = bits | (clock_select << SCS)
        BANKSEL OSCCON
        movlw bits 
        movwf OSCCON

        endm