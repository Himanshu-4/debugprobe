/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef BOARD_DEBUG_PROBE_H_
#define BOARD_DEBUG_PROBE_H_

#define PROBE_IO_SWDI
#define PROBE_CDC_UART
// No reset pin 

// PIO config
#define PROBE_SM 0
#define PROBE_PIN_OFFSET 12
#define PROBE_PIN_SWCLK (PROBE_PIN_OFFSET + 0)
// For level-shifted input.
#define PROBE_PIN_SWDI (PROBE_PIN_OFFSET + 1)
#define PROBE_PIN_SWDIO (PROBE_PIN_OFFSET + 2)

// UART config
#define PROBE_UART_TX 4
#define PROBE_UART_RX 5
#define PROBE_UART_INTERFACE uart1
#define PROBE_UART_BAUDRATE 115200

#define PROBE_USB_CONNECTED_LED 2
#define PROBE_DAP_CONNECTED_LED 3
#define PROBE_DAP_RUNNING_LED 25
#define PROBE_UART_RX_LED 7
#define PROBE_UART_TX_LED 8

#define PROBE_PRODUCT_STRING "Debug Probe (CMSIS-DAP)"


// Patch for probe_config.h
// Add JTAG pin definitions
#define PROBE_PIN_RESET     5

// JTAG pin definitions (sharing some pins with SWD)
#define PROBE_PIN_TCK       PROBE_PIN_SWCLK  // TCK shares with SWCLK 
#define PROBE_PIN_TDI       PROBE_PIN_SWDI   // TDI shares with SWDI
#define PROBE_PIN_TDO       PROBE_PIN_SWDIO  // TDO shares with SWDIO 
#define PROBE_PIN_TMS       (PROBE_PIN_OFFSET + 3)  // TMS dedicated pin
#define PROBE_PIN_TRST      (PROBE_PIN_OFFSET  + 4)  // Optional TRST (can be NC)

// Pin mapping explanation:
// SWD uses: SWCLK (2), SWDIO (3), RESET (5)
// JTAG uses: TCK (2), TMS (4), TDI (6), TDO (3), TRST (7), RESET (5)
//
// Shared pins:
// - TCK shares with SWCLK (both are clock signals)
// - TDO shares with SWDIO (both are bidirectional data)
// - RESET is common to both protocols

// Protocol selection
typedef enum {
    PROBE_PROTOCOL_SWD = 0,
    PROBE_PROTOCOL_JTAG = 1
} probe_protocol_t;

// Global protocol state
extern probe_protocol_t current_protocol;

// Function prototypes
void probe_init_pins(probe_protocol_t protocol);
void probe_deinit_pins(void);

#endif
