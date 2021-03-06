#******************************************************************************
#
# Makefile - Rules for building the FreeRTOS example.
#
# Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 9453 of the LM3S5749 Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM3S5749

#
# Set the processor variant.
#
VARIANT=cm3

#
# The base directory for StellarisWare.
#
ROOT=../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=./FreeRTOSV7.3.0/FreeRTOS/Source/portable/GCC/ARM_CM3
VPATH+=./FreeRTOSV7.3.0/FreeRTOS/Source/portable/MemMang
VPATH+=./FreeRTOSV7.3.0/FreeRTOS/Source
VPATH+=./FreeRTOSV7.3.0/FreeRTOS-Plus/FreeRTOS-Plus-CLI
VPATH+=../../../utils


#
# Where to find header files that do not live in the source directory.
#
IPATH=.
IPATH+=..
IPATH+=../../..
IPATH+=../../../utils
IPATH+=./FreeRTOSV7.3.0/FreeRTOS
IPATH+=./FreeRTOSV7.3.0/FreeRTOS/Source/include
IPATH+=./FreeRTOSV7.3.0/FreeRTOS/Source/portable/GCC/ARM_CM3
IPATH+=./FreeRTOSV7.3.0/FreeRTOS-Plus/FreeRTOS-Plus-CLI

#
# The default rule, which causes the FreeRTOS example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/freertos_demo.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the FreeRTOS example.
#
${COMPILER}/freertos_demo.axf: 	${COMPILER}/startup_${COMPILER}.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/heap_2.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/syscalls.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/test_task.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/timer.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/lcd.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/FreeRTOS_CLI.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/cli_task.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/misc_cli.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/list.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/port.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/queue.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/tasks.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/uartstdio.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/ustdlib.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/usb_serial.o
${COMPILER}/freertos_demo.axf: 	${COMPILER}/freertos_demo.o
${COMPILER}/freertos_demo.axf: 	${ROOT}/usblib/${COMPILER}-cm3/libusb-cm3.a
${COMPILER}/freertos_demo.axf: 	${COMPILER}/usb_serial_structs.o
${COMPILER}/freertos_demo.axf: 	${ROOT}/driverlib/${COMPILER}-cm3/libdriver-cm3.a
${COMPILER}/freertos_demo.axf: 	boot_loader.ld
SCATTERgcc_freertos_demo=boot_loader.ld
ENTRY_freertos_demo=ResetISR
CFLAGSgcc=-DTARGET_IS_DUSTDEVIL_RA0

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
