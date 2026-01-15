
wait
        
        btfss   INTCON,T0IF      
        goto    wait
        bcf     INTCON,T0IF      
        return
        
         
;funkcja wysy?aj?ca
;LCD
;;;;;;;;;;;;;;;;;;;;;;;;;

send
         movwf    port_lcd
         
         bsf      port_lcd_e,enable
         nop
         bcf      port_lcd_e,enable

              
         return

;funkcja pisz?ca na ekranie
write_lcd

         
         bsf      port_lcd_rs,rs
         
         call     send
         
         bcf      port_lcd_rs,rs    
         return

;funkcja czyszcz?ca ekran

;funkcja czyszcz?ca ekran

cmd_off

         
         bcf      port_lcd_rs,rs
         
         
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
         clrf     port_lcd
         bsf      STATUS,RP0        ;bank 1
        movlw   b'11111111'
         movwf    tris_lcd
         
         bcf      STATUS,RP0
                  
         bsf      port_lcd_rw,rw
         bcf      port_lcd_rs,rs

check
         
         bsf      port_lcd_e,enable
         
         bcf      port_lcd_e,enable

         btfsc    port_lcd,7
         goto     check
         bsf      STATUS,RP0        ;bank 1
         movlw   b'00000000'
         movwf    tris_lcd
         bcf      STATUS,RP0
         
         bcf      port_lcd_rw,rw
         bcf      port_lcd_rs,rs
         return



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                           
;pocz?tek dzia?ania ekranu

lcd_init
         call     cmd_off
         clrf     port_lcd
;wylaczam przerwanie         
        
         clrf     TMR0
         bcf    INTCON,T0IE
;wylaczam przerwania tmr0         
        
         call     wait     ;64ms
         
         movlw    set_8bit
         movwf    port_lcd
         call     send    ;za??czam
         clrf     TMR0 ;64 ms
         call     wait
        
         movlw   display_on
         movwf    port_lcd
         call     send
         
         clrf   TMR0;64 ms
         call     wait
         
         movlw    display_clear
         movwf    port_lcd
         call     send
        
        clrf   TMR0;64 ms
         call     wait

         ;call    check_busy
         movlw   entry
        movwf   port_lcd
        call    send
        return