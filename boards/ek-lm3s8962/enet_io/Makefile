#******************************************************************************
#
# Makefile - Rules for building the Sample Ethernet I/O Control Application using lwIP 1.3.0.
#
# Copyright (c) 2008-2009 Luminary Micro, Inc.  All rights reserved.
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
PART=LM3S8962

#
# The base directory for DriverLib.
#
ROOT=../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=../drivers
VPATH+=../../../third_party/lwip-1.3.0/apps/httpserver_raw
VPATH+=../../../utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=.
IPATH+=..
IPATH+=../../..
IPATH+=../../../third_party/lwip-1.3.0/apps
IPATH+=../../../third_party/lwip-1.3.0/ports/stellaris/include
IPATH+=../../../third_party/lwip-1.3.0/src/include
IPATH+=../../../third_party/lwip-1.3.0/src/include/ipv4

#
# The default rule, which causes the Sample Ethernet I/O Control Application using lwIP 1.3.0 to be built.
#
all: ${COMPILER}
all: ${COMPILER}/enet_io.axf

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
# Rules for building the Sample Ethernet I/O Control Application using lwIP 1.3.0.
#
${COMPILER}/enet_io.axf: ${COMPILER}/cgifuncs.o
${COMPILER}/enet_io.axf: ${COMPILER}/enet_io.o
${COMPILER}/enet_io.axf: ${COMPILER}/httpd.o
${COMPILER}/enet_io.axf: ${COMPILER}/io.o
${COMPILER}/enet_io.axf: ${COMPILER}/lmi_fs.o
${COMPILER}/enet_io.axf: ${COMPILER}/lwiplib.o
${COMPILER}/enet_io.axf: ${COMPILER}/rit128x96x4.o
${COMPILER}/enet_io.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/enet_io.axf: ${COMPILER}/ustdlib.o
${COMPILER}/enet_io.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
ifeq (${COMPILER}, gcc)
${COMPILER}/enet_io.axf: enet_io.ld
endif
SCATTERgcc_enet_io=enet_io.ld
ifeq (${COMPILER}, sourcerygxx)
${COMPILER}/enet_io.axf: enet_io_sourcerygxx.ld
endif
SCATTERsourcerygxx_enet_io=lm3s8962-rom.ld -T enet_io_sourcerygxx.ld
ENTRY_enet_io=ResetISR

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
