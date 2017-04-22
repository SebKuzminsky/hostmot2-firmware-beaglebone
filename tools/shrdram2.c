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
// This is a Linux userspace tool to read and write the SHRDRAM2 memory
// region of the Beaglebone, which is shared between the ARM and the
// two PRUs.
//

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/mman.h>


#define SHRDRAM2_OFFSET 0x4a310000 // PA as seen by the Linux kernel, see DTB
#define SHRDRAM2_LENGTH 0x3000     // 12 kB


void usage(void) {
    printf("usage: shrdram2 ADDR [VAL]\n");
    printf("This program needs permission to read and write /dev/mem, so you probably\n");
    printf("have to run it as root.\n");
}


int main(int argc, char *argv[]) {
    int fd;
    uint32_t *shrdram2;
    int addr;
    uint32_t val;

    if ((argc != 2) && (argc != 3)) {
        usage();
        return 1;
    }

    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
        usage();
        return 0;
    }

    fd = open("/dev/mem", O_RDWR);
    if (fd < 0) {
        printf("failed to open /dev/mem: %s\n", strerror(errno));
        return 1;
    }

    shrdram2 = mmap(0, SHRDRAM2_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SHRDRAM2_OFFSET);
    if (shrdram2 == MAP_FAILED) {
        printf("error mapping pruss shrdram2: %s\n", strerror(errno));
        return 1;
    }

    addr = strtol(argv[1], NULL, 0);

    if (argc == 2) {
        val = shrdram2[addr];
        printf("read shrdram2[0x%04x] = 0x%08x (%d)\n", addr, val, val);
    } else {
        val = strtoul(argv[2], NULL, 0);
        shrdram2[addr] = val;
        printf("write shrdram2[0x%04x] = 0x%08x (%d)\n", addr, val, val);
    }

    return 0;
}
