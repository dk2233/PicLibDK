;ds18_status
ds18_init_error    equ     0
ds18_init_ok       equ     1
ds18_crc_fault     equ     2
ds18_conv_busy     equ     3
ds18_parasite_power  equ   4
ds18_normal_power    equ   5 
ds18_precision_was_read  equ  6


;markers_pomiary 
;wykonaj_pomiar   equ   0
wysylam_rozkaz_pomiaru   equ  1
czekam_na_odczyt_DS1    equ  2
wysylam_rozkaz_odbioru   equ  3 
odczytaj_pomiar_DS1      equ 4
;czy_wykonuje_pomiar_DS1   equ  5

DS18b20_read_rom     equ 0x33
DS18b20_read_power_supply  equ   0xb4
DS18b20_skip_rom     equ    0xCC
DS18b20_temp_convert     equ    0x44
DS18b20_read_scratchpad   equ   0xbe

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
         BANKSEL TMR2
         clrf     TMR2
         bcf      PIR1,TMR2IF
         call     PIN_LO
         
         call     PIN_HI

        BANKSEL   PIR1
loop_send_bit1
         btfss    PIR1,TMR2IF        
         goto     loop_send_bit1
         
         return

send_bit_0
         call     PIN_LO         
         BANKSEL  TMR2
         clrf     TMR2
         bcf      PIR1,TMR2IF
         
         BANKSEL  PIR1
loop_send_bit_0
         btfss    PIR1,TMR2IF        
         goto     loop_send_bit_0
         call     PIN_HI
         
         return

check_ds_processing 

         configure_tmr2 0 , 1, ds18b20_tmr2_value_to_wait_60us ; prescaler 1 , post 2
    
         BANKSEL PIE1 
         bcf  PIE1,TMR2IE
         BANKSEL TMR2
         clrf TMR2
conversion_busy         
         bcf  PIR1,TMR2IF

         call PIN_LO

         nop

         call PIN_HI 

         wait_specific_time_no_tmr_us  mcu_freq, .4, operandl 

        BANKSEL port_ds18b20
        btfss   port_ds18b20, port_pin_ds18b20
        goto    conversion_busy
    
    ; Bus is high - conversion done
        BANKSEL ds18_status
        bcf     ds18_status, ds18_conv_busy
        movlw   0x00               ; Return 0 = done
        return



init_ds1820
;wysyla 0 na linie danych do ds1820 przez 
;inicjalizacja 0 na wyjsciu okolo 480us tzn dla ustawienia 1:2 
;        dokladnie 256 z liczeniem TMR2 co 2, pozniej otwarcie portu na wejscie przejscie do 1 i po kilkunastu us otwarcie portu na wejscie
;        powinien byc obecny sygnal 0 do okolo 240 us.
;otwieram port na wyjscie
         BANKSEL   tris_of_ds18b20_port
         bcf      tris_of_ds18b20_port,port_pin_ds18b20
         
         configure_tmr2 0 , 1, ds18b20_tmr2_value_to_wait_480us ; prescaler 1 , post 2
         ;4/4e6 * 240 * 2 = 480 us
         BANKSEL PIE1 
         bcf  PIE1,TMR2IE
         BANKSEL TMR2
         clrf TMR2
         bcf  PIR1,TMR2IF

         call     PIN_HI
         
         call     PIN_LO
;loop to wait until ds18b20 will finish
        wait_until_tmr2_period_pass 

;switch to receive data from DS18
         BANKSEL  tris_of_ds18b20_port
         bsf      tris_of_ds18b20_port,port_pin_ds18b20
         nop
         BANKSEL  PIR1
         bcf      PIR1,TMR2IF
;sprawdzam czy w ciagu 480us pojawilo sie 0 na porcie czujnika 
init_ds1820_loop_wait_for_ds_out        
         btfss    port_ds18b20,port_pin_ds18b20
         goto     petla_inicjacji3
         btfss    PIR1,TMR2IF
         goto     init_ds1820_loop_wait_for_ds_out

         goto   init_ds1820_error
         
petla_inicjacji3
         btfsc    port_ds18b20,port_pin_ds18b20
         goto     inicjacja_ok
         btfss    PIR1,TMR2IF
         goto     petla_inicjacji3

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

        wait_until_tmr2_period_pass ; need to finish until 480us perios will finish

init_ds1820_end         
        return 
         
             


ds18_req_temp_convert
        BANKSEL znak_rozkazu_ds
        clrf    znak_rozkazu_ds
        
         clrf   markers_pomiary
         
         ;1. send request to measure temperature   0xcc 0x44
         ;2. wait about 1 sec
         ;3. send request to read temperature      0x33
         
         clrf   znak_rozkazu_ds
         
         ;disable_all_isr
         call     init_ds1820
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         RETURN

         configure_tmr2 0 , 1, ds18b20_tmr2_value_to_wait_60us ; prescaler 1 , post 2
         BANKSEL polecenie_wysylane
         movlw   DS18b20_skip_rom
         movwf   polecenie_wysylane

         call     ds18_send_request_loop_send_byte_start
         
         BANKSEL polecenie_wysylane
         movlw   DS18b20_temp_convert
         movwf   polecenie_wysylane

         call     ds18_send_request_loop_send_byte_start

         ;wait about  1 s - for 12 bit measurement it is 750 ms
         BANKSEL  markers_pomiary
         bsf      markers_pomiary,czekam_na_odczyt_DS1

         call check_ds_processing
         ;enable_all_isr
         
         return
     
ds18_req_scratchpad_read
        BANKSEL markers_pomiary
        clrf markers_pomiary
         movlw    9
         movwf    jak_duzo_bajtow_odbieram_z_ds

        call check_ds_processing
         
         call     init_ds1820
         
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         retlw  1
         configure_tmr2 0 , 1, ds18b20_tmr2_value_to_wait_60us ; prescaler 1 , post 2
         ;disable_all_isr 
         BANKSEL  polecenie_wysylane
         movlw    DS18b20_skip_rom
         movwf    polecenie_wysylane         
         call     ds18_send_request_loop_send_byte_start
         
         BANKSEL  polecenie_wysylane
         movlw    DS18b20_read_scratchpad
         movwf    polecenie_wysylane         
         call     ds18_send_request_loop_send_byte_start

         BANKSEL markers_pomiary
         bcf      markers_pomiary, wysylam_rozkaz_odbioru

         BANKISEL  dane_odebrane_z_ds
         movlw     LOW dane_odebrane_z_ds
         movwf     FSR
         
         call     petla_odbioru_rozkazu_1
        ;enable_all_isr 
         
         ;sprawdz CRC
         BANKISEL  dane_odebrane_z_ds
         movlw  LOW dane_odebrane_z_ds
         movwf  FSR     
         BANKSEL  n_bit   
         ;9 bajt to CRC
         movlw    9
         movwf    n_bit
         
         call     check_CRC_DS
         BANKSEL ds8_CRC_result
         movf     ds8_CRC_result,w
         ;jezeli 0 to jest ok
         btfsc    STATUS,Z         
         bcf ds18_status, ds18_crc_fault

         ;show crc fault
         btfss    STATUS,Z         
         bsf ds18_status, ds18_crc_fault
         
         ;means fine
         retlw   0
;DS18B20

ds18_read_power_supply 
        BANKSEL markers_pomiary
        clrf markers_pomiary
         call     init_ds1820
         
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         RETURN
         configure_tmr2 0 , 1, ds18b20_tmr2_value_to_wait_60us ; prescaler 1 , post 2
         
         BANKSEL polecenie_wysylane
         movlw   DS18b20_skip_rom
         movwf   polecenie_wysylane

         call     ds18_send_request_loop_send_byte_start
         
         BANKSEL polecenie_wysylane
         movlw   DS18b20_read_power_supply
         movwf   polecenie_wysylane

         call     ds18_send_request_loop_send_byte_start
;read power type = 0 or 1
         call PIN_LO

         nop

         call PIN_HI 

         wait_specific_time_no_tmr_us  mcu_freq, .8, operandl 
         ;fill (nop), .2

        BANKSEL port_ds18b20
        btfss   port_ds18b20, port_pin_ds18b20
        goto    ds18_read_power_supply_parasite 

        BANKSEL ds18_status

        bcf  ds18_status, ds18_parasite_power
        bsf  ds18_status, ds18_normal_power
        return 

ds18_read_power_supply_parasite
        BANKSEL ds18_status
        bsf  ds18_status, ds18_parasite_power
        bcf  ds18_status, ds18_normal_power
        return 

ds18_req_id_read
        BANKSEL markers_pomiary
        clrf markers_pomiary
         movlw    8
         movwf    jak_duzo_bajtow_odbieram_z_ds
         disable_all_isr 
         call     init_ds1820
         
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         RETURN
         configure_tmr2 0 , 1, ds18b20_tmr2_value_to_wait_60us ; prescaler 1 , post 2
         
         BANKSEL polecenie_wysylane
         movlw   DS18b20_read_rom
         movwf   polecenie_wysylane

         call     ds18_send_request_loop_send_byte_start


         BANKISEL  dane_odebrane_z_ds
         movlw     LOW dane_odebrane_z_ds
         movwf     FSR
         
         call     petla_odbioru_rozkazu_1
         
         enable_all_isr
         ;sprawdz CRC
         BANKISEL  dane_odebrane_z_ds
         movlw  LOW dane_odebrane_z_ds
         movwf  FSR     
         BANKSEL  n_bit   
         ;8 bajt to CRC
         movlw    8
         movwf    n_bit
         
         call     check_CRC_DS
         BANKSEL ds8_CRC_result
         movf     ds8_CRC_result,w
         ;jezeli 0 to jest ok
         btfsc    STATUS,Z         
         bcf ds18_status, ds18_crc_fault

         ;show crc fault
         btfss    STATUS,Z         
         bsf ds18_status, ds18_crc_fault
         
         return
ds18_send_request_loop
        BANKSEL  markers_pomiary
        btfsc    markers_pomiary, wysylam_rozkaz_pomiaru
        movlw HIGH rozkaz_pomiaru
        
        btfsc    markers_pomiary, wysylam_rozkaz_odbioru
        movlw HIGH rozkaz_odbioru
        
        movwf    PCLATH
        
        btfsc    markers_pomiary, wysylam_rozkaz_pomiaru
        call     rozkaz_pomiaru
 
        btfsc    markers_pomiary, wysylam_rozkaz_odbioru
        call rozkaz_odbioru
        
         clrf     PCLATH
	 movwf     polecenie_wysylane
         xorlw     0
         btfsc    STATUS,Z
         return             
          
         incf   znak_rozkazu_ds,f
         
;set 60us wait time
         call   ds18_send_request_loop_send_byte_start

        goto       ds18_send_request_loop
        return

         
ds18_send_request_loop_send_byte_start

         BANKSEL   PIR1
         clrf      TMR2
         bcf      PIR1,TMR2IF
         movlw     8 ;send 8 bit of request byte
         movwf     n_bit
ds18_send_request_loop_send_byte

        BANKSEL   polecenie_wysylane
        btfss     polecenie_wysylane,0
        call      send_bit_0
        BANKSEL   polecenie_wysylane
        btfsc     polecenie_wysylane,0
        call      send_bit_1
        
        bcf       STATUS,C
        rrf       polecenie_wysylane,f
        
        decfsz    n_bit,f
        goto      ds18_send_request_loop_send_byte
        return
 



odbierz_pomiary_temp_show_data
        BANKSEL markers_pomiary
         clrf markers_pomiary
         BANKISEL  dane_odebrane_z_ds         
         movlw     LOW dane_odebrane_z_ds
         movwf     FSR
         
        swapf  INDF,w
        andlw  0x0f 
        movwf ds18_measured_temp

        movf   INDF,w 
        andlw  0x0f 
        movwf   ds18_measured_temp_01


        incf  FSR,f 
        swapf  INDF,w 
        andlw   0xf0 
        addwf  ds18_measured_temp,f 


        ;calculation of fraction requires knowledge of current bit precision
        ;12 bit , 11 bit, 10 bit, 9 bit
        ;that gives following precision
        ;1/16  ,  1/8,   1/4 ,   1/2
        ;this information is read every time srachtpad is read with 0xBE command

         return
         



petla_odbioru_rozkazu_1
        BANKSEL jak_duzo_bajtow_odbieram_z_ds
         movf     jak_duzo_bajtow_odbieram_z_ds,w
         movwf    tmp7
            
         ;configure_tmr2 0 , 1, ds18b20_tmr2_value_to_wait_60us ; prescaler 4 , post 2
         banksel TMR2
         clrf     TMR2
         bcf      PIR1,TMR2IF   
        
;procedura sprawdza czy ds1820 cos wysyla jezeli tak to sprawdza przez 60 us czy jest choc na chwile 0
;normalnie jezeli ds1820 nic nie wysyla to jest caly czas 1 bez rzadnych zmian
petla_odbioru_z_ds1820_1
        movlw     8
        movwf     n_bit
        clrf      TMR2
        clrf      INDF
        bcf       PIR1,TMR2IF
        
petla_stan_odebranego_bitu_1
         call     PIN_LO
        

         call     PIN_HI
         
         
         wait_specific_time_no_tmr_us   mcu_freq, .8, operandl
         
         BANKSEL port_ds18b20
         btfss    port_ds18b20,port_pin_ds18b20
         bcf      STATUS,C
         btfsc    port_ds18b20,port_pin_ds18b20
         bsf      STATUS,C
         rrf     INDF,f
         
czekam_na_kolejny_bit_DS_1
        btfss    PIR1,TMR2IF
        goto      czekam_na_kolejny_bit_DS_1
        
        bcf       PIR1,TMR2IF
        
        decfsz    n_bit,f
        goto      petla_stan_odebranego_bitu_1
        incf      FSR,f
        
;czy juz przeszly wszystkie bajty z DS
        decfsz    tmp7,f
        goto      petla_odbioru_z_ds1820_1

         return      

check_CRC_DS 
         BANKSEL ds8_CRC_result
         clrf  ds8_CRC_result
         ;movlw    
   
;tablica danych DS18b20 musi byc w FSR2
check_CRC_DS_loop
         movf INDF,w   
         incf   FSR,f
         xorwf ds8_CRC_result,f       
         movlw 0     
         
         btfsc ds8_CRC_result,0 
         xorlw 0x5e       
         
         btfsc ds8_CRC_result,1 
         xorlw 0xbc 
         
         btfsc ds8_CRC_result,2 
         xorlw 0x61 
         
         btfsc ds8_CRC_result,3 
         xorlw 0xc2 
         
         btfsc ds8_CRC_result,4 
         xorlw 0x9d 
         
         btfsc ds8_CRC_result,5 
         xorlw 0x23 
         
         btfsc ds8_CRC_result,6 
         xorlw 0x46 
         
         btfsc ds8_CRC_result,7 
         xorlw 0x8c 
         
         movwf ds8_CRC_result   
         
         decfsz n_bit,f 
         goto check_CRC_DS_loop         
                     

         return         

rozkaz_pomiaru         
        BANKSEL  znak_rozkazu_ds
        movf     znak_rozkazu_ds,w
         addwf    PCL,f         
         dt   0xcc,0x44,0x00

rozkaz_odbioru
        BANKSEL  znak_rozkazu_ds
        movf     znak_rozkazu_ds,w
         addwf    PCL,f
         dt   0xcc,0xbe,0x00
         
rozkac_id
        BANKSEL  znak_rozkazu_ds
        movf     znak_rozkazu_ds,w
         addwf    PCL,f
         dt       0x33,0x00
;*********KOniec ds1820