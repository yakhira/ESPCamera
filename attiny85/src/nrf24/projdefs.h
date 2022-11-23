/*
The BSD 3-Clause License

Copyright (c) 2022, Dalimir Orfanus


Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


//------------------------------------------------------------------------
// projdefs.h - Project configuration file. 
// Contains configuration of features and pins for particular driver(s) and devices.
//------------------------------------------------------------------------

#pragma once

#include <avr/io.h> 

//------------------------------------------------------------------------
// Feature configuration for nRF24L01 driver
//------------------------------------------------------------------------

// Enable support for writing data to NRF registers directly from PROGMEM. It saves footprint.
#define NR24_READ_PROGMEM
#define NRF24L01_SHARED_CE_CSN

//------------------------------------------------------------------------
// Pins configuration for nRF24L01 driver
//------------------------------------------------------------------------
#define NRF24L01_PORT		PORTB
#define NRF24L01_DDR		DDRB
#define NRF24L01_INPORT		PINB

#define NRF24L01_CE		    PB4 // Mandatory
#define NRF24L01_CSN		PB3 // Mandatory
#define NRF24L01_SCK		PB2 // Mandatory
#define NRF24L01_MISO		PB1 // Mandatory
#define NRF24L01_MOSI		PB0 // Mandatory