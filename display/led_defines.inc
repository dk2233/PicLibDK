
;     a
;  f |-|b
;    g-
;  e |_|c
;     d
;                  '.gfedcba'
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
_degree  equ      b'01100011'


;number to show specific symbol
led_minus     equ     0x14
led_H         equ     0x19 
led_S         equ     .19

led_dot_pin   equ  7


;led_dot_display
light_dot1      equ  0
light_dot10      equ  1
light_dot100    equ  2
light_dot1000   equ  3 
light_dot10000   equ  4 

;leds possible bits
led1              equ      0
led10             equ      1
led100            equ      2
led1000           equ      3
led10000          equ      4