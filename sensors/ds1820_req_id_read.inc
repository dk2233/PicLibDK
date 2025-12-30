
;can only be used when only one ds connected
ds18_req_id_read
        BANKSEL  ds18_number_of_receive_bytes
         movlw    8
         movwf    ds18_number_of_receive_bytes
         disable_all_isr 
         call     init_ds1820
         
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         retlw  1
         
         BANKSEL ds18_command_to_be_send
         movlw   DS18b20_read_rom
         movwf   ds18_command_to_be_send

         call     ds18_send_request_loop_send_byte_start


         BANKISEL  ds18_read_from_RAM
         movlw     LOW ds18_read_from_RAM
         movwf     FSR
         
         call     ds18_receive_data_from_sensor
         
         enable_all_isr
         ;sprawdz CRC
         BANKISEL  ds18_read_from_RAM
         movlw  LOW ds18_read_from_RAM
         movwf  FSR     
         BANKSEL  n_bit   
         ;8 bajt to CRC
         movlw    8
         movwf    n_bit
         
         call     check_CRC_DS
         BANKSEL ds18_CRC_result
         movf     ds18_CRC_result,w
         ;jezeli 0 to jest ok
         btfsc    STATUS,Z         
         bcf ds18_status, ds18_crc_fault

         ;show crc fault
         btfss    STATUS,Z         
         bsf ds18_status, ds18_crc_fault
         
         retlw 0