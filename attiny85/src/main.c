#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <Arduino.h>

#include "nrf24/projdefs.h"
#include "nrf24/nRF24L01.h"
#include "utils/utils.h"
#include "tinysnore/tinysnore.h"


#define CHANNEL 76 // 0-125
#define FIVE_BYTES	5
#define SLEEP_PERIOD_MILLIS 1000

const uint8_t PIPE0_ADDRESS_PGM[] PROGMEM = "3Node"; // pipe 0 address in progmem - don't forget to define macro NR24_READ_PROGMEM in projdefs.h to support it

void receive() {
	bool sleep = true;
	uint8_t payload[NRF24_MAX_SIZE];

	nrf24_writeReg(W_REGISTER | EN_AA,      NRF24_PIPE_0);  // en autoack
	nrf24_writeReg(W_REGISTER | NRF_STATUS, NRF24_STATUS_CLEAR_ALL);  // clear status
	
	// set RX mode, enable CRC with 2 bytes, mask all IRQs, power on nRF radio (POWER DOWN ==> STANDBY-1)

	while(1) {
		nrf24_writeReg(W_REGISTER | NRF_CONFIG,
			NRF24_CFG_PWR_UP | NRF24_CFG_RX_MODE | NRF24_CFG_CRC_2B | NRF24_CFG_CRC_EN | NRF24_CFG_IRQ_MASK_ALL);

		_delay_ms(DELAY_POWER_UP_MILLIS);

		uint8_t status = nrf24_readReg(R_REGISTER | NRF_STATUS);

		if( status & NRF24_STATUS_RX_DR ) {
			// read everything what has been received from RX FIFO
			// (make sure you read everything from RX FIFO otherwise RX FIFO will overflow eventually)
			// (if you are lazy, you can always do FLUSH_RX after each read but you loose unprocessed frames, RX has 3 FIFOs
			uint8_t bytes = nrf24_readReg(R_RX_PL_WID);

			nrf24_readRegs(R_RX_PAYLOAD, payload, bytes);	// read payload into a buffer
			nrf24_writeReg(W_REGISTER | NRF_STATUS, NRF24_STATUS_CLEAR_ALL); // clear received flag

			payload[bytes] = 0;

			if (strcmp(payload, "UP") == 0) {
				digitalWrite_B(PB4, HIGH);
				sleep = false;
			}  else if (strcmp(payload, "DOWN") == 0) {
				digitalWrite_B(PB4, LOW);
				sleep = true;
			}
		}

		nrf24_writeReg(W_REGISTER | NRF_CONFIG,
			NRF24_CFG_PWR_DOWN | NRF24_CFG_RX_MODE | NRF24_CFG_CRC_2B | NRF24_CFG_CRC_EN | NRF24_CFG_IRQ_MASK_ALL);

		if (sleep) {
			snore(SLEEP_PERIOD_MILLIS);
		}
	}
}

void setup() {
	DDRB |= _BV(PB4);

	nrf24_init(); // initialize radio (UNDEFINED ==> POWER ON RESET) 
	
	_delay_ms(20); // it takes 10.3ms to switch from POWER ON RESET ==> POWER DOWN
	
	nrf24_writeReg(W_REGISTER | RF_CH,    CHANNEL);
	nrf24_writeReg(W_REGISTER | RF_SETUP, NRF24_PWR_MAX | NRF24_SPEED_250kbps); // 0dbm TX power, 250kbps
	
	nrf24_writeReg(W_REGISTER | EN_RXADDR, NRF24_PIPE_0);	// enable RX in pipe 0 for ACK packet
	nrf24_writeReg(W_REGISTER | DYNPD,     NRF24_PIPE_0);   // enable dynamic payload in pipe 0	x`
	nrf24_writeReg(W_REGISTER | FEATURE,   NRF24_FEATURE_EN_DPL); // enable dynamic payload length
	
	// Target pipe 0 address from PROGMEM. Because we read value from PROGMEM, we have to add flag NRF24_PROGMEM_MASK to the size.
	nrf24_writeRegs(W_REGISTER | TX_ADDR, PIPE0_ADDRESS_PGM, FIVE_BYTES | NRF24_PROGMEM_MASK); 	
	// RX address on pipe 0 from PROGMEM. Because we read value from PROGMEM, we have to add flag NRF24_PROGMEM_MASK to the size.
	nrf24_writeRegs(W_REGISTER | RX_ADDR_P0, PIPE0_ADDRESS_PGM, FIVE_BYTES | NRF24_PROGMEM_MASK); 

	nrf24_cmd(FLUSH_TX); // clean TX FIFOs thoroughly
	nrf24_cmd(FLUSH_RX); // clean RX FIFOs thoroughly
}

void loop(){
	receive();
}
