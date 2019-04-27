/*
 * cmd_func.h
 *
 * Created: 02.03.2019 10:54:00
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef CMD_FUNC_H_
#define CMD_FUNC_H_


cmd_result_func_e cmd_setup(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_exit(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_help(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_add_device(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_attach_channel(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_buzzer(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_channels_info(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_del_device(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_edit_channel(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_detach_channel(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_devices_info(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_about(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_keyboard_info(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_led_blink(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_led_clr(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_led_msg(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_leds_info(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_load(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_save(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_edit_device(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_test_channels(cmd_argument_s * args, uint8_t args_num);
cmd_result_func_e cmd_led_lumi(cmd_argument_s * args, uint8_t args_num);



#endif /* CMD_FUNC_H_ */
