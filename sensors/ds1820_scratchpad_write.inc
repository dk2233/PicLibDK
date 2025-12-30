     
;DS18B20
;3 bytes have to be put into arguments
ds18_req_scratchpad_write
        
         call     init_ds1820
         
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         retlw  1
         disable_all_isr 
         call ds18_match_or_skip_rom
         
         movlw    DS18b20_write_scratchpad
         call     ds18_send_request_loop_send_byte_start

         BANKISEL  ds18_TH
         movlw     ds18_TH 
         movwf     FSR     

ds18_req_scratchpad_write_loop
        movf  INDF,w 
        call     ds18_send_request_loop_send_byte_start


        movf  FSR,W
        xorlw ds18_config_byte
        SKPNZ ;do we reached all 3 bytes to be write?
        goto ds18_req_scratchpad_write_exit

        incf  FSR,f 
        goto ds18_req_scratchpad_write_loop

ds18_req_scratchpad_write_exit
        enable_all_isr
        ;means fine
        retlw   0

