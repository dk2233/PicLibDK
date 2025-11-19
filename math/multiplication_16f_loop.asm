;operation multiplication
;used while loop
;to sum 16 bit number 16 bit operand number
;16 bits number | fractionh | fractionl* 16 bits operand = 32 bits result
; number_h| number_l *  operandh| operandl = results01| result001
; result 
;fraction represents 1/256**2 = 1.52587890625e-05

mul16f 
         clrf     result_ll
         clrf     result_lh
         clrf     result_hl
         clrf     result_H
         clrf     result_01
         clrf     result_001
mul16f_loop
         ; check if operandl is not 0 
         movf     operandl,w
         btfss    STATUS,Z
         goto     mul16f_l2

         movf     operandh,w
         btfsc    STATUS,Z
         return ;operand is 0
         
mul16f_l2    
        movlw    1
         subwf     operandl,f 
         SKPNC ;no C means we cross 0
         goto      mul16f_l22
         ;if operandl was 0 and was overflow -> operandh has to be > 0
         decf    operandh,f

mul16f_l22
         movf     fraction_l,w         
         addwf    result_001,f
         btfsc    STATUS,C
        call mul16f_check_cross
         
         movf     fraction_h,w         
         addwf    result_01,f
         btfsc    STATUS,C
        call mul16f_check_cross_ll
         
         movf     number_l,w
         addwf    result_ll,f
         btfsc    STATUS,C
         call     mul16f_check_cross_lh

         movf     number_h ,w
         addwf    result_lh,f
         btfsc    STATUS,C
         call      mul16f_check_cross_hl      
         
         goto     mul16f_loop
         
mul16f_check_cross

         incf     result_01,f
         btfss    STATUS,Z 
         return 

mul16f_check_cross_ll
         incf    result_ll,f 
         btfss   STATUS,Z 
         return 

mul16f_check_cross_lh
         incf     result_lh,f
         btfss    STATUS,Z 
         return 

mul16f_check_cross_hl
         incf     result_hl,f
         btfss  STATUS,Z 
         return 

         incf    result_H,f 
         return