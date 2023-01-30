#include <atmel_start.h>
#include <stdio.h>
#include <util/delay.h>
#include "set_pwm.h"
#include "heater.h"
extern volatile uint32_t tcb_irqs;
extern volatile uint16_t tcb_per, tcb_dc;
extern uint32_t uptime_ms; // From driver_isr.c

uint32_t freq_in = 0;
uint32_t dc_in = 0;
uint32_t freq_out[3] = {10000, 50000, 90000};
int dc_out[3] = {20, 60, 90};
int ret = 0;

// ***Error codes***
// 1 = Blower error
// 2 = Glowplug error
// 3 = Fuelpump error
// 4 = Normal mode run error
// 5 = Overheating warning

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	// Let's test that our TCB3 timer interrupts work. They should occur once per msec with 16MHz sysclk.
	printf("Startup! Uptime now %lu ms ", uptime_ms);
	_delay_ms(1000);
	printf("...and now it is %lu ms. Was that correct?\r\n", uptime_ms);

		ret = heater_check();
		if(ret != 0){
			LED_D13_set_level(false);
			FAULT_LED_set_level(true); //Fault led code port = PB2
			printf("Heater check exited with error code %d.\r\n", ret);	//Heater_check function returns error code with reason
			_delay_ms(2000);
		}
		else {
			printf("Heater check passed.");							//Heater_check is working properlty
			PASS_LED_set_level(true);  //Pass led code port = PB1
		}
		while(1);
	return 0;
}
