//
//    Copyright (C) 2017 Sebastian Kuzminsky
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

//
// This is Hostmot2 firmware for the Beaglebone PRUSS.  It implements
// the IDROMv2 flavor of Hostmot2.  The register file is in the PRUSS
// "Shared RAM 2" region, which is accessible to both the PRUs and to the
// OS running on the ARM.  The ARM should be running the LinuxCNC hm2_bb
// driver, which interfaces the PRU0 and PRU1 Hostmot2 instances to HAL.
//


#include <string.h>

#include <pru/io.h>

#include "resource_table_0.h"

#include "hostmot2-constants.h"


#define SHRDRAM2 0x10000

#define SHRDRAM2_OFFS (0x10000)
#define SHRDRAM2_SIZE (3*1024)

// PRU0 has 11 pins brought out to P8 and P9
// PRU1 has 15 pins
#define NUM_PINS 11


#define NUM_STEPGENS 4

#define PD_ADDR 0x110
#define MD_ADDR 0x200


typedef struct {
    uint32_t period;  // MSB means move the other direction
    uint32_t count;
    uint32_t old_cycles;  // last time we incremented the internal counter
    uint32_t dir;
    uint32_t dir_bit;
    uint32_t step;
    uint32_t step_bit;
} pru_stepgen_t;


inline void idrom_write4(uint16_t addr, uint32_t val) {
    *(uint32_t*)((uint8_t*)SHRDRAM2_OFFS + addr) = val;
}

inline uint32_t idrom_read4(uint16_t addr) {
    return *(uint32_t*)((uint8_t*)SHRDRAM2_OFFS + addr);
}

inline void idrom_write2(uint16_t addr, uint16_t val) {
    *(uint16_t*)(SHRDRAM2_OFFS + addr) = val;
}

inline uint16_t idrom_read2(uint16_t addr) {
    return *(uint16_t*)(SHRDRAM2_OFFS + addr);
}

inline void idrom_write1(uint16_t addr, uint8_t val) {
    *(uint8_t*)(SHRDRAM2_OFFS + addr) = val;
}

inline uint8_t idrom_read1(uint16_t addr) {
    return *(uint8_t*)(SHRDRAM2_OFFS + addr);
}


void idrom_write_pin_descriptor(
    uint16_t pd_base_addr,
    int pin_index,
    uint8_t primary_tag,
    uint8_t secondary_tag,
    uint8_t secondary_unit,
    uint8_t secondary_pin
) {
    idrom_write1(pd_base_addr + (4 * pin_index) + 0, secondary_pin);
    idrom_write1(pd_base_addr + (4 * pin_index) + 1, secondary_tag);
    idrom_write1(pd_base_addr + (4 * pin_index) + 2, secondary_unit);
    idrom_write1(pd_base_addr + (4 * pin_index) + 3, primary_tag);
}


void idrom_write_module_descriptor_gpio(uint32_t addr) {
    idrom_write1(addr + 0, HM2_GTAG_IOPORT);  // GTAG
    idrom_write1(addr + 1, 0);                // version
    idrom_write1(addr + 2, 2);                // clocktag, 1=low, 2=high
    idrom_write1(addr + 3, 1);                // number of instances
    idrom_write2(addr + 4, 0x0300);           // base address
    idrom_write1(addr + 6, 5);                // number of registers
    idrom_write1(addr + 7, 0);                // instance stride 0 (0x0004)
    idrom_write4(addr + 8, 0x1f);             // bitmap of which registers are multiple (1=multiple, LSb=r0)
}


void hm2_idrom_setup(void) {
    memset((void*)SHRDRAM2_OFFS, 0x00, SHRDRAM2_SIZE);

    idrom_write4(HM2_ADDR_IOCOOKIE, 0x55aacafe);

    idrom_write1(HM2_ADDR_CONFIGNAME+0, (uint8_t)'H');
    idrom_write1(HM2_ADDR_CONFIGNAME+1, (uint8_t)'O');
    idrom_write1(HM2_ADDR_CONFIGNAME+2, (uint8_t)'S');
    idrom_write1(HM2_ADDR_CONFIGNAME+3, (uint8_t)'T');
    idrom_write1(HM2_ADDR_CONFIGNAME+4, (uint8_t)'M');
    idrom_write1(HM2_ADDR_CONFIGNAME+5, (uint8_t)'O');
    idrom_write1(HM2_ADDR_CONFIGNAME+6, (uint8_t)'T');
    idrom_write1(HM2_ADDR_CONFIGNAME+7, (uint8_t)'2');

    // Put the IDROM structure at the very beginning, to conserve
    // address space.
    idrom_write4(HM2_ADDR_IDROM_OFFSET, 0);

    // Here's the IDROM:
    // typedef struct {
    //     rtapi_u32 idrom_type;
    //     rtapi_u32 offset_to_modules;
    //     rtapi_u32 offset_to_pin_desc;
    //     rtapi_u8 board_name[8];  // ascii string, but not NULL terminated!
    //     rtapi_u32 fpga_size;
    //     rtapi_u32 fpga_pins;
    //     rtapi_u32 io_ports;
    //     rtapi_u32 io_width;
    //     rtapi_u32 port_width;
    //     rtapi_u32 clock_low;
    //     rtapi_u32 clock_high;
    //     rtapi_u32 instance_stride_0;
    //     rtapi_u32 instance_stride_1;
    //     rtapi_u32 register_stride_0;
    //     rtapi_u32 register_stride_1;
    // } hm2_idrom_t;

    idrom_write4( 0, 0x2);    // idrom type
    idrom_write4( 4, MD_ADDR);  // offset from beginning of idrom to beginning of module descriptors
    idrom_write4( 8, PD_ADDR);  // offset from beginning of idrom to beginning of pin descriptors

    // board name: "BB-PRUx"
    idrom_write1(12, 'B');
    idrom_write1(13, 'B');
    idrom_write1(14, '-');
    idrom_write1(15, 'P');
    idrom_write1(16, 'R');
    idrom_write1(17, 'U');
    idrom_write1(18, '0');  // '0' or '1'
    idrom_write1(19, '\0');

    idrom_write4(20, 0);         // fpga size - not applicable
    idrom_write4(24, 0);         // number of pins on fpga - not applicable
    idrom_write4(28, 1);         // number of io ports
    idrom_write4(32, NUM_PINS);  // number of io pins total
    idrom_write4(36, NUM_PINS);  // number of io pins per port

    idrom_write4(40, 1000*1000);      // frequency of the slow clock
    idrom_write4(44, 200*1000*1000);  // frequency of the fast clock

    idrom_write4(48, 0x0004);  // "instance stride 0"
    idrom_write4(52, 0x0040);  // "instance stride 1"

    idrom_write4(56, 0x0100);  // "register stride 0"
    idrom_write4(60, 0x0004);  // "register stride 1"

    // Add Pin Descriptors
    for (int i = 0; i < NUM_PINS; i ++) {
        idrom_write_pin_descriptor(PD_ADDR, i, HM2_GTAG_IOPORT, 0, 0, 0);
    }

    // Add Module Descriptors
    idrom_write_module_descriptor_gpio(MD_ADDR);
}


static void delay_us(unsigned int us) {
	/* assume cpu frequency is 200MHz */
	__delay_cycles (us * (1000 / 5));
}


int main(void) {
    uint32_t *shrdram2 = (uint32_t*)SHRDRAM2;

    uint32_t cycles = 0;

    uint32_t stepgen_start = 0x1000;
    pru_stepgen_t stepgen[NUM_STEPGENS];

    // FIXME: If the hm2_bb driver on the ARM tries to load before
    // hm2_idrom_setup() finishes, it'll see an incomplete/invalid
    // IDROM structure.  :-(
    hm2_idrom_setup();

    for (int i = 0; i < NUM_STEPGENS; i ++) {
        // stepgen period, 0 means disable
        shrdram2[stepgen_start + (i * 0x10)] = 0;

        stepgen[i].count = 0;
        stepgen[i].old_cycles = 0;
    }

    stepgen[0].step_bit = 5;   // P9_27
    stepgen[0].dir_bit = 2;    // P9_30

    stepgen[1].step_bit = 15;  // P8_11
    stepgen[1].dir_bit = 14;   // P8_12

    stepgen[2].step_bit = 0;
    stepgen[2].dir_bit = 0;

    stepgen[3].step_bit = 0;
    stepgen[3].dir_bit = 0;

    // enable cycle counter
    PRU_CTRL.CYCLE = 0;
    PRU_CTRL.CONTROL_bit.COUNTER_ENABLE = 1;

    do {
        uint32_t out;

        // FIXME: there should be an atomic copy-and-zero here
        cycles += PRU_CTRL.CYCLE;
        PRU_CTRL.CYCLE = 0;
        // shrdram2[1] = cycles;

        out = 0;

        for (int i = 0; i < NUM_STEPGENS; i ++) {
            stepgen[i].period = shrdram2[stepgen_start + (i * 0x10)];

            if (stepgen[i].period == 0) {
                stepgen[i].dir = 0;
                stepgen[i].step = 0;
            } else {
                stepgen[i].dir = (stepgen[i].period & 0x80000000) >> 31;

                // FIXME wrap around?
                if ((cycles - stepgen[i].old_cycles) > (stepgen[i].period & 0x7fffffff)) {
                    stepgen[i].count ++;
                    stepgen[i].old_cycles = cycles;
                }

                stepgen[i].step = stepgen[i].count & 0x1;
            }

            out |= stepgen[i].step << stepgen[i].step_bit;
            out |= stepgen[i].dir << stepgen[i].dir_bit;
        }

        write_r30(out);

        // delay_us(1);
    } while(1);

    // disable cycle counter
    PRU_CTRL.CONTROL_bit.COUNTER_ENABLE = 0;

    return 0;
}
