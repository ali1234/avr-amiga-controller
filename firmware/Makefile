#
#             LUFA Library
#     Copyright (C) Dean Camera, 2012.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

MCU          = atmega32u2
ARCH         = AVR8
BOARD        = MINIMUS
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = avr-amiga-controller
SRC          = main.c descriptors.c $(LUFA_SRC_USB)
LUFA_PATH    = ../../LUFA-120730/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -Iconfig/
LD_FLAGS     =

# Default target
all:

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk

flash: $(TARGET).hex
	dfu-programmer atmega32u2 erase
	dfu-programmer atmega32u2 flash $(TARGET).hex
	dfu-programmer atmega32u2 start


