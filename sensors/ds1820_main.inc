;kucharski22daniel@gmail.com  2025
;to use this library
;also use of math macros:
;memory_operation_16f.inc
;math_hex_to_dec_1000.inc 
;math_function_div.asm


;setup following symbols
;port_ds1820
;   - has to be set

;tris_of_ds18b20_port
; - choose specific TRIS register

;port_pin_ds18b20
; - which pin


;AND add defintion of following variables 
;best will be to have them in same BANK
;ds18_status -
; different information about ds18 operation

;n_bit - counter to send data to ds18b20

;ds18_configuration 
; this holds information about power source 

;ds18_number_of_receive_bytes
; to hold number of bytes expected to be received

;ds18_measured_temp
;ds18_measured_temp_01
;variables holds measured temperatures, fraction in ds18_measured_temp_01

;ds18_command_to_be_send
; variable should store command to be send

;ds18_TH 
;       variable to set user byte TH

;ds18_TL
;       variable to set user byte TL

;ds18_config_byte
;       variable holds config byte to be send to ds18b20

;ds18_expected_resolution
; set 9,10, 11, 12 

;ds18_status
ds18_init_error    equ     0
ds18_init_ok       equ     1
ds18_crc_fault     equ     2
ds18_conv_busy     equ     3
ds18_wait_for_measurement   equ  4
ds18_below_zero    equ    5
ds18_precision_was_read  equ  6
ds18_receive_byte_from_lsb    equ  7

;ds18_configuration
ds18_parasite_power  equ   0
ds18_normal_power    equ   1 
ds18_multiple_sensors  equ  2

DS18B20_TH_byte      equ    2 
DS18B20_TL_byte      equ    3 
DS18B20_offset_of_config_byte   equ  4

DS18b20_read_rom     equ 0x33
DS18b20_read_power_supply  equ   0xb4
DS18b20_skip_rom     equ    0xCC
DS18b20_match_rom     equ    0x55

DS18b20_temp_convert     equ    0x44
DS18b20_read_scratchpad   equ   0xbe
DS18b20_write_scratchpad  equ   0x4e
DS18b20_search_rom        equ   0xf0

;config byte 

DS18B20_config_R0    equ  5
DS18B20_resolution_mask   equ    b'10011111'
DS18B20_low_resolution  equ 9
DS18B20_id_size         equ 8 

;*                                  *************************POLecenie DS1820
; in this procedure TMR2 is used for DS18B20 driver
; we need 
;"ds18_status" - this variable will hold bit state of ds18 information

PIN_HI
        BANKSEL tris_of_ds18b20_port
        BSF     tris_of_ds18b20_port, port_pin_ds18b20           ; high impedance
        RETURN

PIN_LO
        BANKSEL port_ds18b20
        BCF     port_ds18b20,port_pin_ds18b20
        BANKSEL tris_of_ds18b20_port
        BCF     tris_of_ds18b20_port, port_pin_ds18b20           ; low impedance zero
        RETURN

send_bit_1
         call     PIN_LO
         
         call     PIN_HI

        wait_specific_time_no_tmr_us   mcu_freq, .60, ds18_tmp
         
         return

send_bit_0
         call     PIN_LO         
        wait_specific_time_no_tmr_us   mcu_freq, .60, ds18_tmp
         call     PIN_HI
         
         return

check_ds_processing 

conversion_busy         

         call PIN_LO

         nop

         call PIN_HI 

         wait_specific_time_no_tmr_us  mcu_freq, .4, ds18_tmp 

        BANKSEL port_ds18b20
        btfss   port_ds18b20, port_pin_ds18b20
        goto    conversion_busy
    
    ; Bus is high - conversion done
        BANKSEL ds18_status
        bcf     ds18_status, ds18_conv_busy
        movlw   0x00               ; Return 0 = done
        return



init_ds1820
         BANKSEL   tris_of_ds18b20_port
         bcf      tris_of_ds18b20_port,port_pin_ds18b20
         
         call     PIN_HI
         
         call     PIN_LO
;loop to wait until ds18b20 will finish
        wait_specific_time_no_tmr_us  mcu_freq, .480, ds18_tmp

;switch to receive data from DS18
         BANKSEL  tris_of_ds18b20_port
         bsf      tris_of_ds18b20_port,port_pin_ds18b20
         nop
;sprawdzam czy w ciagu 480us pojawilo sie 0 na porcie czujnika 
        calculate_loop_number_to_reach_period_us  mcu_freq, .480, .4
        banksel ds18_tmp
        movwf ds18_tmp
        BANKSEL port_ds18b20
init_ds1820_loop_wait_for_ds_out        
         btfss    port_ds18b20,port_pin_ds18b20 ;1
         goto     petla_inicjacji3
         
         banksel ds18_tmp
         decfsz  ds18_tmp,f   ;1
         goto     init_ds1820_loop_wait_for_ds_out ;2

         goto   init_ds1820_error
         
petla_inicjacji3
        calculate_loop_number_to_reach_period_us  mcu_freq, .480, .4
        movwf  ds18_tmp
        BANKSEL port_ds18b20
ds18_init_loop3        
         btfsc    port_ds18b20,port_pin_ds18b20 ;1
         goto     inicjacja_ok

         decfsz   ds18_tmp,f ;1
         goto     petla_inicjacji3 ;2

init_ds1820_error 
;error of ds18b20 initialization
        BANKSEL ds18_status
        bsf ds18_status, ds18_init_error
        bcf ds18_status, ds18_init_ok
        goto init_ds1820_end

inicjacja_ok
        BANKSEL ds18_status
         bsf ds18_status, ds18_init_ok 
         bcf ds18_status,ds18_init_error

init_ds1820_end         
        return 
         
ds18_match_or_skip_rom 
        BANKSEL ds18_configuration
         btfss   ds18_configuration, ds18_multiple_sensors
         goto    ds18_match_or_skip_rom_skip

         movlw   DS18b20_match_rom
         call  ds18_send_request_loop_send_byte_start

         ;ds18 id is in FSR
         movlw  8
         BANKSEL ds18_number_of_bytes
         movwf ds18_number_of_bytes
ds18_send_id 
         movf  INDF,w 
         call  ds18_send_request_loop_send_byte_start
        INCF FSR,f
        BANKSEL ds18_number_of_bytes
        decfsz  ds18_number_of_bytes,f 
        goto ds18_send_id

        return



ds18_match_or_skip_rom_skip
         movlw   DS18b20_skip_rom
         call     ds18_send_request_loop_send_byte_start
        return


ds18_req_temp_convert
         ;1. send request to measure temperature   0xcc 0x44
         ;2. wait about 1 sec
         ;3. send request to read temperature      0x33
         
         ;disable_all_isr
         call     init_ds1820
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         RETURN

         ;observed issues without critical section

         disable_all_isr

         call ds18_match_or_skip_rom

         movlw   DS18b20_temp_convert
         call     ds18_send_request_loop_send_byte_start

        enable_all_isr
         ;wait about  1 s - for 12 bit measurement it is 750 ms
         BANKSEL  ds18_status
         bsf      ds18_status,ds18_wait_for_measurement
        #ifdef (DS18_CHECK_BUSY_AFTER_CONVERT_START)
         call check_ds_processing
         #endif
         return
     
ds18_req_scratchpad_read
        BANKSEL ds18_number_of_receive_bytes
         movlw    9
         movwf    ds18_number_of_receive_bytes

         
         call     init_ds1820
         
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         retlw  1
         disable_all_isr 
         call ds18_match_or_skip_rom
         
         movlw    DS18b20_read_scratchpad
         call     ds18_send_request_loop_send_byte_start

         BANKISEL  ds18_read_from_RAM
         movlw     LOW ds18_read_from_RAM
         movwf     FSR
         
         call     ds18_receive_data_from_sensor
         ;observed issues without critical section
        enable_all_isr 
         
         ;sprawdz CRC
         BANKISEL  ds18_read_from_RAM
         movlw  LOW ds18_read_from_RAM
         movwf  FSR     
         BANKSEL  n_bit   
         ;9 bajt to CRC
         movlw    9
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
         
         ;means fine
         retlw   0
;DS18B20




         
ds18_send_request_loop_send_byte_start

         BANKSEL  ds18_command_to_be_send
        movwf     ds18_command_to_be_send
         movlw     8 ;send 8 bit of request byte
         movwf     n_bit
ds18_send_request_loop_send_byte

        BANKSEL   ds18_command_to_be_send
        btfss     ds18_command_to_be_send,0
        call      send_bit_0
        BANKSEL   ds18_command_to_be_send
        btfsc     ds18_command_to_be_send,0
        call      send_bit_1
        
        BANKSEL ds18_command_to_be_send
        bcf       STATUS,C
        rrf       ds18_command_to_be_send,f
        
        BANKSEL n_bit
        decfsz    n_bit,f
        goto      ds18_send_request_loop_send_byte
        return
 
check_below_zero_measurement   macro temp_h 
        movlw  0xf0 
        BANKSEL temp_h 
        andwf  temp_h,w
        BANKSEL ds18_status
        bcf  ds18_status, ds18_below_zero
        SKPZ 
        bsf  ds18_status, ds18_below_zero

        ;if 0 it means + measurement
        ;if not 0 means - below 0
        endm 

ds18_translate_measurements_to_dec
         BANKISEL  ds18_read_from_RAM         
         movlw     LOW ds18_read_from_RAM
         movwf     FSR 
        incf FSR,f ;fsr shows temp_MSB
        check_below_zero_measurement  INDF
        SKPNZ  
        goto  convert_temperature_positive


        comf INDF,f ;temp_H
        decf FSR,f ;fsr shows temp_LSB
        ;convert to positive - minus will be added 
        comf INDF,F ;temp_L
        incf INDF,f ;+1
        SKPZ 
        goto convert_temperature_positive
        incf FSR,f  
        incf INDF,f

         
convert_temperature_positive
         BANKISEL  ds18_read_from_RAM         
         movlw     LOW ds18_read_from_RAM
         movwf     FSR 
        swapf  INDF,w
        andlw  0x0f 
        BANKSEL ds18_measured_temp
        movwf ds18_measured_temp

        movf   INDF,w 
        andlw  0x0f 
        movwf   ds18_measured_temp_01


        incf  FSR,f 
        swapf  INDF,w 
        andlw   0xf0 
        addwf  ds18_measured_temp,f 


        movlw HIGH  tab_fraction
        BANKSEL PCLATH
        movwf  PCLATH
        BANKSEL ds18_measured_temp
        movf ds18_measured_temp_01,w 
        ;calculation of fraction requires knowledge of current bit precision
        ;12 bit , 11 bit, 10 bit, 9 bit
        ;that gives following precision
        ;1/16  ,  1/8,   1/4 ,   1/2
        ;this information is read every time srachtpad is read with 0xBE command
        call  tab_fraction
        BANKSEL ds18_measured_temp
        movwf ds18_measured_temp_01

         return

;function to translate 
;ds18_measured_temp
;ds18_measured_temp_01 
;into 4 digits
; 125.9 
;or negative
; -54.9 (minus i 1000 number)
;this could be suitable for led segments
;or lcd alpahnumeric screens
;this is in form of macro
;digit1 has to be in lowest address, digit10 higher and so on
;temp_common_l has to be in common register (available for all banks)
ds18_convert_dec_measurement_into_4_digits_display macro digit1 , temp_common_l
    BANKSEL ds18_measured_temp
    movf   ds18_measured_temp,w 
    movwf  temp_common_l
    clrf   temp_common_l+1
;digit1
;digit10 = digit1+1
;digit100 = digit1 + 2
;digit1000 = digit1 +3
    macro_16bits_into_N_dec  temp_common_l, digit1, 3
    BANKSEL digit1
    movf digit1+2,w ;100 to 1000
    movwf digit1+3
    movf digit1+1,w ;10 to 100 
    movwf digit1+2
    movf digit1,w ; 1 to 10
    movwf digit1+1
    movf  ds18_measured_temp_01,w 
    movwf digit1

    movlw minus_symbol ; here define in inc file value that will show - symbol for led - or lcd or any other display type
    btfsc ds18_status,ds18_below_zero
    movwf digit1+3

    endm   

ds18_receive_data_from_sensor
        BANKSEL ds18_number_of_receive_bytes
         movf     ds18_number_of_receive_bytes,w
         BANKSEL ds18_number_of_bytes
         movwf    ds18_number_of_bytes
            
        
;procedura sprawdza czy ds1820 cos wysyla jezeli tak to sprawdza przez 60 us czy jest choc na chwile 0
;normalnie jezeli ds1820 nic nie wysyla to jest caly czas 1 bez rzadnych zmian
petla_odbioru_z_ds1820_1
        movlw     8
        BANKSEL n_bit
        movwf     n_bit
        clrf      INDF

        call   ds18_receive_one_bit

        incf      FSR,f
        
        BANKSEL ds18_number_of_bytes
;czy juz przeszly wszystkie bajty z DS
        decfsz    ds18_number_of_bytes,f
        goto      petla_odbioru_z_ds1820_1

         return      

        
ds18_receive_one_bit
         call     PIN_LO
        

         call     PIN_HI
         
         
         wait_specific_time_no_tmr_us   mcu_freq, .8, ds18_tmp
         
         BANKSEL port_ds18b20
         btfss    port_ds18b20,port_pin_ds18b20
         bcf      STATUS,C
         btfsc    port_ds18b20,port_pin_ds18b20
         bsf      STATUS,C
         BANKSEL ds18_status
         btfss  ds18_status,ds18_receive_byte_from_lsb
         rrf     INDF,f
         btfsc  ds18_status,ds18_receive_byte_from_lsb
         rlf    INDF,f

        wait_specific_time_no_tmr_us  mcu_freq, .50, ds18_tmp

        BANKSEL n_bit
        decfsz    n_bit,f
        goto      ds18_receive_one_bit
        return 


;array with DS18b20 bytes scratchpad should be placed in FSR
check_CRC_DS 
         BANKSEL ds18_CRC_result
         clrf  ds18_CRC_result
check_CRC_DS_loop
         movf INDF,w   
         incf   FSR,f
         xorwf ds18_CRC_result,f       
         movlw 0     
         
         btfsc ds18_CRC_result,0 
         xorlw 0x5e       
         
         btfsc ds18_CRC_result,1 
         xorlw 0xbc 
         
         btfsc ds18_CRC_result,2 
         xorlw 0x61 
         
         btfsc ds18_CRC_result,3 
         xorlw 0xc2 
         
         btfsc ds18_CRC_result,4 
         xorlw 0x9d 
         
         btfsc ds18_CRC_result,5 
         xorlw 0x23 
         
         btfsc ds18_CRC_result,6 
         xorlw 0x46 
         
         btfsc ds18_CRC_result,7 
         xorlw 0x8c 
         
         movwf ds18_CRC_result   
         
         BANKSEL n_bit
         decfsz n_bit,f 
         goto check_CRC_DS_loop         
                     

         return         

;*********END of ds1820 procedures