;*                                  *************************POLecenie DS1820
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

send4bit_1
         BANKSEL TMR2
         clrf     TMR2
         bcf      PIR1,TMR2IF
         call     PIN_LO
         nop
         call     PIN_HI
         ;bcf      port_ds1820,port_pin_ds18b20
         ;nop
         ;nop
         ;nop
         ;nop
         ;nop
         ;nop
         ;bsf      port_ds1820,port_pin_ds18b20
loop_send4bit1
         btfss    PIR1,TMR2IF        
         goto     loop_send4bit1
         
         return

send4bit_0
         ;bsf      port_ds1820,port_pin_ds18b20
         call     PIport_pin_ds18b20N_LO         
         clrf     TMR2
         nop
         
         bcf      PIR1,TMR2IF
         
         bcf      port_ds1820,port_pin_ds18b20
loop_send4bit0
         btfss    PIR1,TMR2IF        
         goto     loop_send4bit0
         call     PIN_HI
         
         return



inicjacja_ds1820
;wysyla 0 na linie danych do ds1820 przez 
;inicjalizacja 0 na wyjsciu okolo 480us tzn dla ustawienia 1:2 
;        dokladnie 256 z liczeniem TMR2 co 2, pozniej otwarcie portu na wejscie przejscie do 1 i po kilkunastu us otwarcie portu na wejscie
;        powinien byc obecny sygnal 0 do okolo 240 us.
;otwieram port na wyjscie
         bcf      RCSTA,CREN
         bsf      STATUS,RP0
         bcf      tris_of_ds18b20_port,port_pin_ds18b20
         
;ustawiam tmr2 na zliczanie 2 
         movlw    0xff
         
         movwf    PR2
         bcf      STATUS,RP0
         
         movlw    b'00001100'
         movwf    T2CON
         bcf      PIR1,TMR2IF
;wlaczam przerwanie Tmr2
         ;bsf      STATUS,RP0
         ;bsf      PIE1,TMR2IE2
         ;bcf      STATUS,RP0         
         call     PIN_HI
         call     PIN_LO
;daje znacznik ze dotyczy to inicjacji ds1820
         ;bsf      znaczniki_ds,inicjacja
;petla czekania na koniec inicjacji
petla_inicjacji1
         
         btfss    PIR1,TMR2IF
         goto     petla_inicjacji1
;teraz przelaczam sie na odbior danych z ds1820
         BANKSEL  tris_of_ds18b20_port
         bsf      tris_of_ds18b20_port,port_pin_ds18b20
         nop
         BANKSEL  PIR1
         bcf      PIR1,TMR2IF
;sprawdzam czy w ciagu 480us pojawilo sie 0 na porcie czujnika
petla_inicjacji2
         btfss    port_ds1820,port_pin_ds18b20
         goto     petla_inicjacji3
         btfss    PIR1,TMR2IF
         goto     petla_inicjacji2
         
blad_inicjacji_ds
         call    sprawdz_czy_mozna_wysylac
         movlw    znak_lf
         movwf   TXREG
         call    sprawdz_czy_mozna_wysylac
         movlw    0x35
         
         movwf   TXREG
         call    sprawdz_czy_mozna_wysylac
         movlw    znak_lf
         movwf   TXREG
         goto     powrot_z_polecen   
petla_inicjacji3
         btfsc    port_ds1820,port_pin_ds18b20
         goto     inicjacja_ok
         btfss    PIR1,TMR2IF
         goto     petla_inicjacji3
         
inicjacja_ok
         btfsc    markers,czy_wysylanie
         goto     wysylanie_danych_rozkaz
         
         call    sprawdz_czy_mozna_wysylac
         movlw    znak_lf
         movwf   TXREG
         call    sprawdz_czy_mozna_wysylac
         movlw    _O
         
         movwf   TXREG
         call    sprawdz_czy_mozna_wysylac
         movlw    _K
         movwf   TXREG
         call    sprawdz_czy_mozna_wysylac
         movlw    znak_lf
         movwf   TXREG
         call    sprawdz_czy_mozna_wysylac
         
         goto     powrot_z_polecen
         
         
             
;jezeli otrzyma 1 to na ekran wysyla informacje 'ds ok'         


wysylanie_danych
         bsf      markers,czy_wysylanie
         goto     inicjacja_ds1820
wysylanie_danych_rozkaz
         bcf      markers,czy_wysylanie
         bsf      STATUS,RP0
        
;ustawiam tmr2 na 60us bo tyle trwa jeden bit  
         movlw    0x3c
         
         movwf    PR2
         
         ;bcf      tris_of_ds18b20_port,port_pin_ds18b20
        
         bcf      STATUS,RP0
;zliczanie 1:1         
         movlw    b'00000100'
         movwf    T2CON
         bcf      PIR1,TMR2IF
;wlaczam przerwanie Tmr2
         
wysylanie_bajtu_rozkazu
;zczytuje dana do wyslania
        call      kopiuj_liczbe_przeslana
        movf      tmp,w
        movwf     polecenie_wysylane
        clrf      TMR2
        movlw     8
        movwf     n
petla_send4biting        
;jezeli 0 to wyslij   0
        btfss     polecenie_wysylane,0
        call      send4bit_0
        btfsc     polecenie_wysylane,0
        call      send4bit_1
        bsf       port_ds1820,port_pin_ds18b20
        bcf       STATUS,C
        rrf       polecenie_wysylane,f
        bcf       STATUS,C
        ;movf      polecenie_wysylane,w
        ;btfss     STATUS,Z
        decfsz    n,f
        goto      petla_send4biting
        
;sprawdzam czy kolejny bajt jest 0x0d        
        incf     FSR,f
        movf      INDF,w
        xorlw     0x0d
        btfsc     STATUS,Z
        goto     sprawdz_czy_ds1820_wysyla
        
        decf      FSR,f
        goto      wysylanie_bajtu_rozkazu
        
        
sprawdz_czy_ds1820_wysyla
         movlw    bajt_ds
         movwf    FSR
         movf     jak_duzo_bajtow_odbieram_z_ds,w
         movwf    liczba1
            
        ;goto     powrot_z_polecen
;procedura sprawdza czy ds1820 cos wysyla jezeli tak to sprawdza przez 60 us czy jest choc na chwile 0
;normalnie jezeli ds1820 nic nie wysyla to jest caly czas 1 bez rzadnych zmian
petla_odbioru_ds1820
        movlw     8
        movwf     n
        clrf      TMR2
        clrf      INDF
        bcf       PIR1,TMR2IF
        
petla_czy_jest_0
         call     PIN_LO
         nop
         call     PIN_HI
         nop
         nop
         nop
         nop
         nop
         nop
         nop
         movf     port_ds1820,w
         movwf    liczba2
         btfss    liczba2,port_pin_ds18b20
         bcf      STATUS,C
         btfsc    liczba2,port_pin_ds18b20
         bsf      STATUS,C
         rrf      INDF,f
         
czekam_na_kolejny_bit
        btfss    PIR1,TMR2IF
        goto      czekam_na_kolejny_bit
        bcf       PIR1,TMR2IF
        
        decfsz    n,f
        goto      petla_czy_jest_0
        incf      FSR,f
;czy juz przeszly wszystkie linie (4 bajty odebrano)  
        decfsz    liczba1,f
        goto      petla_odbioru_ds1820
        
;jezeli minelo 60us i nic sie nie dzialo to przerwij odbieranie 
        ;btfsc    PIR1,TMR2IF
        movf     jak_duzo_bajtow_odbieram_z_ds,w
        movwf     n
        movlw    bajt_ds
        movwf    FSR
petla_wysylania_odebranych_bajtow        
        call    sprawdz_czy_mozna_wysylac
         
         swapf    INDF,w
         andlw    0x0f
         call     zamien_na_hex
         movwf   TXREG
         
         call    sprawdz_czy_mozna_wysylac
         
         movf     INDF,w
         andlw    0x0f
         call     zamien_na_hex
         movwf   TXREG     
         incf     FSR,f
         decfsz   n,f
         goto     petla_wysylania_odebranych_bajtow
         
         call    sprawdz_czy_mozna_wysylac
         movlw    znak_lf
         movwf   TXREG   
        goto     powrot_z_polecen
        
ustawienie_liczby_odbieranych_bajtow
         
         incf     FSR,f
         call     sprawdz_czy_odebrano_litere
         movf     tmp7,w
         sublw    0x0b
         btfss    STATUS,C
         goto     ustawienie_liczby_odbieranych_bajtow_zero
         
         movf     tmp7,w
         movwf    jak_duzo_bajtow_odbieram_z_ds
         goto     powrot_z_polecen
         
ustawienie_liczby_odbieranych_bajtow_zero
         movlw    0x04
         movwf    jak_duzo_bajtow_odbieram_z_ds
         goto     powrot_z_polecen
         
;zarejestrowalem 0
        
;tzn czy przez czas 8  bitow cos         
rozkazy_ds1820
        incf     FSR,f
        movlw    _i
        xorwf    INDF,w
        btfsc    STATUS,Z 
        goto      inicjacja_ds1820
        
        movlw    _s
        xorwf    INDF,w
        btfsc    STATUS,Z 
        goto     wysylanie_danych
        
        movlw    _b
        xorwf    INDF,w
        btfsc    STATUS,Z 
        goto     ustawienie_liczby_odbieranych_bajtow
        
        goto     powrot_z_polecen
        
;*********KOniec ds1820