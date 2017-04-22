
//
//    Copyright (C) 2007-2008 Sebastian Kuzminsky
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

// this is just the hostmot2 #defines from linuxcnc's hostmot2.h

#ifndef __HOSTMOT2_CONSTANTS_H
#define __HOSTMOT2_CONSTANTS_H


// 
// idrom addresses & constants
// 

#define HM2_ADDR_IOCOOKIE  (0x0100)
#define HM2_IOCOOKIE       (0x55AACAFE)

#define HM2_ADDR_CONFIGNAME    (0x0104)
#define HM2_CONFIGNAME         "HOSTMOT2"
#define HM2_CONFIGNAME_LENGTH  (8)

#define HM2_ADDR_IDROM_OFFSET (0x010C)

#define HM2_MAX_MODULE_DESCRIPTORS  (48)
#define HM2_MAX_PIN_DESCRIPTORS     (1000)

// 
// Pin Descriptor constants
// 

#define HM2_PIN_SOURCE_IS_PRIMARY   (0x00000000)
#define HM2_PIN_SOURCE_IS_SECONDARY (0x00000001)

#define HM2_PIN_DIR_IS_INPUT     (0x00000002)
#define HM2_PIN_DIR_IS_OUTPUT    (0x00000004)


// 
// Module Descriptor constants
// 

#define HM2_GTAG_WATCHDOG          (2)
#define HM2_GTAG_IOPORT            (3)
#define HM2_GTAG_ENCODER           (4)
#define HM2_GTAG_STEPGEN           (5)
#define HM2_GTAG_PWMGEN            (6)
#define HM2_GTAG_SPI               (7) // Not supported
#define HM2_GTAG_SSI               (8)
#define HM2_GTAG_UART_TX           (9)
#define HM2_GTAG_UART_RX           (10)
#define HM2_GTAG_PKTUART_TX        (27)  // PktUART uses same addresses as normal UART with 
#define HM2_GTAG_PKTUART_RX        (28) // the assumption you would not use both in one config
#define HM2_GTAG_TRANSLATIONRAM    (11)
#define HM2_GTAG_MUXED_ENCODER     (12)
#define HM2_GTAG_MUXED_ENCODER_SEL (13)
#define HM2_GTAG_BSPI              (14)
#define HM2_GTAG_DBSPI             (15) // Not supported
#define HM2_GTAG_DPLL              (16) // Not supported
#define HM2_GTAG_MUXED_ENCODER_M   (17) // Not supported
#define HM2_GTAG_MUXED_ENC_SEL_M   (18) // Not supported
#define HM2_GTAG_TPPWM             (19)
#define HM2_GTAG_WAVEGEN           (20) // Not supported
#define HM2_GTAG_DAQFIFO           (21) // Not supported
#define HM2_GTAG_BINOSC            (22) // Not supported
#define HM2_GTAG_DDMA              (23) // Not supported
#define HM2_GTAG_BISS              (24) 
#define HM2_GTAG_FABS              (25) 
#define HM2_GTAG_HM2DPLL           (26) 
#define HM2_GTAG_LIOPORT           (64) // Not supported
#define HM2_GTAG_LED               (128)

#define HM2_GTAG_RESOLVER          (192)
#define HM2_GTAG_SMARTSERIAL       (193)
#define HM2_GTAG_TWIDDLER          (194) // Not supported



//
// IDROM and MD structs
//


typedef struct {
    uint32_t idrom_type;
    uint32_t offset_to_modules;
    uint32_t offset_to_pin_desc;
    uint8_t board_name[8];  // ascii string, but not NULL terminated!
    uint32_t fpga_size;
    uint32_t fpga_pins;
    uint32_t io_ports;
    uint32_t io_width;
    uint32_t port_width;
    uint32_t clock_low;
    uint32_t clock_high;
    uint32_t instance_stride_0;
    uint32_t instance_stride_1;
    uint32_t register_stride_0;
    uint32_t register_stride_1;
} hm2_idrom_t;


typedef struct {
    uint8_t gtag;
    uint8_t version;
    uint8_t clock_tag;
    uint32_t clock_freq;  // this one's not in the MD struct in the device, it's set from clock_tag and the idrom header for our convenience
    uint8_t instances;
    uint16_t base_address;

    uint8_t num_registers;
    uint32_t register_stride;
    uint32_t instance_stride;
    uint32_t multiple_registers;
} hm2_module_descriptor_t;




//
// encoders
//

#define HM2_ENCODER_QUADRATURE_ERROR    (1<<15)
#define HM2_ENCODER_AB_MASK_POLARITY    (1<<14)
#define HM2_ENCODER_LATCH_ON_PROBE      (1<<13)
#define HM2_ENCODER_PROBE_POLARITY      (1<<12)
#define HM2_ENCODER_FILTER              (1<<11)
#define HM2_ENCODER_COUNTER_MODE        (1<<10)
#define HM2_ENCODER_INDEX_MASK          (1<<9)
#define HM2_ENCODER_INDEX_MASK_POLARITY (1<<8)
#define HM2_ENCODER_INDEX_JUSTONCE      (1<<6)
#define HM2_ENCODER_CLEAR_ON_INDEX      (1<<5)
#define HM2_ENCODER_LATCH_ON_INDEX      (1<<4)
#define HM2_ENCODER_INDEX_POLARITY      (1<<3)
#define HM2_ENCODER_INPUT_INDEX         (1<<2)
#define HM2_ENCODER_INPUT_B             (1<<1)
#define HM2_ENCODER_INPUT_A             (1<<0)

#define HM2_ENCODER_CONTROL_MASK  (0x0000ffff)



#define HM2_PWMGEN_OUTPUT_TYPE_PWM          1  // this is the same value that the software pwmgen component uses
#define HM2_PWMGEN_OUTPUT_TYPE_UP_DOWN      2  // this is the same value that the software pwmgen component uses
#define HM2_PWMGEN_OUTPUT_TYPE_PDM          3  // software pwmgen does not support pdm as an output type
#define HM2_PWMGEN_OUTPUT_TYPE_PWM_SWAPPED  4  // software pwmgen does not support pwm/swapped output type because it doesnt need to 

#endif
