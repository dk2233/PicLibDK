; memory allocation
; sp  
; stack_array
;stack_array+1
;stack_array+2
;stack_array+3
;...


STACK_VALUE_NOT_FOUND   equ  0xff

m_push_to_stack macro  stack_array, sp , variable_with_value
     BANKISEL stack_array
     BANKSEL sp
     if (variable_with_value == 0) 
     movwf stack_array ; stack array has one empty space just in case as temporary reg
     endif
     incf sp,f
     movlw LOW stack_array
     addwf sp,w 
     movwf FSR
     if (variable_with_value != 0) 
     BANKSEL variable_with_value
     movf variable_with_value,w 
     else 
     movf stack_array,w
     endif
     movwf INDF
     endm

;popped value will be place in Wreg
m_pop_stack macro stack_array, sp 

     BANKSEL stack_array

     movlw LOW stack_array
     addwf sp,w 
     BANKISEL stack_array
     movwf FSR 
     
     decf sp,f 
     movf INDF,w 
     clrf INDF

     endm

;check if specific value is already on stack - find  first appear of this value     
m_find_value_position_on_stack macro stack_array, sp, variable_with_value, temp
     LOCAL  sp_find_value_loop, sp_find_value_loop_exit, sp_find_value_loop_found , sp_find_value_not_found
     BANKSEL temp
     movlw LOW stack_array 
     movwf FSR 
     clrf  temp

     BANKISEL stack_array
sp_find_value_loop
     movf   INDF,w
     BANKSEL variable_with_value
     xorwf  variable_with_value,w
     SKPNZ 
     goto sp_find_value_loop_found

     BANKSEL temp
     movf temp,w 
     BANKSEL sp
     xorwf sp,w 
     SKPNZ 
     goto sp_find_value_not_found   
     incf temp,f
     incf FSR,f 

     goto  sp_find_value_loop


sp_find_value_loop_found     
     BANKSEL temp
     movf  temp,w 
     goto  sp_find_value_loop_exit

sp_find_value_not_found
     movlw STACK_VALUE_NOT_FOUND 

sp_find_value_loop_exit
     endm
