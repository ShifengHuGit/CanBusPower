#******************************************************************************
#
# Makefile - Rules for building the CAN Device LED Switch.
#
# Copyright (c) 2007-2009 Luminary Micro, Inc.  All rights reserved.
# Software License Agreement
# 
# Luminary Micro, Inc. (LMI) is supplying this software for use solely and
# exclusively on LMI's microcontroller products.
# 
# The software is owned by LMI and/or its suppliers, and is protected under
# applicable copyright laws.  All rights are reserved.  You may not combine
# this software with "viral" open-source software in order to form a larger
# program.  Any use in violation of the foregoing restrictions may subject
# the user to criminal sanctions under applicable laws, as well as to civil
# liability for the breach of the terms and conditions of this license.
# 
# THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
# OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
# LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
# CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 4423 of the EK-LM3S8962 Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM3S2110

#
# The base directory for DriverLib.
#
ROOT=../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find header files that do not live in the source directory.
#
IPATH=../../..

#
# The default rule, which causes the CAN Device LED Switch to be built.
#
all: ${COMPILER}
all: ${COMPILER}/can_device_led.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir ${COMPILER}

#
# Rules for building the CAN Device LED Switch.
#
${COMPILER}/can_device_led.axf: ${COMPILER}/can_device_led.o
${COMPILER}/can_device_led.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/can_device_led.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
ifeq (${COMPILER}, gcc)
${COMPILER}/can_device_led.axf: can_device_led.ld
endif
SCATTERgcc_can_device_led=can_device_led.ld
ifeq (${COMPILER}, sourcerygxx)
${COMPILER}/can_device_led.axf: can_device_led_sourcerygxx.ld
endif
SCATTERsourcerygxx_can_device_led=lm3s2110-rom.ld -T can_device_led_sourcerygxx.ld
ENTRY_can_device_led=ResetISR

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
