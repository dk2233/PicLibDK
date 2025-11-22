;ds18_status
ds18_init_error    equ     0
ds18_init_ok       equ     1


;*                                  *************************POLecenie DS1820
; in this procedure TMR2 is used for DS18B20 driver
; we need 
;"ds18_status" - this variable will hold bit state of ds18 information

PIN_HI
        BANKSEL tris_of_ds18b20_port
        BSF     tris_of_ds18b20_port, port_pin_ds18b20           ; high impedance
        RETURN

PIN_LO
        BANKSEL port_ds1820
        BCF     port_ds1820,port_pin_ds18b20
        BANKSEL tris_of_ds18b20_port
        BCF     tris_of_ds18b20_port, port_pin_ds18b20           ; low impedance zero
        RETURN

send_bit_1
         BANKSEL TMR2
         clrf     TMR2
         bcf      PIR1,TMR2IF
         call     PIN_LO
         nop
         call     PIN_HI

loop_send_bit1
         btfss    PIR1,TMR2IF        
         goto     loop_send_bit1
         
         return

send_bit_0
         ;bsf      port_ds1820,port_pin_ds18b20
         call     PIN_LO         
         clrf     TMR2
         nop
         
         bcf      PIR1,TMR2IF
         
         bcf      port_ds1820,port_pin_ds18b20
loop_send_bit_0
         btfss    PIR1,TMR2IF        
         goto     loop_send_bit_0
         call     PIN_HI
         
         return



init_ds1820
;wysyla 0 na linie danych do ds1820 przez 
;inicjalizacja 0 na wyjsciu okolo 480us tzn dla ustawienia 1:2 
;        dokladnie 256 z liczeniem TMR2 co 2, pozniej otwarcie portu na wejscie przejscie do 1 i po kilkunastu us otwarcie portu na wejscie
;        powinien byc obecny sygnal 0 do okolo 240 us.
;otwieram port na wyjscie
         BANKSEL   tris_of_ds18b20_port
         bcf      tris_of_ds18b20_port,port_pin_ds18b20
         
;ustawiam tmr2 na zliczanie 2 
         ;movlw    0xff   ; here we need a value + prescaler\postscaler to give about 480 us impuls         
         ;movwf    PR2
         
         configure_tmr2 0 , 1, ds18b20_tmr2_value_to_wait_480us

         ;movlw    b'00001100'
         ;movwf    T2CON
         BANKSEL PIR1
         bcf      PIR1,TMR2IF
;wlaczam przerwanie Tmr2
         ;bsf      STATUS,RP0
         ;bsf      PIE1,TMR2IE2
         ;bcf      STATUS,RP0         
         call     PIN_HI
         call     PIN_LO
;daje znacznik ze dotyczy to inicjacji ds1820
         ;bsf      znaczniki_ds,inicjacja
;loop to wait until ds18b20 will finish
loop_wait_for_init
         
         btfss    PIR1,TMR2IF
         goto     loop_wait_for_init
;switch to receive data from DS18
         BANKSEL  tris_of_ds18b20_port
         bsf      tris_of_ds18b20_port,port_pin_ds18b20
         nop
         BANKSEL  PIR1
         bcf      PIR1,TMR2IF
;sprawdzam czy w ciagu 480us pojawilo sie 0 na porcie czujnika 
loop_wait_for_ds_out
         btfss    port_ds1820,port_pin_ds18b20
         goto     petla_inicjacji3
         btfss    PIR1,TMR2IF
         goto     loop_wait_for_ds_out

         goto   init_ds1820_error
         
petla_inicjacji3
         btfsc    port_ds1820,port_pin_ds18b20
         goto     inicjacja_ok
         btfss    PIR1,TMR2IF
         goto     petla_inicjacji3

init_ds1820_error 
;error of ds18b20 initialization
        BANKSEL ds18_status
        bsf ds18_status, ds18_init_error
        goto init_ds1820_end

inicjacja_ok
        BANKSEL ds18_status
         bsf ds18_status, ds18_init_ok 

init_ds1820_end         
        return 
         
             
;jezeli otrzyma 1 to na ekran wysyla informacje 'ds ok'         


Pomiary_temperatury
        bcf     markers,wykonaj_pomiar
        
        clrf    znak_rozkazu_ds
        
         movlw    9
         movwf    jak_duzo_bajtow_odbieram_z_ds
         
         ; movlw       HIGH rozkaz_pomiaru
         ; movwf       TBLPTRH
      
         ; movlw       LOW rozkaz_pomiaru
         ; movwf       TBLPTRL
         
         bsf         markers_pomiary, wysylam_rozkaz_pomiaru
         
         ;najpierw wy�lij rozkaz pomiaru
         ;wykorzystuje procedury kt�re dzia�aj� po wybraniu wysy�ania danych przez polecenie
         ;
         ;        "*D1scc44"
         
         ; bcf    markers,czy_rozkaz
         clrf   znak_rozkazu_ds
         
         call     inicjacja_ds1820_1
         
         call     petla_wysylania_rozkazu_1

         
         ;pozniej czekaj 1 s
         bcf         markers_pomiary, wysylam_rozkaz_pomiaru
         bsf      markers_pomiary,czekam_na_odczyt_DS1
         
         return
     


check_CRC_DS 
         clrf  bajt_CRC
         ;movlw    
   
;tablica danych DS18b20 musi byc w FSR2
check_CRC_DS_loop
         movf INDF,w   
         incf   FSR,f
         xorwf bajt_CRC,f       
         movlw 0     
         
         btfsc bajt_CRC,0 
         xorlw 0x5e       
         
         btfsc bajt_CRC,1 
         xorlw 0xbc 
         
         btfsc bajt_CRC,2 
         xorlw 0x61 
         
         btfsc bajt_CRC,3 
         xorlw 0xc2 
         
         btfsc bajt_CRC,4 
         xorlw 0x9d 
         
         btfsc bajt_CRC,5 
         xorlw 0x23 
         
         btfsc bajt_CRC,6 
         xorlw 0x46 
         
         btfsc bajt_CRC,7 
         xorlw 0x8c 
         
         movwf bajt_CRC   
         
         decfsz n,f 
         goto check_CRC_DS_loop         
                     

         ;movwf bajt_CRC         
 

         return         

rozkaz_pomiaru         
        movf     znak_rozkazu_ds,w
         addwf    PCL,f         
         dt   0xcc,0x44,0x00

rozkaz_odbioru
        movf     znak_rozkazu_ds,w
         addwf    PCL,f
         dt   0xcc,0xbe,0x00
         
rozkac_id
        movf     znak_rozkazu_ds,w
         addwf    PCL,f
         dt       0x33,0x00
;*********KOniec ds1820