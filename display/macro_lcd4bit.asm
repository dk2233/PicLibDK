;macros to generate and use lcd hitachi driver
;first call lcd_init macro alongside type of connection to micro (4bit/8bit), number of lines, fonts size
;the constraints are:
;data port is fully used by one of micro ports = wether is 8 bit or 4 bit
;in case of 4 bits - whole nibble will be occupied by data (lower or higher)


;send data to lcd 
m_lcd_send_data macro is_8_bit, tmp_lcd
;np PORTA bity 0-3  sa uzywane przez 4 bitowy ekran lcd
;najpierw wysylane sa 4bity gorne
      clear_port_lcd
   if (is_8_bit == 1)
      movwf   lcd_data
      movwf port_lcd
   else         
    IF (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze lcd_data musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
       swapf   lcd_data,w
       andlw  0x0f ; clear in higher nibble
		 addwf	port_lcd,f ;clear current lcd data in port 
    elif  (lcd_location_4_bit_in_H == 1)        
      movf   lcd_data,w 
      andlw  0xf0
		 addwf	port_lcd,f 
    ENDIF
         
       m_trigger_enable

       clear_port_lcd
  IF (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze lcd_data musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
    	 movf	 lcd_data,w
       andlw  0x0f
		 addwf	 port_lcd,f 
  elif (lcd_location_4_bit_in_H == 1)        
;jesli linia LCD jest podlaczona do gornych linii portu LCD nie na razie nie obracam            
       swapf   lcd_data,w
       andlw  0xf0 ; clear in higher nibble
		 addwf	port_lcd,f ;clear current lcd data in port 
   ENDIF

   endif      
      m_trigger_enable
      endm

m_trigger_enable macro 
      bsf      port_lcd_e,enable
      nop
      nop
      bcf      port_lcd_e,enable

      m_check_busy  tmp_lcd
   endm 
         
         
         
         
;funkcja pisz?ca na ekranie
m_write_lcd  macro is_8_bit
         bsf      port_lcd_rs,rs         
         m_lcd_send_data  is_8_bit
         bcf      port_lcd_rs,rs
         return

;funkcja czyszcz?ca ekran

m_cmd_off macro 
      bcf      port_lcd_rs,rs
      endm
         


;place into n_letters
;place address into Wreg
m_clear_line macro    lcd_data, n_letters
   local m_clear_line_loop
      movwf    lcd_data
      call     func_name_send_data
      
m_clear_line_loop
      movlw    lcd_space
      movwf    lcd_data
      call     func_write_data
      
      decfsz   n_letters,f
      goto     clear_line_loop
      
      m_cmd_offcmd_off
        
      endm

         
m_check_busy macro  tmp_lcd
if (lcd_RW_operates == lcd_RW)
      local m_check_loop 
      clear_port_lcd
         banksel tris_lcd         
         movf     tris_lcd,w
         movlw    lcd_tris_check_busy_mask
         andwf    tris_lcd,f
         
         banksel port_lcd      
                  
         bsf      port_lcd_rw,rw
         bcf      port_lcd_rs,rs

m_check_loop
         bsf      port_lcd_e,enable   
      IF (lcd_data_8bit == 1)
         movf port_lcd,w         
         movwf    tmp_lcd
      ELIF (lcd_location_4_bit_in_H == 0) 
         swapf    port_lcd,w
         andlw    0xf0   
         movwf    tmp_lcd
         bcf      port_lcd_e,enable         
         bsf      port_lcd_e,enable
         movf    port_lcd,w
         andlw   0x0f
         addwf   tmp_lcd,f
      ELIF (lcd_location_4_bit_in_H == 1)        
         movf    port_lcd,w
         andlw    0xf0   
         movwf    tmp_lcd
         bcf      port_lcd_e,enable         
         bsf      port_lcd_e,enable
         swapf    port_lcd,w
         movf    port_lcd,w
         andlw    0x0f
         addwf    tmp_lcd,f         
      ENDIF
                  
         
         bcf      port_lcd_e,enable         
            
         btfsc    tmp_lcd,7
         goto     m_check_loop
         
         banksel tris_lcd
         movlw   lcd_tris_normal_mask
         andwf    tris_lcd,f
         
         banksel port_lcd         
         
         bcf      port_lcd_rw,rw
         bcf      port_lcd_rs,rs
else 
       wait_specific_time_no_tmr_us  mcu_freq, lcd_wait_time_after_cmd, tmp_lcd  
endif 
      endm


clear_port_lcd  macro 
   if (lcd_data_8bit == 1)
   else 
   if (lcd_location_4_bit_in_H == 0) 
      movlw   0xf0 
      andwf port_lcd,f 
      movlw    (func_set_value >> 4)
      iorwf    port_lcd,f 
   elif (lcd_location_4_bit_in_H == 1)        
      movlw   0x0f 
      andwf port_lcd,f 
      movlw    func_set_value 
      iorwf   port_lcd,f
   endif
   endif


   endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                           

lcd_set_8_bit    equ  1
lcd_set_4_bit    equ  0
lcd_set_2_lines  equ  1
lcd_set_1_lines  equ  0
lcd_set_5_10_fonts equ  1
lcd_set_5_8_fonts equ  0

lcd_cmd_function_set   equ  b'0010'
#define lcd_cmd_8_bit_pos  .4
#define lcd_cmd_NL_pos     .3
#define lcd_cmd_fonts_size_pos    .2

#define lcd_RW_operates  1 
#define lcd_RW_grounded   0
#define lcd_wait_time_after_cmd    .37

#define lcd_init_8bit	 b'00110000'

m_lcd_init macro  is_8_bit, number_of_lines, character_fonts, is_RW_used, lcd_tmp_regL
   local func_set_value = lcd_cmd_function_set | (is_8_bit << lcd_cmd_8_bit_pos ) | (1 << lcd_cmd_NL_pos ) | (1 << lcd_cmd_fonts_size_pos)
         m_cmd_off

      if (is_8_bit == 1)
         #define lcd_data_8bit  1
         #define lcd_tris_normal_state_mask  0x0 ;and this value with tris
         #define lcd_tris_check_busy_mask    0xff  ;add this value to tris
      else 
         #define lcd_data_4bit 1
         if (lcd_location_4_bit_in_H == 1)
            #define lcd_tris_normal_state_mask  0x0f ;add + with tris this value and as a result normal tris for lcd is set
            #define lcd_tris_check_busy_mask    0xf0 ;and & this value to tris 
         else 
            #define lcd_tris_normal_state_mask  0xf0 ; and &
            #define lcd_tris_check_busy_mask    0x0f ; add
         endif
      endif

         #define lcd_RW_usage  lcd_RW_operates

      endif 
      wait_specific_time_no_tmr_us   mcu_freq, .41000, lcd_tmp_regL ; wait for more than 40 ms

         clear_port_lcd
      if (is_8_bit == 1)
         movlw   lcd_init_8bit ; 8 bit data transfer - whole one port use for DB0-DB7 data 
	      movwf	  port_lcd,f

      else 

         IF (lcd_location_4_bit_in_H == 0) 
      ;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
      ;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze lcd_data musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
            movlw (lcd_init_8bit >> 4)
            andlw  0x0f ; clear in higher nibble
            addwf	port_lcd,f ;clear current lcd data in port 
         elif  (lcd_location_4_bit_in_H == 1)        
            movf   lcd_data,w 
            andlw  0xf0
            addwf	port_lcd,f 
         ENDIF
   endif

   m_trigger_enable 
   wait_specific_time_no_tmr_us mcu_freq, .4200, lcd_tmp_regL     
         
         
   m_trigger_enable 
         
   wait_specific_time_no_tmr_us mcu_freq, .100, lcd_tmp_regL     

   m_trigger_enable 

    clear_port_lcd
   if (is_8_bit == 1)
      movlw func_set_value
      movwf	  port_lcd,f
      m_trigger_enable
   else 
   if (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze lcd_data musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
      movlw    (func_set_value >> 4)
      addwf    port_lcd,f 
   elif (lcd_location_4_bit_in_H == 1)        
      movlw    func_set_value 
      addwf   port_lcd,f
   endif
   m_trigger_enable
   ENDIF
         
      call     func_check_busy

        movlw   set_4bit
        call     func_name_send_data
         
        movlw    display_set
        call     func_name_send_data
        
        movlw    display_clear
        call     func_name_send_data
        
        movlw   set_entry
        call     func_name_send_data
        endm