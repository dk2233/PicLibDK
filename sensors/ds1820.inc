;ds18_status
ds18_init_error    equ     0
ds18_init_ok       equ     1


;markers_pomiary 
wykonaj_pomiar   equ   0
wysylam_rozkaz_pomiaru   equ  1
czekam_na_odczyt_DS1    equ  2
wysylam_rozkaz_odbioru   equ  3 
odczytaj_pomiar_DS1      equ 4
czy_wykonuje_pomiar_DS1   equ  5


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
         nop
         call     PIN_HI

        BANKSEL   PIR1
loop_send_bit1
         btfss    PIR1,TMR2IF        
         goto     loop_send_bit1
         
         return

send_bit_0
         ;bsf      port_ds18b20,port_pin_ds18b20
         call     PIN_LO         
         clrf     TMR2
         nop
         
         bcf      PIR1,TMR2IF
         
         bcf      port_ds18b20,port_pin_ds18b20
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
         
         configure_tmr2 1 , 1, ds18b20_tmr2_value_to_wait_480us

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
        BANKSEL PIR1
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
        goto init_ds1820_end

inicjacja_ok
        BANKSEL ds18_status
         bsf ds18_status, ds18_init_ok 

init_ds1820_end         
        return 
         
             


Pomiary_temperatury
        BANKSEL znak_rozkazu_ds
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
         
         clrf   znak_rozkazu_ds
         
         call     init_ds1820
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         RETURN
         
         call     petla_wysylania_rozkazu_1

         
         ;pozniej czekaj 1 s
         BANKSEL  markers_pomiary
         bcf         markers_pomiary, wysylam_rozkaz_pomiaru
         bsf      markers_pomiary,czekam_na_odczyt_DS1
         
         return
     
;DS18B20
petla_wysylania_rozkazu_1

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
         ;movwf        TABLAT,w
	 movwf     polecenie_wysylane
         xorlw     0
         btfsc    STATUS,Z
         return             
          
         incf   znak_rozkazu_ds,f
         
         
         ;jezeli 0 to skocz do wyswietlania slowa z linii 2
         
         
         movlw     ds18b20_tmr2_value_to_wait_60us      
         ;ustawiam TMR2 na odbieranie
         
         banksel  PR2
         movwf    PR2
         banksel  T2CON
;ustawiam 60us         
         movlw    ds18b20_tmr2_value_to_wait_60us
         movwf    T2CON
         bcf      PIR1,TMR2IF
         clrf      TMR2
         movlw     8
         movwf     n
         
petla_sending_pomiar_1

        btfss     polecenie_wysylane,0
        call      send_bit_0
        btfsc     polecenie_wysylane,0
        call      send_bit_1
        bsf       port_ds18b20,port_pin_ds18b20
        
        bcf       STATUS,C
        rrf       polecenie_wysylane,f
        
        decfsz    n,f
        goto      petla_sending_pomiar_1
         
        goto       petla_wysylania_rozkazu_1
 

odbierz_pomiary_temp
        BANKSEL markers_pomiary
       bcf  markers_pomiary,odczytaj_pomiar_DS1
       bcf  markers_pomiary,czekam_na_odczyt_DS1
       bcf  markers_pomiary,czy_wykonuje_pomiar_DS1
        clrf    znak_rozkazu_ds
         
         bsf         markers_pomiary, wysylam_rozkaz_odbioru
         call     init_ds1820
         
         
         call     petla_wysylania_rozkazu_1
         bcf         markers_pomiary, wysylam_rozkaz_odbioru


         BANKISEL  dane_odebrane_z_ds
         movlw     LOW dane_odebrane_z_ds
         movwf     FSR
         
         call     petla_odbioru_rozkazu_1
         
         
         ;sprawdz CRC
         BANKISEL  dane_odebrane_z_ds
         movlw  LOW dane_odebrane_z_ds
         movwf  FSR        
         ;9 bajt to CRC
         movlw    9
         movwf    n
         
         call     check_CRC_DS
         
         movf     bajt_CRC,w
         ;jezeli 0 to jest ok
         btfsc    STATUS,Z         
         goto      odbierz_pomiary_temp_show_data
         
         
         ; call    wysylac_napis_CRC_notOK
         
        
        
         return


odbierz_pomiary_temp_show_data
         
         BANKISEL  dane_odebrane_z_ds         
         movlw     LOW dane_odebrane_z_ds
         movwf     FSR
         
;call     zamien_dane_na_temp
        
        
         return
         



petla_odbioru_rozkazu_1
         movf     jak_duzo_bajtow_odbieram_z_ds,w
         movwf    tmp7
            
         movlw     ds18b20_tmr2_value_to_wait_60us
         banksel    PR2
         movwf    PR2
         movlw    ds18b20_tmr2_value_to_wait_60us
         
         banksel        T2CON
         movwf    T2CON
         clrf     TMR2
         bcf      PIR1,TMR2IF   
        
;procedura sprawdza czy ds1820 cos wysyla jezeli tak to sprawdza przez 60 us czy jest choc na chwile 0
;normalnie jezeli ds1820 nic nie wysyla to jest caly czas 1 bez rzadnych zmian
petla_odbioru_z_ds1820_1
        movlw     8
        movwf     n
        clrf      TMR2
        clrf      INDF
        bcf       PIR1,TMR2IF
        
petla_stan_odebranego_bitu_1
         
         call     PIN_LO
        
         nop
         nop
         nop
         
         call     PIN_HI
         
         
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         
         btfss    port_ds18b20,port_pin_ds18b20
         bcf      STATUS,C
         btfsc    port_ds18b20,port_pin_ds18b20
         bsf      STATUS,C
         rrf     INDF,f
         
czekam_na_kolejny_bit_DS_1
        btfss    PIR1,TMR2IF
        goto      czekam_na_kolejny_bit_DS_1
        
        bcf       PIR1,TMR2IF
        
        decfsz    n,f
        goto      petla_stan_odebranego_bitu_1
        incf      FSR,f
        
;czy juz przeszly wszystkie bajty z DS
        decfsz    tmp7,f
        goto      petla_odbioru_z_ds1820_1

         return      

check_CRC_DS 
         BANKSEL bajt_CRC
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