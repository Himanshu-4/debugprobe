// // jtag.c - JTAG protocol implementation
// // Create new file: src/jtag.c

// #include <stdint.h>
// #include <stdbool.h>
// #include <string.h>
// #include "pico/stdlib.h"
// #include "hardware/pio.h"
// #include "hardware/clocks.h"
// #include "jtag.pio.h"
// #include "probe.pio.h"

// typedef  unsigned int uint;

// static PIO jtag_pio = pio0;
// static uint jtag_shift_sm;
// static uint jtag_tms_sm;
// static jtag_config_t jtag_cfg;

// // TMS sequence lookup table for state transitions
// static const uint8_t tms_transitions[16][16] = {
//     // Table maps current_state -> target_state to TMS bit sequence
//     // Implementation of TAP state machine transitions
//     // Each entry contains bit pattern and length
// };

// void jtag_init(PIO pio, uint sm, const jtag_config_t *config) {
//     jtag_pio = pio;
//     jtag_shift_sm = sm;
//     jtag_tms_sm = sm + 1;
    
//     memcpy(&jtag_cfg, config, sizeof(jtag_config_t));
    
//     // Load PIO programs
//     uint shift_offset = pio_add_program(pio, &jtag_shift_program);
//     uint tms_offset = pio_add_program(pio, &jtag_tms_program);
    
//     // Initialize state machines
//     jtag_shift_program_init(pio, jtag_shift_sm, shift_offset,
//                              config->tdi_pin, config->tdo_pin, config->tck_pin);
    
//     jtag_tms_program_init(pio, jtag_tms_sm, tms_offset,
//                            config->tms_pin, config->tck_pin);
    
//     // Initialize TRST if present (active low reset)
//     if (config->trst_pin != 0xFF) {
//         gpio_init(config->trst_pin);
//         gpio_set_dir(config->trst_pin, GPIO_OUT);
//         gpio_put(config->trst_pin, 1);  // Inactive
//     }
    
//     jtag_set_frequency(config->freq_khz);
//     jtag_reset();
// }

// void jtag_set_frequency(uint32_t freq_khz) {
//     float clk_div = (float)clock_get_hz(clk_sys) / (freq_khz * 1000 * 2);
//     pio_sm_set_clkdiv(jtag_pio, jtag_shift_sm, clk_div);
//     pio_sm_set_clkdiv(jtag_pio, jtag_tms_sm, clk_div);
//     jtag_cfg.freq_khz = freq_khz;
// }

// void jtag_reset(void) {
//     // Send 5+ TMS=1 to reach Test-Logic-Reset state
//     uint32_t tms_pattern = 0xFFFFFFFF;
//     pio_sm_put_blocking(jtag_pio, jtag_tms_sm, tms_pattern);
    
//     // Wait for completion
//     pio_sm_drain_tx_fifo(jtag_pio, jtag_tms_sm);
    
//     jtag_cfg.current_state = TAP_RESET;
    
//     // Move to Run-Test/Idle
//     tms_pattern = 0x00000000;
//     pio_sm_put_blocking(jtag_pio, jtag_tms_sm, tms_pattern);
//     jtag_cfg.current_state = TAP_IDLE;
// }

// void jtag_goto_state(jtag_tap_state_t target_state) {
//     if (jtag_cfg.current_state == target_state) {
//         return;
//     }
    
//     // Implement shortest path through TAP state machine
//     uint8_t tms_sequence = 0;
//     uint8_t tms_count = 0;
    
//     jtag_tap_state_t state = jtag_cfg.current_state;
    
//     while (state != target_state && tms_count < 8) {
//         bool tms = false;
        
//         switch (state) {
//             case TAP_RESET:
//                 tms = (target_state != TAP_IDLE);
//                 state = tms ? TAP_RESET : TAP_IDLE;
//                 break;
//             case TAP_IDLE:
//                 tms = true;
//                 state = TAP_SELECT_DR;
//                 break;
//             case TAP_SELECT_DR:
//                 if (target_state >= TAP_SELECT_IR) {
//                     tms = true;
//                     state = TAP_SELECT_IR;
//                 } else {
//                     tms = false;
//                     state = TAP_CAPTURE_DR;
//                 }
//                 break;
//             case TAP_CAPTURE_DR:
//                 tms = (target_state == TAP_EXIT1_DR);
//                 state = tms ? TAP_EXIT1_DR : TAP_SHIFT_DR;
//                 break;
//             case TAP_SHIFT_DR:
//                 tms = true;
//                 state = TAP_EXIT1_DR;
//                 break;
//             case TAP_EXIT1_DR:
//                 tms = (target_state != TAP_PAUSE_DR);
//                 state = tms ? TAP_UPDATE_DR : TAP_PAUSE_DR;
//                 break;
//             case TAP_PAUSE_DR:
//                 tms = true;
//                 state = TAP_EXIT2_DR;
//                 break;
//             case TAP_EXIT2_DR:
//                 tms = (target_state != TAP_SHIFT_DR);
//                 state = tms ? TAP_UPDATE_DR : TAP_SHIFT_DR;
//                 break;
//             case TAP_UPDATE_DR:
//                 tms = (target_state != TAP_IDLE);
//                 state = tms ? TAP_SELECT_DR : TAP_IDLE;
//                 break;
//             case TAP_SELECT_IR:
//                 tms = (target_state == TAP_RESET);
//                 state = tms ? TAP_RESET : TAP_CAPTURE_IR;
//                 break;
//             case TAP_CAPTURE_IR:
//                 tms = (target_state == TAP_EXIT1_IR);
//                 state = tms ? TAP_EXIT1_IR : TAP_SHIFT_IR;
//                 break;
//             case TAP_SHIFT_IR:
//                 tms = true;
//                 state = TAP_EXIT1_IR;
//                 break;
//             case TAP_EXIT1_IR:
//                 tms = (target_state != TAP_PAUSE_IR);
//                 state = tms ? TAP_UPDATE_IR : TAP_PAUSE_IR;
//                 break;
//             case TAP_PAUSE_IR:
//                 tms = true;
//                 state = TAP_EXIT2_IR;
//                 break;
//             case TAP_EXIT2_IR:
//                 tms = (target_state != TAP_SHIFT_IR);
//                 state = tms ? TAP_UPDATE_IR : TAP_SHIFT_IR;
//                 break;
//             case TAP_UPDATE_IR:
//                 tms = (target_state != TAP_IDLE);
//                 state = tms ? TAP_SELECT_DR : TAP_IDLE;
//                 break;
//         }
        
//         if (tms) {
//             tms_sequence |= (1 << tms_count);
//         }
//         tms_count++;
//     }
    
//     // Send TMS sequence
//     if (tms_count > 0) {
//         uint32_t tms_word = tms_sequence;
//         pio_sm_put_blocking(jtag_pio, jtag_tms_sm, tms_word);
//     }
    
//     jtag_cfg.current_state = target_state;
// }

// uint32_t jtag_shift_data(const uint8_t *tdi_data, uint8_t *tdo_data,
//                           uint32_t bit_count, bool exit_shift) {
//     uint32_t bytes = (bit_count + 7) / 8;
//     uint32_t words = (bit_count + 31) / 32;
    
//     // Clear RX FIFO
//     pio_sm_clear_fifos(jtag_pio, jtag_shift_sm);
    
//     // Shift data
//     for (uint32_t i = 0; i < words; i++) {
//         uint32_t tdi_word = 0;
//         if (tdi_data) {
//             for (int j = 0; j < 4 && (i * 4 + j) < bytes; j++) {
//                 tdi_word |= ((uint32_t)tdi_data[i * 4 + j]) << (j * 8);
//             }
//         }
        
//         pio_sm_put_blocking(jtag_pio, jtag_shift_sm, tdi_word);
        
//         if (tdo_data) {
//             uint32_t tdo_word = pio_sm_get_blocking(jtag_pio, jtag_shift_sm);
//             for (int j = 0; j < 4 && (i * 4 + j) < bytes; j++) {
//                 tdo_data[i * 4 + j] = (tdo_word >> (j * 8)) & 0xFF;
//             }
//         }
//     }
    
//     if (exit_shift) {
//         // Exit on last bit via TMS
//         uint32_t tms_pattern = 0x00000001;
//         pio_sm_put_blocking(jtag_pio, jtag_tms_sm, tms_pattern);
//     }
    
//     return bit_count;
// }

// void jtag_write_ir(uint32_t ir_value, uint32_t ir_len) {
//     jtag_goto_state(TAP_SHIFT_IR);
//     uint8_t ir_bytes[4];
//     memcpy(ir_bytes, &ir_value, sizeof(ir_bytes));
//     jtag_shift_data(ir_bytes, NULL, ir_len, true);
//     jtag_goto_state(TAP_IDLE);
// }

// uint32_t jtag_read_dr(uint32_t dr_len) {
//     jtag_goto_state(TAP_SHIFT_DR);
//     uint32_t result = 0;
//     jtag_shift_data(NULL, (uint8_t*)&result, dr_len, true);
//     jtag_goto_state(TAP_IDLE);
//     return result;
// }

// void jtag_write_dr(uint32_t dr_value, uint32_t dr_len) {
//     jtag_goto_state(TAP_SHIFT_DR);
//     uint8_t dr_bytes[4];
//     memcpy(dr_bytes, &dr_value, sizeof(dr_bytes));
//     jtag_shift_data(dr_bytes, NULL, dr_len, true);
//     jtag_goto_state(TAP_IDLE);
// }

// void jtag_pulse_tck(uint32_t count) {
//     // Stay in current state, pulse TCK
//     uint32_t tms_pattern = 0x00000000;
//     for (uint32_t i = 0; i < (count + 31) / 32; i++) {
//         pio_sm_put_blocking(jtag_pio, jtag_tms_sm, tms_pattern);
//     }
// }