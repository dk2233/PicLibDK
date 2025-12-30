
;in FSR place first byte of read scratchpad
; 
ds18_check_resolution
        movlw  DS18B20_offset_of_config_byte
        addwf  FSR,F
        movf   INDF,w 
        movwf  operandl ; common gpr

        movlw  DS18B20_config_R0 
        BANKSEL n_bit
        movwf  n_bit 
        shift_byte_n_times shift_byte_right, operandl,  n_bit 

        movlw  DS18B20_low_resolution
        addwf  operandl,W


        return   

ds18_set_resolution macro temp_var 
        BANKSEL temp_var

        movlw  DS18B20_low_resolution
        subwf  ds18_expected_resolution,w
        movwf temp_var
        movlw  DS18B20_config_R0  ;how much move bits
        movwf  n_bit 
        shift_byte_n_times shift_byte_left, temp_var, n_bit

        movlw  LOW ds18_read_from_RAM
        addlw  DS18B20_offset_of_config_byte
        movwf  FSR 
        BANKISEL ds18_read_from_RAM
        movf   INDF,w
        andlw  DS18B20_resolution_mask
        BANKSEL ds18_config_byte
        movwf  ds18_config_byte

        movf    temp_var,w ; 
        addwf   ds18_config_byte,f
        endm