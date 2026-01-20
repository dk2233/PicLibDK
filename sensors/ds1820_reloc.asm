
    include ../../PicLibDK/memory_operation_16f.inc
    include ../../PicLibDK/math/math_macros.inc
    include ../../PicLibDK/stacks/macro_stack_operation.inc
    include ../../PicLibDK/macro_time_common.inc
    include ../../PicLibDK/interrupts.inc
ds18_udata udata

ds18_config_byte   res 1 
ds18_configuration  res 1
ds18_number_of_bytes  res 1
ds18_command_to_be_send  res 1
ds18_id_bit  res  1
ds18_id_which_byte  res 1
ds18_id_RAM_offset res 1
ds18_number_of_sensors  res 1
ds18_CRC_result  res 1
ds18_status  res 1
ds18_measured_temp_01 res 1 
ds18_measured_temp    res 1
ds18_number_of_receive_bytes res 1 
ds18_received_id_bit_count  res 1
ds18_TH res 1 
ds18_TL res 1
ds18_expected_resolution res 1
n_bit  res 1 

;this has to be located in one of continous sections
ds18_ids udata
ds18_read_from_RAM res 9
ds18_read_id_1 res 8 
ds18_read_id_2 res 8 
ds18_read_id_3 res 8 
ds18_read_id_4 res 8 
ds18_read_id_5 res 8 
ds18_read_id_6 res 8 
ds18_read_id_7 res 8 
ds18_read_id_8 res 8 

    extern result_001, result_01, result_ll, result_lh , result_H 
    extern fraction_l, fraction_h, stack_sp, stack_of_differences 
    extern number_l, number_h
    extern operandl, operandh
    extern tmp7

    





    global ds18b20_start
    global ds18_read_id_1, ds18_read_id_2, ds18_read_id_3
    global ds18_read_from_RAM
    global ds18_status
    global ds18_CRC_result
    global value_for_one_digit_segment
    global ds18_search_rom_all_sensors 
    global init_ds1820, check_ds_processing, ds18_match_or_skip_rom, ds18_req_temp_convert
    global ds18_req_scratchpad_read, ds18_req_scratchpad_write, ds18_read_power_supply
    global ds18_req_id_read, ds18_search_rom, ds18_send_request_loop_send_byte_start
    global ds18_check_resolution
    global ds18_translate_measurements_to_dec, ds18_receive_data_from_sensor, check_CRC_DS
    global PIN_HI, PIN_LO, send_bit_1, send_bit_0
    
    CODE 
    include ../../PicLibDK/sensors/ds1820.inc


    END