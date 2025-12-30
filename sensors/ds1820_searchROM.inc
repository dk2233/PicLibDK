

;in operandl put state of bit to be set in ds18b20 id
;in FSR id byte to be set
ds18b20_set_id_bit_for_sensor
        ;movf  ds18_number_of_sensors
        ;movwf operandl 

        BANKISEL   ds18_read_id_1
        movlw LOW  ds18_read_id_1
        BANKSEL   ds18_id_which_byte
        addwf      ds18_id_which_byte,w
        addwf      ds18_id_RAM_offset,w 
        movwf      FSR         

ds18b20_set_id_bit_for_all_sensors_loop
        bcf    STATUS,C
        movf   operandl,w ;if 0 than unset bit
        SKPZ
        bsf    STATUS,C
        rrf    INDF,f 

        ;movlw  8 
        ;addwf FSR,f ;next sensor id same bit

        ;decfsz  operandl,f
        ;goto  ds18b20_set_id_bit_for_all_sensors_loop
        return
ds18_search_rom_all_sensors

        BANKSEL ds18_number_of_sensors
        clrf  ds18_number_of_sensors

         disable_all_isr 
ds18_search_rom_all_sensors_loop         
        call ds18_search_rom 
        movwf result_ll
        ;increment sensors number if return is 0 
        movf result_ll,w 
        SKPZ 
        goto ds18_search_rom_all_sensors_exit


        BANKSEL  ds18_number_of_receive_bytes
         movlw    8
         movwf    ds18_number_of_receive_bytes
         movwf    ds18_id_bit
         clrf     ds18_id_which_byte

        ;check if last received id is not already detected previously
        ;but we need to have at least 2 of them
        movlw   .8
        movwf  operandl
        compare_two_registers ds18_id_RAM_offset, operandl
        movwf result_H
        BZ    ds18_search_rom_all_sensors_search_same
        xorlw  compare_two_registers_returns_minus_1 ;means offset < 16
        BZ ds18_search_rom_all_sensors_skip_searching
ds18_search_rom_all_sensors_search_same
        BANKSEL ds18_id_RAM_offset
        movf ds18_id_RAM_offset,w 
        movwf operandl
        movlw LOW ds18_read_id_1
        addwf operandl,f

        BANKISEL ds18_read_id_1
        clrw 
        btfsc STATUS,IRP
        movlw 1 
        movwf fraction_h
        movlw LOW ds18_read_id_1
        addwf ds18_id_RAM_offset,w 
        movwf fraction_l

        mem_search_data_on_array  ds18_read_id_1, number_h, number_l, operandl, DS18B20_id_size, fraction_h, fraction_l, result_01, result_001, result_ll
        movwf result_ll
        movf result_ll,w
        BNZ ds18_search_rom_all_sensors_overwrite 
          

ds18_search_rom_all_sensors_skip_searching
        movlw  DS18B20_id_size
        BANKSEL ds18_id_RAM_offset
        addwf  ds18_id_RAM_offset,f
        incf  ds18_number_of_sensors,f 

ds18_search_rom_all_sensors_overwrite        
        BANKSEL stack_sp
        ;check if anything is still on stack differences
        movf  stack_sp,w 
        SKPZ
        goto ds18_search_rom_all_sensors_loop
        ;check if no more bit was in conflicts
ds18_search_rom_all_sensors_exit
        enable_all_isr
        return 


;this will search for id in rom of every sensors on the same line
;it will read following possibilities:
; 01 - means read bit of every connected sensors is 0
; 10 - means n bit is 1 of every sensors
; 11 - this is not possible - no device attached
; 00 - this means connected sensors have conflicts on this bit position
ds18_search_rom
        BANKSEL ds18_received_id_bit_count
        clrf  ds18_received_id_bit_count
         call     init_ds1820
         
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         goto ds18_search_rom_NOTOK_return
         ;error no init finished properly
         
         BANKSEL ds18_command_to_be_send
         movlw   DS18b20_search_rom
         call     ds18_send_request_loop_send_byte_start


        BANKSEL  ds18_number_of_receive_bytes
         movlw    8
         movwf    ds18_number_of_receive_bytes
         movwf    ds18_id_bit
         clrf     ds18_id_which_byte

;loop to read data
         BANKSEL  ds18_status
         bsf   ds18_status, ds18_receive_byte_from_lsb
ds18_search_rom_loop_1_bit
         BANKISEL  ds18_read_from_RAM
         movlw     LOW ds18_read_from_RAM
         movwf     FSR
         clrf  INDF
         movlw    2
         movwf    n_bit  
         call     ds18_receive_one_bit

        BANKSEL ds18_received_id_bit_count
         incf ds18_received_id_bit_count,f
         movlw  b'00000011'
         xorwf  INDF,w 
         SKPNZ
         ;retlw  1 ; no ds responds with any bit - if first bit is 1 second should be complement 0
         goto  ds18_search_rom_NOTOK_return
         ;now check result 
         movlw  b'00000001' ; set bit 1 - this is 01 case
         xorwf  INDF,w
         SKPZ  
         goto  ds18_search_rom_loop_1_bit_1
         clrf  operandl
         call ds18b20_set_id_bit_for_sensor
         call   send_bit_0

         goto ds18_search_rom_loop_1_bit_next

         ;for how many ds sensors we have - all this bit set as 0

ds18_search_rom_loop_1_bit_1
         movlw  b'00000010'
         xorwf  INDF,w
         SKPZ 
         goto ds18_search_rom_loop_1_bit_2
         movlw 1 
         movwf operandl
         call ds18b20_set_id_bit_for_sensor
         call send_bit_1  

         ;for how many ds sensors we have - all this bit set as 1

         goto ds18_search_rom_loop_1_bit_next

ds18_search_rom_loop_1_bit_2
         movlw  b'00000000' ;first bit is 0 that is becuase different sensors have 0 or 1 at this position - we do not know how many sensors we have
         xorwf  INDF,w 
         SKPZ 
         goto ds18_search_rom_loop_1_bit_2
         ;check if this bit was already set which means 0 was set to this one previously - now set 1
         m_find_value_position_on_stack stack_of_differences, stack_sp, ds18_received_id_bit_count, operandl

         ;check what is in Wreg 
         movwf result_ll 
         xorlw STACK_VALUE_NOT_FOUND
         SKPNZ 
         goto ds18_search_rom_loop_00_push_to_stack

         ;and if this last one result == sp
         movf  result_ll, w 
         BANKSEL stack_sp
         xorwf stack_sp, w 
         SKPNZ
         goto ds18_search_rom_loop_00_last_bit_number_on_stack_no_more_send_1 
         
         ;send 0 

         clrf operandl
         call ds18b20_set_id_bit_for_sensor
         call send_bit_0
         goto ds18_search_rom_loop_1_bit_next

         ;check if this is the last conflct on stack 

ds18_search_rom_loop_00_last_bit_number_on_stack_no_more_send_1
        movlw  1
         movwf operandl
         call ds18b20_set_id_bit_for_sensor
         call send_bit_1  
         ;no more possibilities on this bit - remove from stack - 
         ;but it is possible after this bit there will be another one conflicting - so we will need this to return to this pattern
         ; pattern is the following  0 - 1 - new conflict - so 1 here should be repeated
         ;we need to revert this removal
         m_pop_stack stack_of_differences, stack_sp
         goto ds18_search_rom_loop_1_bit_next

ds18_search_rom_loop_00_push_to_stack
         ;if not exist on stack - push it
         m_push_to_stack stack_of_differences, stack_sp, ds18_received_id_bit_count
         ;and send  
         clrf operandl
         call ds18b20_set_id_bit_for_sensor
         call send_bit_0
         

         goto    ds18_search_rom_loop_1_bit_next

         
ds18_search_rom_loop_1_bit_next

        BANKSEL ds18_id_bit
        decfsz   ds18_id_bit,f
        goto  ds18_search_rom_loop_1_bit
         
        movlw  8
        movwf  ds18_id_bit
        incf  ds18_id_which_byte,f 
     
        decfsz ds18_number_of_receive_bytes,f
        goto ds18_search_rom_loop_1_bit 

        goto  ds18_search_rom_OK_return

        ;run another iteration


ds18_search_rom_OK_return
         BANKSEL ds18_status
         bcf   ds18_status, ds18_receive_byte_from_lsb

        retlw 0

ds18_search_rom_NOTOK_return
         BANKSEL ds18_status
         bcf   ds18_status, ds18_receive_byte_from_lsb

        retlw 1