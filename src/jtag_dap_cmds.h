#ifndef _DAP_CONFIG_H
#define _DAP_CONFIG_H

// Implementation in probe_config.c (create new file)

#include "probe_config.h"
#include "pico/stdlib.h"



#include <stdio.h>
#include <stdint.h>
// Existing DAP command IDs
#define ID_DAP_Info                     0x00
#define ID_DAP_HostStatus              0x01
#define ID_DAP_Connect                 0x02
#define ID_DAP_Disconnect              0x03
#define ID_DAP_TransferConfigure       0x04
#define ID_DAP_Transfer                0x05
#define ID_DAP_TransferBlock           0x06
#define ID_DAP_TransferAbort           0x07
#define ID_DAP_WriteABORT              0x08
#define ID_DAP_Delay                   0x09
#define ID_DAP_ResetTarget             0x0A
#define ID_DAP_SWJ_Pins                0x10
#define ID_DAP_SWJ_Clock               0x11
#define ID_DAP_SWJ_Sequence            0x12

// Add JTAG-specific command IDs
#define ID_DAP_JTAG_Sequence           0x14
#define ID_DAP_JTAG_Configure          0x15
#define ID_DAP_JTAG_IDCODE             0x16

#define ID_DAP_SWO_Transport           0x17
#define ID_DAP_SWO_Mode                0x18
#define ID_DAP_SWO_Baudrate            0x19
#define ID_DAP_SWO_Control             0x1A
#define ID_DAP_SWO_Status              0x1B
#define ID_DAP_SWO_ExtendedStatus      0x1E
#define ID_DAP_SWO_Data                0x1C

// DAP transfer request bits
#define DAP_TRANSFER_APnDP             (1U << 0)
#define DAP_TRANSFER_RnW               (1U << 1)
#define DAP_TRANSFER_A2                (1U << 2)
#define DAP_TRANSFER_A3                (1U << 3)
#define DAP_TRANSFER_MATCH_VALUE       (1U << 4)
#define DAP_TRANSFER_MATCH_MASK        (1U << 5)
#define DAP_TRANSFER_TIMESTAMP         (1U << 7)

#define DAP_TRANSFER_A32               (DAP_TRANSFER_A2 | DAP_TRANSFER_A3)

// DAP transfer response values
#define DAP_TRANSFER_OK                (1U << 0)
#define DAP_TRANSFER_WAIT              (1U << 1)
#define DAP_TRANSFER_FAULT             (1U << 2)
#define DAP_TRANSFER_ERROR             (1U << 3)
#define DAP_TRANSFER_MISMATCH          (1U << 4)

// DAP Port values
#define DAP_PORT_AUTODETECT            0
#define DAP_PORT_DISABLED              0
#define DAP_PORT_SWD                   1
#define DAP_PORT_JTAG                  2

// DAP Status codes
#define DAP_OK                         0x00
#define DAP_ERROR                      0xFF

// JTAG-specific configuration
#define DAP_JTAG_DEV_CNT               8     // Max devices in JTAG chain
#define DAP_JTAG_IR_LENGTH             4     // Default IR length

// JTAG IR register values for ARM CoreSight
#define JTAG_IR_ABORT                  0x08
#define JTAG_IR_DPACC                  0x0A
#define JTAG_IR_APACC                  0x0B
#define JTAG_IR_IDCODE                 0x0E
#define JTAG_IR_BYPASS                 0x0F

// DAP Data structure extension for JTAG
typedef struct {
    uint32_t clock_freq;
    uint8_t  transfer_retry;
    uint8_t  transfer_idle;
    uint16_t transfer_match;
    
    // JTAG-specific fields
    uint8_t  jtag_dev_count;
    uint8_t  jtag_ir_length[DAP_JTAG_DEV_CNT];
    uint8_t  jtag_ir_before[DAP_JTAG_DEV_CNT];
    uint8_t  jtag_ir_after[DAP_JTAG_DEV_CNT];
    uint32_t jtag_idcode[DAP_JTAG_DEV_CNT];
} DAP_Data_t;

extern DAP_Data_t DAP_Data;

// JTAG helper macros
#define JTAG_SEQUENCE_TMS              (1U << 6)
#define JTAG_SEQUENCE_TDO              (1U << 7)

#endif // _DAP_CONFIG_H