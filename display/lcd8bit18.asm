;wersja 2007-06-07
;dla prockow serji 18fxxx
;do odczekania 64ms uzywam komendy cpfsgt
wait
         
        ;bsf      portlampka,lampka              
IF (procek == 18f442)
        bcf     INTCON,T0IF
wait_petla                   
        btfss   INTCON,T0IF      
        goto    wait_petla
        bcf     INTCON,T0IF            
ENDIF
IF (procek == 18f4320)
         bcf     INTCON,TMR0IF
wait_petla         
        btfss   INTCON,TMR0IF      
        goto    wait_petla
        bcf     INTCON,TMR0IF
        bcf      INTCON,TMR0IE
              
ENDIF        
        ;bcf      portlampka,lampka              
        return
        
         
;funkcja wysy?aj?ca
;LCD
;;;;;;;;;;;;;;;;;;;;;;;;;

send
         
         movwf    latch_lcd
         ;movwf    PORTD
         
         bsf      latch_lcd_e,enable
         nop
         bcf      latch_lcd_e,enable

              
         return

;funkcja pisz?ca na ekranie
write_lcd

         
         bsf      latch_lcd_rs,rs
         ;bsf      PORTB,rs
         
         call     send
         
         bcf      latch_lcd_rs,rs    
         ;bcf      PORTB,rs
         return

;funkcja czyszcz?ca ekran

;funkcja czyszcz?ca ekran

cmd_off

         
         bcf      latch_lcd_rs,rs
         
         
        return
         


;funkcja czysci cala linie
; przed jej wywolaniem w W umi#1esc adres linii
;po zakonczeniu z powrotem w niej umieszcza adres
;stara wersja - dziala na cala linie

clear_line_old
         movwf    n
         bcf      port_lcd_rs,rs
         
         movf     n,w
         movwf    dane_lcd
         call     send
         movlw    ile_znakow
         movwf    tmp7
         addlw    0xff
         movwf    tmp
clear_line_old2
         call     check_busy
         movlw    _puste
         movwf    dane_lcd
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
         call     check_busy
         movlw    _puste
         movwf    dane_lcd
         call     write_lcd
         
         decfsz   n,f
         goto     clear_line_petla
         
         call     cmd_off
        
         return

         
         
check_busy
         clrf     latch_lcd
         clrf     port_lcd
         ;clrf     PORTD
         
         movlw   b'11111111'
         movwf    tris_lcd
         
         
                  
         bsf      latch_lcd_rw,rw
         bcf      latch_lcd_rs,rs

check
         
         bsf      latch_lcd_e,enable
         
         bcf      latch_lcd_e,enable

         btfsc    latch_lcd,7
         goto     check
         
         movlw   b'00000000'
         movwf    tris_lcd
         
         
         bcf      latch_lcd_rw,rw
         bcf      latch_lcd_rs,rs
         return



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                           
;pocz?tek dzia?ania ekranu

lcd_init
         call     cmd_off
         clrf     port_lcd
         
;wylaczam przerwanie         
        
         clrf     TMR0L
IF (procek == 18f442)
         bcf      INTCON,T0IE
         
ENDIF
IF (procek == 18f4320)
         bcf      INTCON,TMR0IE
         
         
ENDIF         
         
;wylaczam przerwania tmr0         
        
         call     wait     ;64ms
         
         movlw    set_8bit
         ;movwf    port_lcd,ACCESS
         call     send    ;za??czam
         clrf     TMR0L ;64 ms
         call     wait
        
         movlw   display_on
         ;movwf    port_lcd,ACCESS
         call     send
         
         clrf   TMR0L;64 ms
         call     wait
         
         movlw    display_clear
         ;movwf    port_lcd,ACCESS
         call     send
        
        clrf   TMR0L;64 ms
         call     wait

         ;call    check_busy
         movlw   entry
        ;movwf   port_lcd,ACCESS
        call    send
        return