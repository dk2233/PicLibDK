 ;;ten dokument zawiera definicje wszystkich funkcji
;;uytych do wywietlania danych na LCD
;;wywietlanie 8 bitowe

;def lcd
;cay port RB0-RB7 uyty jako dane do LCD
;init
;to idzie na port zwizany z danymi



init_8bit	equ	b'00110000'
set_8bit 	equ	b'00111100' ; 8bit, 2 linie,font 5x8
function_set2	equ	b'00001000' ;ustawia 2 linie i 5x8 font
;init_4bit          equ      b'00100010' ;ustawia 2 linie i 5x8 font

display_off	equ	b'00001000' ;ustawia blinking,cursor,
entry		equ	b'00000110' ;increment
dd_addres_set	equ	b'10000000' ;od rb6 do rb0 wstawi�adres

display_clear	equ	b'00000001'
display_on	equ	b'00001100';bez kursora
display_2	equ	b'00001111'
write_4bit	equ	b'00010000' ;pisz na ekranie
write		equ	b'01000000' ;zacz rd6 - rs

disp_home	equ	b'00000010'


;;dla 4bit
function_set1_4b	equ	b'00000011' ;- ustawia 4 bit
function_set2_4b	equ	b'00001000' ;ustawia 2 linie i 5x8 font
display1_4b	equ	b'00000000'
display2_4b	equ	b'00001111' ;ustawia blinking,cursor,
entry1_4b		equ	b'00000000'
entry2_4b		equ	b'00001100' ;increment
display_clear1_4b	equ	b'00000000'
display_clear2_4b	equ	b'00000001'


_0		equ	b'00110000'
_1		equ	b'00110001'
_2		equ	b'00110010'
_3		equ	b'00110011'
_4		equ	b'00110100'
_5		equ	b'00110101'
_6		equ	b'00110110'
_7		equ	b'00110111'
_8		equ	b'00111000'
_9		equ	b'00111001'
_dwukropek	equ	b'00111010'
_puste		equ	b'00100000'
_kropka		equ	b'00101110'
_stopni		equ	b'11011111'
_minus		equ	0x2d
_plus		equ	0x2b
_A		equ	b'01000001'
_B		equ	b'01000010'
_C		equ	b'01000011'
_D		equ	b'01000100'
_E		equ	b'01000101'
_F		equ	b'01000110'
_G		equ	b'01000111'
_H		equ	b'01001000'
_I		equ	b'01001001'
_J		equ	b'01001010'
_K		equ	b'01001011'
_L		equ	b'01001100'
_M		equ	b'01001101'
_N		equ	b'01001110'
_O		equ	b'01001111'
_P		equ	b'01010000'
_Q		equ	b'01010001'
_R		equ	b'01010010'
_S		equ	b'01010011'
_T		equ	b'01010100'
_U		equ	b'01010101'
_V		equ	b'01010110'
_W		equ	b'01010111'
_X		equ	b'01011000'
_Y		equ	b'01011001'
_Z		equ	b'01011010'

_a		equ	b'01100001'
_b		equ	b'01100010'
_c		equ	b'01100011'
_d		equ	b'01100100'
_e		equ	b'01100101'
_f		equ	b'01100110'
_g		equ	b'01100111'
_h		equ	b'01101000'
_i		equ	b'01101001'
_j		equ	b'01101010'
_k		equ	b'01101011'
_l		equ	b'01101100'
_m		equ	b'01101101'
_n		equ	b'01101110'
_o		equ	b'01101111'
_p		equ	b'01110000'
_q		equ	b'01110001'
_r		equ	b'01110010'
_s		equ	b'01110011'
_t		equ	b'01110100'
_u		equ	b'01110101'
_v		equ	b'01110110'
_w		equ	b'01110111'
_x		equ	b'01111000'
_y		equ	b'01111001'
_z		equ	b'01111010'

_procent        equ     0x25
_star		equ	0x2a
_up		equ	b'01011110'
_przecinek	equ	0x2c
_mniejszosci      equ      0x3c
_wiekszosci       equ      0x3e
_linia_pionowa    equ         0x7c


linia_pierwsza	equ	0x80
linia_gorna	equ	0x80

linia_dolna	equ	0xc0
linia_druga	equ	0xc0

linia_trzecia	equ	0x90
linia_czwarta	equ	0xd0
;

