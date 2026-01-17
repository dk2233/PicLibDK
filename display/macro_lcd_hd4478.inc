;macros to generate and use lcd hitachi driver
;first call lcd_init macro alongside type of connection to micro (4bit/8bit), number of lines, fonts size
;the constraints are:
;data port is fully used by one of micro ports = wether is 8 bit or 4 bit
;in case of 4 bits - whole nibble will be occupied by data (lower or higher)


#define display_clear	b'00000001'
#define lcd_set_entry b'00000100'

;send data to lcd 
m_lcd_send_data macro lcd_data_to_be_send
;np PORTA bity 0-3  sa uzywane przez 4 bitowy ekran lcd
;najpierw wysylane sa 4bity gorne
      clear_port_lcd
   if (lcd_data_8bit == 1)
      movwf lcd_port_data
   else         
      movwf  lcd_data_to_be_send
    IF (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze lcd_data musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
       swapf   lcd_data_to_be_send,w
       andlw  0x0f ; clear in higher nibble
		 addwf	lcd_port_data,f ;clear current lcd data in port 
    elif  (lcd_location_4_bit_in_H == 1)        
      movf   lcd_data_to_be_send,w 
      andlw  0xf0
		addwf	lcd_port_data,f 
    ENDIF
         
       m_trigger_enable

       clear_port_lcd
  IF (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze lcd_data musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
    	 movf	 lcd_data_to_be_send,w
       andlw  0x0f
		 addwf	 lcd_port_data,f 
  elif (lcd_location_4_bit_in_H == 1)        
;jesli linia LCD jest podlaczona do gornych linii portu LCD nie na razie nie obracam            
       swapf   lcd_data_to_be_send,w
       andlw  0xf0 ; clear in higher nibble
		 addwf	lcd_port_data,f ;clear current lcd data in port 
   ENDIF

   endif      
      m_trigger_enable
      endm

m_trigger_enable macro  
      bsf      port_lcd_e,lcd_enable_pin
      nop
      nop
      bcf      port_lcd_e,lcd_enable_pin

   endm 
         
         
m_trigger_enable_with_check  macro tmp_lcd
   m_trigger_enable

   m_check_busy  tmp_lcd
   endm 
         
;funkcja pisz?ca na ekranie
m_write_lcd  macro  lcd_data_to_be_send
         bsf      port_lcd_rs,lcd_RS_pin         
         m_lcd_send_data  lcd_data_to_be_send
         bcf      port_lcd_rs,lcd_RS_pin
         endm

;funkcja czyszcz?ca ekran

m_cmd_off macro 
      bcf      port_lcd_rs,lcd_RS_pin
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
if (lcd_RW_operates == lcd_RW_usage)
      local m_check_loop 
      clear_port_lcd
         banksel lcd_TRIS_data         
         movf     lcd_TRIS_data,w
         addlw    lcd_tris_check_busy_mask
         movwf    lcd_TRIS_data
         
         banksel lcd_port_data      
         
         bsf      port_lcd_rw,lcd_RW_pin
         bcf      port_lcd_rs,lcd_RS_pin

m_check_loop
         banksel port_lcd_e
         bsf      port_lcd_e,lcd_enable_pin   
      IF (lcd_data_8bit == 1)
         movf lcd_port_data,w
         banksel tmp_lcd         
         movwf    tmp_lcd
      ELIF (lcd_location_4_bit_in_H == 0) 
         swapf    lcd_port_data,w
         andlw    0xf0   
         banksel tmp_lcd         
         movwf    tmp_lcd
         banksel port_lcd_e         
         bcf      port_lcd_e,lcd_enable_pin         
         bsf      port_lcd_e,lcd_enable_pin
         movf    lcd_port_data,w
         andlw   0x0f
         banksel tmp_lcd         
         addwf   tmp_lcd,f
      ELIF (lcd_location_4_bit_in_H == 1)        
         movf    lcd_port_data,w
         andlw    0xf0   
         banksel tmp_lcd         
         movwf    tmp_lcd
         banksel port_lcd_e
         bcf      port_lcd_e,lcd_enable_pin         
         bsf      port_lcd_e,lcd_enable_pin
         swapf    lcd_port_data,w
         movf    lcd_port_data,w
         andlw    0x0f
         banksel tmp_lcd         
         addwf    tmp_lcd,f         
      ENDIF
                  
         banksel port_lcd_e 
         bcf      port_lcd_e,lcd_enable_pin         
            
         banksel  tmp_lcd   
         btfsc    tmp_lcd,7
         goto     m_check_loop
         
         banksel lcd_TRIS_data
         movlw   lcd_tris_normal_state_mask
         andwf    lcd_TRIS_data,f
         
         banksel lcd_port_data      
         bcf      port_lcd_rw,lcd_RW_pin
         bcf      port_lcd_rs,lcd_RS_pin
else 
       wait_specific_time_no_tmr_us  mcu_freq, lcd_wait_time_after_cmd, tmp_lcd  
endif 
      endm


clear_port_lcd  macro 
      BANKSEL lcd_port_data
   if (lcd_data_8bit == 1)
   else 
   if (lcd_location_4_bit_in_H == 0) 
      movlw   0xf0 
      andwf lcd_port_data,f 
      movlw    (func_set_value >> 4)
      iorwf    lcd_port_data,f 
   elif (lcd_location_4_bit_in_H == 1)        
      movlw   0x0f 
      andwf lcd_port_data,f 
      movlw    func_set_value 
      iorwf   lcd_port_data,f
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

#define lcd_entry_inc_pos     1
#define lcd_entry_shift_pos   0
#define lcd_disp_inc       1
#define lcd_disp_dec       0
#define lcd_disp_shift     1
#define lcd_cursor        0


#define lcd_init_8bit	 b'00110000'

m_lcd_init macro  is_8_bit, number_of_lines, character_fonts, set_increment, display_shift, is_RW_used, lcd_tmp_regL
   local func_set_value = lcd_cmd_function_set | (is_8_bit << lcd_cmd_8_bit_pos ) | (1 << lcd_cmd_NL_pos ) | (1 << lcd_cmd_fonts_size_pos)
   local set_entry_value = lcd_set_entry | (set_increment << lcd_entry_inc_pos) | (display_shift << lcd_entry_shift_pos) 
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
         elif (lcd_location_4_bit_in_H == 0) 
            #define lcd_tris_normal_state_mask  0xf0 ; and &
            #define lcd_tris_check_busy_mask    0x0f ; add
         else 
            ERROR "no lcd_data location defined"
         endif
      endif 
         #define lcd_RW_usage  lcd_RW_operates
      wait_specific_time_no_tmr_us   mcu_freq, .41000, lcd_tmp_regL ; wait for more than 40 ms

         clear_port_lcd
      if (is_8_bit == 1)
         movlw   lcd_init_8bit ; 8 bit data transfer - whole one port use for DB0-DB7 data 
	      movwf	  lcd_port_data

      else 

         IF (lcd_location_4_bit_in_H == 0) 
      ;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
      ;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze lcd_data musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
            movlw (lcd_init_8bit >> 4)
            andlw  0x0f ; clear in higher nibble
            addwf	lcd_port_data,f ;clear current lcd data in port 
         elif  (lcd_location_4_bit_in_H == 1)        
            movlw lcd_init_8bit
            andlw  0xf0
            addwf	lcd_port_data,f 
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
      movwf	  lcd_port_data
      m_trigger_enable_with_check lcd_tmp_regL
   else 
   if (lcd_location_4_bit_in_H == 0) 
;jezli linia LCD jest na dolnych bitach portu przypisanego do LCD, wtedy to co jest
;wysylane musi byc najpierw starsze wyslane, wiec to co jest w rejestrze lcd_data musi byc obrocone tak by starsze bity znalazly sie na miejscy mlodszych (dolnych)
      movlw    (func_set_value >> 4)
      addwf    lcd_port_data,f 
   elif (lcd_location_4_bit_in_H == 1)        
      movlw    func_set_value 
      addwf   lcd_port_data,f
   endif
   m_trigger_enable_with_check lcd_tmp_regL
   ENDIF
         
        movlw   func_set_value
        call     func_name_send_data
         
        ;movlw    display_set
        ;call     func_name_send_data
        
        movlw    display_clear
        call     func_name_send_data
        
        movlw   set_entry_value
        call     func_name_send_data
        endm