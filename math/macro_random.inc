
rand_8bit_init macro rand_reg
    local rand_8bit_init_exit

    movf rand_reg,w 
    SKPZ 
    goto rand_8bit_init_exit
    movlw 0xa5 
    movwf rand_reg

rand_8bit_init_exit
    endm 
;taken from
;http://web.archive.org/web/20221204050945/http://www.piclist.com/techref/microchip/rand8bit.htm
rand_8bit macro rand_reg
    RLF     rand_reg,W
    RLF     rand_reg,W
    BTFSC   rand_reg,4
    XORLW   1
    BTFSC   rand_reg,5
    XORLW   1
    BTFSC   rand_reg,3
    XORLW   1
    MOVWF   rand_reg

    endm


GET_RANDOM macro RANDOM_SEED
    MOVF    RANDOM_SEED, W
    BTFSC   RANDOM_SEED, 3
    ADDLW   0x47
    BTFSS   RANDOM_SEED, 5
    ADDLW   0x8B
    XORWF   RANDOM_SEED, W
    MOVWF   RANDOM_SEED
    endm