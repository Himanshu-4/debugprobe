// probe.pio.h - Add JTAG state machine definitions
// Patch: Add after existing SWD definitions

#ifndef _PROBE_PIO_JTAG_H
#define _PROBE_PIO_JTAG_H

#include <stdint.h>
#include <hardware/pio.h>

// JTAG TAP states
typedef enum {
    TAP_RESET = 0,
    TAP_IDLE,
    TAP_SELECT_DR,
    TAP_CAPTURE_DR,
    TAP_SHIFT_DR,
    TAP_EXIT1_DR,
    TAP_PAUSE_DR,
    TAP_EXIT2_DR,
    TAP_UPDATE_DR,
    TAP_SELECT_IR,
    TAP_CAPTURE_IR,
    TAP_SHIFT_IR,
    TAP_EXIT1_IR,
    TAP_PAUSE_IR,
    TAP_EXIT2_IR,
    TAP_UPDATE_IR
} jtag_tap_state_t;

// JTAG configuration structure
typedef struct {
    uint tck_pin;
    uint tms_pin;
    uint tdi_pin;
    uint tdo_pin;
    uint trst_pin;  // Optional
    uint freq_khz;
    jtag_tap_state_t current_state;
} jtag_config_t;

// Function prototypes
void jtag_init(PIO pio, uint sm, const jtag_config_t *config);
void jtag_set_frequency(uint32_t freq_khz);
void jtag_reset(void);
void jtag_goto_state(jtag_tap_state_t target_state);
uint32_t jtag_shift_data(const uint8_t *tdi_data, uint8_t *tdo_data, 
                          uint32_t bit_count, bool exit_shift);
void jtag_pulse_tck(uint32_t count);
void jtag_write_ir(uint32_t ir_value, uint32_t ir_len);
uint32_t jtag_read_dr(uint32_t dr_len);
void jtag_write_dr(uint32_t dr_value, uint32_t dr_len);

#endif // _PROBE_PIO_JTAG_H