# Hostmot2 Firmware for the Beaglebone PRUSS

This repo contains source code for Hostmot2 firmware for the Beaglebone
PRUSS.  It's intended to interface to LinuxCNC (running on the same
Beaglebone), using the PRUs in place of the usual Mesa FPGAs.

The PRU firmware is written in C, and intended to be compiled by the
PRU port of GCC and the GNU Binutils:

    https://github.com/dinuxbg/gnupru

The firmware is intended to be loaded by the Linux Remoteproc system.
Some instructions on setting up remoteproc on the Beaglebone are here:

    https://github.com/dinuxbg/pru-gcc-examples/blob/master/REMOTEPROC.md
