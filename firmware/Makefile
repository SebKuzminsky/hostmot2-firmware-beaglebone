
CC = pru-gcc
CFLAGS = -Os -Wall -Wextra

# Define this to squeeze code size by removing atexit, exit, constructors
# and destructors from CRT.
CFLAGS += -minrt

# Per-PRU core flags. The -mmcu option will select the correct linker
# script and will predefine mcu-specific macros.
CFLAGS0 = -mmcu=am335x.pru0
CFLAGS1 = -mmcu=am335x.pru1

# List of source files to compile for each PRU core.
SRC0 := main0.c
SRC1 := main1.c

# GCC's -MMD does not yield the needed C dependencies when compiling all
# C source files at once. So manually list headers here.
HEADERS := $(wildcard *.h)

# Final ELF image file names
ELF0 := pru0.elf
ELF1 := pru1.elf


all: $(ELF0) $(ELF1)

$(ELF0): $(SRC0) $(HEADERS)
	$(CC) $(CFLAGS) $(CFLAGS0) $(SRC0) $(LDFLAGS) -o $@

$(ELF1): $(SRC1) $(HEADERS)
	$(CC) $(CFLAGS) $(CFLAGS1) $(SRC1) $(LDFLAGS) -o $@

config-pins:
	config-pin overlay cape-universala
	config-pin P9_30 pruout
	config-pin P9_27 pruout

install: $(ELF0) $(ELF1)
	install -D -m 0444 $(ELF0) $(DESTDIR)/lib/firmware/am335x-pru0-fw
	install -D -m 0444 $(ELF1) $(DESTDIR)/lib/firmware/am335x-pru1-fw

reload: install
	rmmod -f pru_rproc
	modprobe pru_rproc

clean:
	$(RM) -f $(ELF0) $(ELF1)

.PHONY: all config-pins install reload clean
