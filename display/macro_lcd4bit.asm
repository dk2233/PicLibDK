;funkcja zamieniajaca dane 8 bitowe na dwie polowki 4bitowe
;wysylane po kolei
;wersja 2011-03-09

wait
        
        btfss   INTCON,T0IF      
        goto    wait
        bcf     INTCON,T0IF      
        return
        
         
;funkcja wysy?aj?ca
;LCD
;;;;;;;;;;;;;;;;;;;;;;;;;


;przed wywolaniem funkcji umiesc w Wreg dane do wyslania
send4bit
send
;np PORTA bity 0-3  sa uzywane przez 4 bitowy ekran lcd
;najpierw wysylane sa 4bity gorne

         movwf    dane_lcd
         
         ;bsf      port_lcd_e,enable
         
      IF (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze dane_lcd musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
         swapf    dane_lcd,f
		 movlw	0xf0
		 andwf	 port_lcd,f 
      ENDIF
            
      IF (lcd_location_4_bit_in_H == 1)        
;jesli linia LCD jest podlaczona do gornych linii portu LCD nie na razie nie obracam
            
         movlw	0x0f
		 andwf	 port_lcd,f 
      ENDIF
		  

         movlw    ktore_bity_lcd_tris
         andwf    dane_lcd,w
         addwf    port_lcd,f
         
         bsf      port_lcd_e,enable
         nop
         nop
         bcf      port_lcd_e,enable

         

;jesli linia LCD jest podlaczona do gornych linii portu LCD, to wysylajac w drugiej kolejnosci mlodsze bity
;musze teraz obrcocic polowki bajtu, by dolne bity byly w miejscy starszych
         swapf    dane_lcd,f

  IF (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze dane_lcd musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
    	 movlw	0xf0
		 andwf	 port_lcd,f 
  ENDIF
            
      IF (lcd_location_4_bit_in_H == 1)        
;jesli linia LCD jest podlaczona do gornych linii portu LCD nie na razie nie obracam            
       movlw	0x0f
		 andwf	 port_lcd,f 
      ENDIF

         movlw    ktore_bity_lcd_tris
         andwf    dane_lcd,w
         addwf    port_lcd,f
         
         bsf      port_lcd_e,enable
         nop
         nop
         bcf      port_lcd_e,enable
            
         
         return

         
         
         
         
         
         
         
;funkcja pisz?ca na ekranie
m_write_lcd  macro port_lcd
         bsf      port_lcd_rs,rs         
         call     send4bit         
         bcf      port_lcd_rs,rs
         return

;funkcja czyszcz?ca ekran

m_cmd_off macro 
      bcf      port_lcd_rs,rs
      endm
         


;funkcja czysci cala linie
; przed jej wywolaniem w W umi#1esc adres linii
;po zakonczeniu z powrotem w niej umieszcza adres
;stara wersja - dziala na cala linie

clear_line_old
         movwf    n
         bcf      port_lcd_rs,rs
         
         movf     n,w
         movwf    tmp_lcd
         call     send
         movlw    ile_znakow
         movwf    tmp7
         addlw    0xff
         movwf    tmp
clear_line_old2
         call     check_busy4bit
         movlw    _puste
         movwf    tmp_lcd
         call     write_lcd
         
         decfsz   tmp7,f
         goto     clear_line_old2
         
         call     cmd_off
         movf     n,w
         return
         
clear_line
;przed wywolaniem do n trzeba wrzucic ilosc kasowanych znakow
;a w W musi byc adres linii         
         movwf    dane_lcd
         call     send
         
clear_line_petla
         call     check_busy4bit
         movlw    _puste
         movwf    dane_lcd
         call     write_lcd
         
         decfsz   n,f
         goto     clear_line_petla
         
         call     cmd_off
        
         return

         







         
m_check_busy4bit port_lcd, 

      IF (lcd_location_4_bit_in_H == 1)        
;czyszcze bity gorne, a dolne bez zmian
         movlw    0x0f         
         andwf    port_lcd,f
      ENDIF


      IF (lcd_location_4_bit_in_H == 0)        
;czyszcze bity dolne
         movlw    0xf0         
         andwf    port_lcd,f
      ENDIF
       
         banksel tris_lcd
         
         movf     tris_lcd,w
         iorlw    ktore_bity_lcd_tris
         movwf    tris_lcd    
         
         banksel port_lcd      
                  
         bsf      port_lcd_rw,rw
         bcf      port_lcd_rs,rs

check
         
         bsf      port_lcd_e,enable   
         
      IF (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest najpierw (starsze) trzeba przesunac na starsze bity
         swapf    port_lcd,w
         andlw    0xf0   
      ENDIF
            
      IF (lcd_location_4_bit_in_H == 1)        
;jesli na gorze bitow port d
            
         movf    port_lcd,w
         andlw    0xf0   
      ENDIF
                  
         movwf    tmp_lcd
         
         bcf      port_lcd_e,enable         
         bsf      port_lcd_e,enable
         
      IF (lcd_location_4_bit_in_H == 1) 
;jesli na gorze bitow port d
         swapf    port_lcd,w
      ENDIF
            
      IF (lcd_location_4_bit_in_H == 0)            
         movf    port_lcd,w
      ENDIF         
         
         bcf      port_lcd_e,enable    
         ;bo mlodsza polowke zapisuje
         andlw    0x0f
         
         addwf    tmp_lcd,f         
            
         btfsc    tmp_lcd,7
         goto     check
         
         banksel tris_lcd
         movlw   normalne_ustawienie_tris_lcd
         movwf    tris_lcd
         
         banksel 0         
         
         bcf      port_lcd_rw,rw
         bcf      port_lcd_rs,rs
         return



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                           
;pocz?tek dzia?ania ekranu

lcd_init
         m_cmd_off
         clrf     port_lcd

      wait_specific_time_no_tmr_us   mcu_freq, .40000, tmp7 ; wait 40 ms

     IF (lcd_location_4_bit_in_H == 0) 
         movlw    b'00000011'
	      movwf	  port_lcd,f
      ENDIF
           
 
      IF (lcd_location_4_bit_in_H == 1)        
;jesli linia LCD jest podlaczona do gornych linii portu LCD nie na razie nie obracam
         movlw    b'00110000'
         addwf	  port_lcd,f         
      ENDIF

 	bsf      port_lcd_e,enable
         nop
         nop
         bcf      port_lcd_e,enable
        
        
         
         
         clrf     TMR0 ;64 ms
 	bcf    INTCON,T0IE
	bcf	INTCON,T0IF
         call     wait
         
         clrf     TMR0 ;64 ms
 	bcf    INTCON,T0IE
	bcf	INTCON,T0IF
         call     wait
         
         decfsz   tmp,f
         goto     lcd_init_3_petla

         IF (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze dane_lcd musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
         movlw    b'00000010'
	addwf	  port_lcd,f

      ENDIF
           
 
      IF (lcd_location_4_bit_in_H == 1)        
;jesli linia LCD jest podlaczona do gornych linii portu LCD nie na razie nie obracam
         movlw    b'00100000'
         addwf	  port_lcd,f
            
         
      ENDIF
         
         
         
         
         bsf      port_lcd_e,enable
         nop
         nop
         bcf      port_lcd_e,enable
         
         clrf     TMR0 ;64 ms
         call     wait       
         

		call     check_busy4bit

         movlw   set_4bit
         
         call     send4bit
         
         call     check_busy4bit
         
         ;movlw    display_clear
         
         movlw    display_set
         
         
         call     send4bit
        
        call     check_busy4bit
        
         movlw    display_clear
         call     send4bit
        
        call     check_busy4bit
        
         movlw   set_entry
         
         call     send4bit
        
        call     check_busy4bit
        return