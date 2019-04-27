/*
 * power.h
 *
 * Created: 14.10.2018 11:04:30
 *
 *  Copyright © 2019 Alexandr Khokhlov. Contacts: alexander.hohlov@gmail.com
 *  License: http://opensource.org/licenses/GPL-3.0
 */ 


#ifndef POWER_H_
#define POWER_H_

#include <avr/io.h>

// вход контроля питания
#define POWER_PIN	PC4
#define POWER_VALUE	((PINC & (1 << POWER_PIN)) == 0)
#define POWER_DDR	DDRC


void check_power();



#endif /* POWER_H_ */
