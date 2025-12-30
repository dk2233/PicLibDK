
ds18_read_power_supply 
         call     init_ds1820
         
         BANKSEL ds18_status
         btfss   ds18_status,ds18_init_ok 
         ;error no init finished properly
         RETURN
         
         disable_all_isr
         call ds18_match_or_skip_rom
         
         movlw   DS18b20_read_power_supply
         call     ds18_send_request_loop_send_byte_start
;read power type = 0 or 1
         call PIN_LO

         nop

         call PIN_HI 

         wait_specific_time_no_tmr_us  mcu_freq, .8, ds18_tmp 
         ;fill (nop), .2
        enable_all_isr
        BANKSEL port_ds18b20
        btfss   port_ds18b20, port_pin_ds18b20
        goto    ds18_read_power_supply_parasite 

        BANKSEL ds18_configuration

        bcf  ds18_configuration, ds18_parasite_power
        bsf  ds18_configuration, ds18_normal_power
        return 

ds18_read_power_supply_parasite
        BANKSEL ds18_configuration
        bsf  ds18_configuration, ds18_parasite_power
        bcf  ds18_configuration, ds18_normal_power
        return 
