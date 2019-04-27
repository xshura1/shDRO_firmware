/*
 * led.h
 *
 * Created: 08.02.2019 19:17:37
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef LED_H_
#define LED_H_

void check_led();

/**
	Процедура переводит индикатор в режим мигания.
	В режиме мигания может находиться только один индикатор.
	Если в режиме мигания был один индикатор, а стал другой, то,
	предыдущий индикатор переводится в нормальный режим.
*/
void select_led(device_led_e led);

#endif /* LED_H_ */
