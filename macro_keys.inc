    ; State definitions
    #define STATE_IDLE     0
    #define STATE_DEBOUNCE 1
    #define STATE_PRESSED  2
    #define STATE_RELEASED 3 
    #define STATE_DEBOUNCE_RELEASED 4
;
;reaction on key pressed 
;debounce time 
;acknowledge after debounce time 
;but also another possibility - key release + debounce release time will set that key is pressed
;also there is a information how long key was pressed
;  is key pressed or not
;  time key was pressed
button_handler macro key_port, key_pin, button_state, button_press_flag, key_pressed_bit,  debounce_counter, debounce_time, reg_pressed_countL
;, register_after_release
    local not_first_press, button_high, not_release, button_handler_end
    ; Read button
    BANKSEL key_port
    BTFSC   key_port, key_pin
    GOTO    button_high
    
    BANKSEL button_state
    ; Button low (pressed)
    MOVF    button_state, W
    XORLW   STATE_IDLE
    SKPZ
    GOTO    not_first_press
    
    ; First detection - start debounce
    MOVLW   STATE_DEBOUNCE
    MOVWF   button_state
    MOVLW   debounce_time         ; 20ms debounce
    MOVWF   debounce_counter
    goto button_handler_end
    
not_first_press
    movf    button_state,w 
    xorlw   STATE_PRESSED
    SKPZ
    goto not_first_press1
    increment_16bit_value reg_pressed_countL, 1

not_first_press1 
    MOVF    button_state, W
    XORLW   STATE_DEBOUNCE
    skpz
    goto button_handler_end
    
    ; Check if debounce time elapsed
    DECFSZ  debounce_counter, F
    goto button_handler_end
    
    ; Debounce complete - register press
    MOVLW   STATE_PRESSED
    MOVWF   button_state
    ;btfsc 
    goto button_handler_end

button_high
    BANKSEL button_state
    ; Button high (released)
    MOVF    button_state, W
    XORLW   STATE_PRESSED
    SKPZ
    GOTO    not_release; if we never set state_pressed this means we observe debouncing
    ; Was pressed, now released
    MOVLW   STATE_IDLE
    MOVWF   button_state
    ;if (register_after_release == 0)
    ;bcf button_press_flag, key_pressed_bit
    ;endif
    BSF     button_press_flag, key_pressed_bit
    goto button_handler_end
    
not_release
    MOVLW   STATE_IDLE
    MOVWF   button_state
button_handler_end
    endm