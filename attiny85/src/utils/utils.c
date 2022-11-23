#include <avr/io.h>
#include "utils/utils.h"

void digitalWrite_B(int pin, int level) {
	if (level == HIGH) {
		asm("sbi %0, %1" :: "I" (_SFR_IO_ADDR(PORTB)), "I" (pin));
	} else if (level == LOW) {
		asm("cbi %0, %1" :: "I" (_SFR_IO_ADDR(PORTB)), "I" (pin));
	}
}

void digitalWrite_A(int pin, int level) {
	if (level == HIGH) {
		asm("sbi %0, %1" :: "I" (_SFR_IO_ADDR(PORTB)), "I" (pin));
	} else if (level == LOW) {
		asm("cbi %0, %1" :: "I" (_SFR_IO_ADDR(PORTB)), "I" (pin));
	}
}