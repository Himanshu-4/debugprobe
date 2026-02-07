// // Patch for DAP.c - Add JTAG protocol support to CMSIS-DAP
// // Add to existing DAP.c file

// #include "probe.pio.h"

// // Add to global variables section
// static bool jtag_mode_active = false;

// // Modify DAP_Connect function
// uint8_t DAP_Connect(uint8_t port) {
//     if (port == DAP_PORT_AUTODETECT) {
//         // Try SWD first, then JTAG
//         port = DAP_PORT_SWD;
//     }
    
//     switch (port) {
//         case DAP_PORT_SWD:
//             jtag_mode_active = false;
//             // Existing SWD initialization
//             probe_set_swclk_freq(DAP_Data.clock_freq);
//             return DAP_PORT_SWD;
            
//         case DAP_PORT_JTAG:
//             jtag_mode_active = true;
//             // JTAG initialization
//             jtag_config_t jtag_cfg = {
//                 .tck_pin = PROBE_PIN_SWCLK,  // Reuse SWCLK pin for TCK
//                 .tms_pin = PROBE_PIN_TMS,     // New TMS pin
//                 .tdi_pin = PROBE_PIN_TDI,     // New TDI pin
//                 .tdo_pin = PROBE_PIN_TDO,     // New TDO pin (reuse SWDIO)
//                 .trst_pin = 0xFF,             // Optional, not used
//                 .freq_khz = DAP_Data.clock_freq / 1000,
//                 .current_state = TAP_RESET
//             };
//             jtag_init(pio0, 0, &jtag_cfg);
//             return DAP_PORT_JTAG;
            
//         default:
//             return DAP_PORT_DISABLED;
//     }
// }

// // Add new JTAG-specific DAP commands
// uint32_t DAP_JTAG_Sequence(const uint8_t *request, uint8_t *response) {
//     uint8_t sequence_count = *request++;
//     uint32_t resp_len = 1;
    
//     *response++ = DAP_OK;
    
//     for (uint8_t i = 0; i < sequence_count; i++) {
//         uint8_t sequence_info = *request++;
//         uint32_t bit_count = sequence_info & 0x3F;
//         if (bit_count == 0) bit_count = 64;
        
//         bool tms_value = (sequence_info & 0x40) != 0;
//         bool tdo_capture = (sequence_info & 0x80) != 0;
        
//         uint32_t byte_count = (bit_count + 7) / 8;
        
//         if (tms_value) {
//             // TMS sequence - navigate TAP states
//             for (uint32_t bit = 0; bit < bit_count; bit++) {
//                 uint8_t tms = (request[bit / 8] >> (bit % 8)) & 1;
//                 uint32_t tms_word = tms;
//                 pio_sm_put_blocking(pio0, 1, tms_word);  // TMS state machine
//             }
//             request += byte_count;
//         } else {
//             // TDI/TDO sequence - shift data
//             if (tdo_capture) {
//                 jtag_shift_data(request, response, bit_count, false);
//                 response += byte_count;
//                 resp_len += byte_count;
//             } else {
//                 jtag_shift_data(request, NULL, bit_count, false);
//             }
//             request += byte_count;
//         }
//     }
    
//     return resp_len;
// }

// uint32_t DAP_JTAG_Configure(const uint8_t *request, uint8_t *response) {
//     uint8_t device_count = *request++;
    
//     if (device_count == 0 || device_count > 8) {
//         *response = DAP_ERROR;
//         return 1;
//     }
    
//     // Store IR lengths for each device in chain
//     for (uint8_t i = 0; i < device_count; i++) {
//         uint8_t ir_length = *request++;
//         // Store IR length configuration
//         // This will be used for proper JTAG chain navigation
//     }
    
//     *response = DAP_OK;
//     return 1;
// }

// uint32_t DAP_JTAG_IDCode(const uint8_t *request, uint8_t *response) {
//     uint8_t index = *request;
    
//     // Read IDCODE from JTAG chain
//     jtag_goto_state(TAP_RESET);
//     jtag_goto_state(TAP_SHIFT_DR);
    
//     uint32_t idcode = 0;
    
//     // Skip to requested device in chain
//     for (uint8_t i = 0; i < index; i++) {
//         jtag_shift_data(NULL, (uint8_t*)&idcode, 32, false);
//     }
    
//     // Read target device IDCODE
//     jtag_shift_data(NULL, (uint8_t*)&idcode, 32, true);
//     jtag_goto_state(TAP_IDLE);
    
//     *response++ = DAP_OK;
//     memcpy(response, &idcode, 4);
    
//     return 5;  // 1 byte status + 4 bytes IDCODE
// }

// // Modify DAP_ProcessCommand to handle JTAG commands
// uint32_t DAP_ProcessCommand(const uint8_t *request, uint8_t *response) {
//     uint8_t cmd = *request++;
    
//     // Existing commands...
    
//     switch (cmd) {
//         // Existing SWD commands...
        
//         case ID_DAP_JTAG_Sequence:
//             return DAP_JTAG_Sequence(request, response);
            
//         case ID_DAP_JTAG_Configure:
//             return DAP_JTAG_Configure(request, response);
            
//         case ID_DAP_JTAG_IDCODE:
//             return DAP_JTAG_IDCode(request, response);
            
//         case ID_DAP_Transfer:
//             if (jtag_mode_active) {
//                 // Handle transfers via JTAG
//                 return DAP_JTAG_Transfer(request, response);
//             } else {
//                 // Existing SWD transfer handling
//                 return DAP_SWD_Transfer(request, response);
//             }
            
//         default:
//             *response = DAP_ERROR;
//             return 1;
//     }
// }

// // Add JTAG transfer implementation for memory access
// uint32_t DAP_JTAG_Transfer(const uint8_t *request, uint8_t *response) {
//     uint8_t dap_index = *request++;
//     uint8_t transfer_count = *request++;
//     uint8_t transfer_request;
    
//     uint8_t *resp_count = response++;
//     uint8_t *resp_value = response++;
    
//     *resp_count = 0;
//     *resp_value = 0;
    
//     uint32_t resp_len = 2;
    
//     for (uint8_t i = 0; i < transfer_count; i++) {
//         transfer_request = *request++;
        
//         bool apndp = transfer_request & DAP_TRANSFER_APnDP;
//         bool rnw = transfer_request & DAP_TRANSFER_RnW;
//         uint8_t reg = (transfer_request & DAP_TRANSFER_A32) >> 2;
        
//         // Write IR to select DPACC or APACC
//         uint32_t ir = apndp ? 0x0B : 0x0A;  // APACC : DPACC
//         jtag_write_ir(ir, 4);
        
//         if (rnw) {
//             // Read operation
//             uint32_t value = jtag_read_dr(35);  // 35 bits: 32 data + 3 status
//             uint8_t ack = value & 0x07;
            
//             if (ack != 0x02) {  // OK response
//                 *resp_value = ack;
//                 break;
//             }
            
//             memcpy(response, &value, 4);
//             response += 4;
//             resp_len += 4;
//         } else {
//             // Write operation
//             uint32_t data;
//             memcpy(&data, request, 4);
//             request += 4;
            
//             jtag_write_dr(data, 35);
//         }
        
//         (*resp_count)++;
//     }
    
//     return resp_len;
// }