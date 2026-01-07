
lcd_config_lcdcon  macro   clock_bits, lmux_bits
    local x = (clock_bits << CS0)
    local y = lmux_bits;

    BANKSEL  LCDCON
    movlw y | x
    movwf LCDCON
    bsf   LCDCON,LCDEN
    bsf   LCDCON, SLPEN 
    bsf   LCDCON, VLCDEN
    endm

lcd_config_lcdps macro wft_bit, biasmd_bit, lp_bits
    local x = lp_bits 
x = x | (wft_bit << WFT)
x = x | (biasmd_bit << BIASMD )
x = x | (1 << LCDA)
    movlw x 
    BANKSEL LCDPS
    movwf LCDPS
    

    endm 


lcd_config_lcdsegments  macro  reg, byte

    
    BANKSEL reg 
    movlw byte 
    movwf reg
    endm

config_lcd_off  macro 
   
       ;disable LCD module 
        BANKSEL  LCDCON
        clrf LCDCON

;all segments pins set as digital pins 
	clrf	LCDSE0
	clrf	LCDSE1

    #IFDEF   __16F914 
	clrf	LCDSE2
    #ENDIF
    #IFDEF   __16F917 
	clrf	LCDSE2
    #ENDIF
    endm