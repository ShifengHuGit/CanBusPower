#******************************************************************************
#
# Makefile - Rules for building the Sample Ethernet Application using uIP.
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
VPATH=../../../third_party/uip-1.0/apps/dhcpc
VPATH+=../drivers
VPATH+=../../../third_party/uip-1.0/apps/httpd
VPATH+=../../../third_party/uip-1.0/uip
VPATH+=../../../utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=.
IPATH+=..
IPATH+=../../..
IPATH+=../../../third_party/uip-1.0/apps
IPATH+=../../../third_party/uip-1.0/apps/dhcpc
IPATH+=../../../third_party/uip-1.0/apps/httpd
IPATH+=../../../third_party/uip-1.0/uip
IPATH+=../../../third_party/uip-1.0

#
# The default rule, which causes the Sample Ethernet Application using uIP to be built.
#
all: ${COMPILER}
all: ${COMPILER}/enet_uip.axf

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
# Rules for building the Sample Ethernet Application using uIP.
#
${COMPILER}/enet_uip.axf: ${COMPILER}/dhcpc.o
${COMPILER}/enet_uip.axf: ${COMPILER}/enet_uip.o
${COMPILER}/enet_uip.axf: ${COMPILER}/httpd.o
${COMPILER}/enet_uip.axf: ${COMPILER}/rit128x96x4.o
${COMPILER}/enet_uip.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/enet_uip.axf: ${COMPILER}/uip.o
${COMPILER}/enet_uip.axf: ${COMPILER}/uip_arp.o
${COMPILER}/enet_uip.axf: ${COMPILER}/uip_timer.o
${COMPILER}/enet_uip.axf: ${COMPILER}/ustdlib.o
${COMPILER}/enet_uip.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
ifeq (${COMPILER}, gcc)
${COMPILER}/enet_uip.axf: enet_uip.ld
endif
SCATTERgcc_enet_uip=enet_uip.ld
ifeq (${COMPILER}, sourcerygxx)
${COMPILER}/enet_uip.axf: enet_uip_sourcerygxx.ld
endif
SCATTERsourcerygxx_enet_uip=lm3s8962-rom.ld -T enet_uip_sourcerygxx.ld
ENTRY_enet_uip=ResetISR

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
