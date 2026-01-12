    ; State definitions
    #define STATE_IDLE     0
    #define STATE_DEBOUNCE 1
    #define STATE_PRESSED  2
    #define STATE_RELEASED 3 
    #define STATE_DEBOUNCE_RELEASED 4

;key_pressed flags    
key_pressed_bit   equ  0
key_pressed_use   equ  1
key_pressed_released  equ 2

;
;reaction on key pressed 
;debounce time 
;acknowledge after debounce time 
;but also another possibility - key release + debounce release time will set that key is pressed
;also there is a information how long key was pressed
;it is given in format of counted calls of this procedure
; - is key pressed or not - flag key_pressed_bit - set after debounce time pass after key press
; - time key was pressed
; - is key released - flag key_pressed_released - after debounce time


;
;        debunce   how  is pressed
;           time   long     
;          |-----|---------------------| release
;          |                           | debounce
;----------|                           |----------|------- 
button_handler macro key_port, key_pin, button_state, button_press_flag, key_pressed_bit,  debounce_counter, debounce_time, reg_pressed_countL
;, register_after_release
    local not_first_press, not_first_press1, button_high, not_release, button_handler_end
    movf button_state,w 
    xorlw STATE_DEBOUNCE_RELEASED
    SKPNZ
    goto button_handler_check_debounce_release  
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
    clrf button_press_flag
    clrf reg_pressed_countL
    clrf reg_pressed_countL+1
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
    BSF     button_press_flag, key_pressed_bit
    bsf     button_press_flag, key_pressed_use
    ;btfsc 
    goto button_handler_end

button_high
    BANKSEL button_state
    ; Button high (released)
    MOVF    button_state, W
    XORLW   STATE_PRESSED
    SKPZ
    GOTO    not_release; if we never set state_pressed this means we observe debouncing
    ; Was pressed, now released debounce state
    MOVLW   STATE_DEBOUNCE_RELEASED
    MOVWF   button_state
    MOVLW   debounce_time         ; 20ms debounce
    MOVWF   debounce_counter
    ;if (register_after_release == 0)
    ;bcf button_press_flag, key_pressed_bit
    ;endif
    goto button_handler_end

button_handler_check_debounce_release
    tstf   debounce_counter
    SKPNZ 
    goto button_handler_check_debounce_release2
    decfsz debounce_counter,f 
    goto button_handler_end

button_handler_check_debounce_release2
    BANKSEL key_port
    BTFSS   key_port, key_pin
    GOTO    button_handler_end
    bcf button_press_flag, key_pressed_bit
    bcf button_press_flag, key_pressed_use
    bsf button_press_flag, key_pressed_released

not_release
    MOVLW   STATE_IDLE
    MOVWF   button_state
button_handler_end
    endm