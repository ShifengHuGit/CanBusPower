SD card using FAT file system

This example application demonstrates reading a file system from
an SD card.  It makes use of FatFs, a FAT file system driver.  It
provides a simple command console via a serial port for issuing commands
to view and navigate the file system on the SD card.

The first UART, which is connected to the FTDI virtual serial port
on the Stellaris LM3S6965 Evaluation Board, is configured for 115,200
bits per second, and 8-n-1 mode.  When the program is started a message
will be printed to the terminal.  Type ``help'' for command help.

For additional details about FatFs, see the following site:
http://elm-chan.org/fsw/ff/00index_e.html

-------------------------------------------------------------------------------

Copyright (c) 2007-2009 Luminary Micro, Inc.  All rights reserved.
Software License Agreement

Luminary Micro, Inc. (LMI) is supplying this software for use solely and
exclusively on LMI's microcontroller products.

The software is owned by LMI and/or its suppliers, and is protected under
applicable copyright laws.  All rights are reserved.  You may not combine
this software with "viral" open-source software in order to form a larger
program.  Any use in violation of the foregoing restrictions may subject
the user to criminal sanctions under applicable laws, as well as to civil
liability for the breach of the terms and conditions of this license.

THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

This is part of revision 4423 of the EK-LM3S8962 Firmware Package.
