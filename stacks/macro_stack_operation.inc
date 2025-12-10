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

     incf sp,f
     movlw LOW stack_array
     addwf sp,w 
     movwf FSR 
     BANKSEL variable_with_value
     movf variable_with_value,w 
     movwf INDF
     endm


m_pop_stack macro stack_array, sp 

     BANKISEL stack_array
     BANKSEL stack_array

     movlw LOW stack_array
     addwf sp,w 
     movwf FSR 
     
     decf sp,f 
     movf INDF,w 
     clrf INDF

     endm

;check if specific value is already on stack - find  first appear of this value     
m_find_value_position_on_stack macro stack_array, sp, variable_with_value, temp
     LOCAL  sp_find_value_loop, sp_find_value_loop_exit, sp_find_value_loop_found , sp_find_value_not_found
     BANKISEL stack_array
     BANKSEL temp
     movlw LOW stack_array 
     movwf FSR 
     clrf  temp

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
     movf  temp,w 
     goto  sp_find_value_loop_exit

sp_find_value_not_found
     movlw STACK_VALUE_NOT_FOUND 

sp_find_value_loop_exit
     endm
