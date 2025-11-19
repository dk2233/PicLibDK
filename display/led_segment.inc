;Daniel Kucharski 2025 
;led segment control driver written in gpasm
;please use it with care :-)
;
; please define following variables

;leds_common 
;- variable that holds which digit is now on

;led_state 
;holds status of led controll 

;value to be set for each of segment indyvidually
;number of it depeneds of define  LED_SEGMENT 
;segment_digit1
;segment_digit10
;segment_digit100
;segment_digit1000
;segment_digit10000 

;LED_SEGMENT shoule be assing number of used digits 



;procedures requires to create additional procedure 

;translate_value_to_port_pins

;this will be call by refresh_led
;in order to set digit output pin
; the easiest will be something like 
;   movf  value_for_one_digit_segment,w
;   movwf port_led

;however not always one digit will be connected to full port
;sometimes it is better to use some pins of each of ports available
;led_example.asm shows as an example situation with 4 lower bits attached to port_led_L (portC)
;and  4 higher to PORTB
;


;to use this library properly call
;led_digit_init  somewhere in the beginning of your program


;also call refresh_led every couple of ms
;



;     a
;  f |-|b
;    g-
;  e |_|c
;     d
;                   'gfedcba'
_0       equ      b'00111111'
_1       equ      b'00000110' ; bc;;
_2       equ      b'01011011';;
_3       equ      b'01001111';;        
_4       equ      b'01100110';bcgf;;
_5       equ      b'01101101';acdefg;;
_6       equ      b'01111101';        
_7       equ      b'00000111'
_8       equ      b'01111111'
_9       equ      b'01101111'
_A       equ      b'01110111';;
_b       equ      b'01111100'
_c       equ      b'01011000';;
_d       equ      b'01011110'
_e       equ      b'01111001'
_F       equ      b'01110001'
_p       equ      b'01110011'
_null    equ      b'00000000'
_U       equ      b'00111110'
_S       equ      b'01101101'
_minus   equ      b'01000000'
_X       equ      b'01110110'
_t       equ      b'01111000'
_o       equ      b'01011100'
_L       equ      b'00111000'
_H      equ       b'01110110'
_r      equ       b'01010000'
_u      equ       b'00011100'



        


tab_cyfr
         addwf    PCL,f
         retlw    _0
         retlw    _1
         retlw    _2
         retlw    _3
         retlw    _4
         retlw    _5
         retlw    _6
         retlw    _7
         retlw    _8
         retlw    _9
         retlw     _A;10 -0xa
         retlw          _b
         retlw          _c
         retlw          _d
         retlw          _e;14 - e
         retlw          _F;15 - F
         retlw    _p       ;16 -0x10
         retlw    _null  ;17 -0x11
         retlw    _U ;18 -0x12
         retlw    _S ;19 -0x13
         retlw    _minus ;0x14
         retlw          _X;0x15
         retlw          _t;0x16
         retlw          _o;0x17
         retlw          _L;0x18
         retlw          _H ; 0x19
         retlw          _0 ;0x1a
         retlw          _r ;0x1b
         retlw          _u ;0x1c


m_led_common_off macro number_of_digits
    LOCAL n_digit = number_of_digits , x = number_of_digits, divider = 1
    
    while ( x > 1 )
divider = divider * .10
x = x - 1
    endw
        while (n_digit > 0)
         bcf      segment_digit#v(divider)_port,segment_digit#v(divider)b
n_digit = n_digit - 1
divider = divider / .10;

         ;bcf      segment_digit1_port,segment_digit1b
         ;bcf      segment_digit10_port,segment_digit10b
         ;bcf      segment_digit100_port,segment_digit100b
         ;bcf      segment_digit1000_port,segment_digit1000b
        endw 
    endm


m_led_check_highest_digit macro number_of_digits
    LOCAL n_digit = number_of_digits , x = number_of_digits, divider = 1
    
    while ( x > 1 )
divider = divider * .10
x = x - 1
    endw
         btfss      leds_common,led#v(divider)

         ;bcf      segment_digit1_port,segment_digit1b
         ;bcf      segment_digit10_port,segment_digit10b
         ;bcf      segment_digit100_port,segment_digit100b
         ;bcf      segment_digit1000_port,segment_digit1000b

     endm


m_led_common_handle  macro  number_of_digits 

    LOCAL n_digit = number_of_digits , x = number_of_digits, divider = 1
    
    while ( x > 1 )
divider = divider * .10
x = x - 1
    endw
        while (n_digit > 0)
         btfsc    leds_common,led#v(divider)
         bsf      segment_digit#v(divider)_port,segment_digit#v(divider)b
n_digit = n_digit - 1
divider = divider / .10;
        endw 

        ; btfsc    leds_common,led100
        ; bsf      segment_digit100_port,segment_digit100b
         
        ; btfsc    leds_common,led10
        ; bsf      segment_digit10_port,segment_digit10b
         
        ; btfsc    leds_common,led1
        ; bsf      segment_digit1_port,segment_digit1b
         endm

m_led_add_number_to_FSR  macro number_of_digits

    LOCAL n_digit = number_of_digits , x = number_of_digits, divider = 1
    
    while ( x > 1 )
divider = divider * .10
x = x - 1
    endw
        while (n_digit > 0)
         btfsc    leds_common,led#v(divider)
         movlw  n_digit-1 ; add n_digit-1
n_digit = n_digit - 1
divider = divider / .10;
        endw 

         ;btfsc    leds_common,led1000
         ;movlw    3
         
         ;btfsc    leds_common,led100
         ;movlw    2
         
         ;btfsc    leds_common,led10
         ;movlw    1
         
         ;btfsc    leds_common,led1
         ;movlw    0        
         endm


common_leds_start_value equ (1 << (LED_SEGMENT-1))

;call this procedure
;with following variables
;segment_digit1   
;segment_digit10
;segment_digit100
;segment_digit1000
;segment_digit10000 - max value due to usage of division 24bits / 16 bits max - no possibility to divide by 1e5 
; put values to be visible
;for example to see 1234
;place   1 in segment_digit1000, 2 in segment_digit100, 3 in segment_digit10, 4 in segment_digit1
;for example - every 10 ms - 100Hz however for 4 digits will be 25 Hz
refresh_led
         BANKSEL segment_digit1
         BANKISEL segment_digit1
         ;at first disable all cathode 
         m_led_common_off LED_SEGMENT
         
         ;change digit to be shown - only one at a time
         bcf      STATUS,C
         rrf      leds_common,w    ;it could be also rlf however it is easier to detect when we reach restart position - when C bit is set
         btfsc    STATUS,C
         movlw    common_leds_start_value

         movwf    leds_common
         
         ;first digit to FSR
         movlw    segment_digit1
         
refresh_led1      
         movwf    FSR
         
         m_led_check_highest_digit LED_SEGMENT
         goto     refresh_led_next
        ;if letter is 1000 and it is 0 then do not show at all
        ;0100 should be visible as 100
         bsf      led_state,previous_0
         
refresh_led_next

         m_led_add_number_to_FSR LED_SEGMENT
         addwf    FSR,f
         ;this is like pointer in C - we will points to segment_digit1000, or segment_digit100 ...
         movf     INDF,w   
         
         btfss    STATUS,Z       
         goto     refresh_led_not_0

         btfsc   led_state,disable_show
         goto     refresh_led_not_show
;check if previous was 0 or it is first 1000
         btfss    led_state,previous_0
         goto     refresh_led_not_0
;sprawdz czy nie jest to segment_digit 1 bo wtedy cos jednak trzeba wyswietlic
         btfsc    leds_common,led1
         goto     refresh_led_not_0

refresh_led_not_show
         clrw 
         goto  send_to_leds 

refresh_led_not_0
        BANKSEL led_state
         bcf      led_state,previous_0
        

         movf     INDF,w   
         PAGESEL  tab_cyfr
         call     tab_cyfr

            
send_to_leds
         BANKSEL  value_for_one_digit_segment
         movwf    value_for_one_digit_segment

         btfsc    led_state,disable_show
         clrf     value_for_one_digit_segment


         ;split bits between ports to control LED pins
         ;for example = 4 lower bits of LED a,b,c ,d are connected to 0-3 pins of portb
         ;and e,f,g,h are connected to 0-3 pins of port a
         
         PAGESEL translate_value_to_port_pins
         call     translate_value_to_port_pins

         
zalacz_katody     
         BANKSEL leds_common

         m_led_common_handle LED_SEGMENT
        clrf             PCLATH         
         return 
         

;this procedure will translate          
;to number_l and number_h place value to be shown - max 2**16 value
split_number_to_digits 
        ;macro_16bits_into_4dec  number_l, segment_digit1 

        macro_16bits_into_N_dec number_l, segment_digit1 , LED_SEGMENT

        return


;call this to intialize properly led digits procedures
led_digit_init
    BANKSEL leds_common
    movlw  common_leds_start_value
    movwf  leds_common
    return
