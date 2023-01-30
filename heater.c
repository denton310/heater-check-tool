#include "heater.h"
#include <atmel_start.h>
#include <stdio.h>
#include <util/delay.h>
extern uint32_t uptime_ms; // From driver_isr.c

int temp = 0;
int t = 0;
float coef = 0.1564;
int offset = 40;
int i = 0;
int i1_min = 300;	//Blower_min
int i2_min = 200;	//Glow_min
int i3_min = 500;	//Glow+Fuelpump_min
int i4_min = 800;	//Blower+Fuelpump

enum state myState = {WAIT};

int heater_check() {				//Beef of code
	while (myState < TEST_OK) {
		
	i = ADC_0_get_conversion(0);	//Polling if i is increasing
	t = ADC_0_get_conversion(3);
	
	temp = (t*coef-offset);				//temp get value of temperature
	if (temp > 100){
		printf("Overheat warning!\r\n");	// if temperature is too high, it will return error code
		return 5;
	}
	
		switch (myState) {
			case WAIT:							//That is normal mode and tool is waiting to start diagnostic
					LED_D13_toggle_level();
					printf("adc: %d\r\nTemp: %d C\r\n", i, temp);
					_delay_ms(1000);
					if (i > i1_min) {
						myState++;
					}
					break;
					
			case BLOW_TEST:
				uptime_ms = 2500;
				LED_D13_set_level(true);			//BLOW_TEST is ok
				printf("Blow test is ok.\r\nTemp: %d C\r\nadc: %d\r\n", temp, i);
				_delay_ms(500);
				LED_D13_set_level(false);
				myState++;
				break;

			case GLOW_TEST:
			if((uptime_ms > 30000 && uptime_ms < 35000) && ((i > i2_min) && (i < i1_min))){
				LED_D13_set_level(true);			// GLOW_TEST is ok
				printf("Glow test is ok.\r\nTemp: %d C\r\nadc: %d\r\n", temp, i);
				_delay_ms(500);
				LED_D13_set_level(false);
				myState++;
			}
			else if(uptime_ms > 35000){			// If uptime get more than that value return error code 4
				return 2;					//Vois tarkistaa jotain häröilytuloksia i1 ja i2
			}
			break;
	
		case DOS_TEST:
			if((uptime_ms > 46000 && uptime_ms < 48000) && i > i3_min){
				LED_D13_set_level(true);			// DOS_TEST is ok
				printf("Fuel pump test is ok.\r\nTemp: %d C\r\nadc: %d\r\n", temp, i);
				_delay_ms(500);
				LED_D13_set_level(false);
				myState++;
			}
			else if (uptime_ms > 48000){	
				return 3;
			}
			break;

		case BD_TEST:
			if((uptime_ms > 135000 && uptime_ms < 140000) && i > i4_min){
				LED_D13_set_level(true);		// BLOW+DOS_TEST is ok
				printf("Blow+fuel pump test is ok.\r\nTemp: %d C\r\nadc: %d\r\n", temp, i);
				_delay_ms(500);
				LED_D13_set_level(false);
				myState++;
			}
			else if(uptime_ms > 140000){
				return 4;
			}
			break;
		default:
			myState = TEST_OK;
		}
	}
	return 0;
}