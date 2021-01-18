Boot Loader Demo 2

An example to demonstrate the use of the Serial or Ethernet boot loader.
After being started by the boot loader, the application will configure the
UART and Ethernet controller, wait for the select button to be pressed,
and then branch back to the boot loader to await the start of an update.
The UART will always be configured at 115,200 baud and does not require the
use of auto-bauding.  The Ethernet controller will be configured for basic
operation and enabled.  Reprogramming using the Ethernet boot loader will
require both the MAC and IP Addresses.  The MAC address will be displayed
at the bottom of the screen.  Since a TCP/IP stack is not being used in
this demo application, an IP address will need to be selected that will be
on the same subnet as the PC that is being used to program the flash, but
is not in conflict with any IP addresses already present on the network. 

Both the boot loader and the application must be placed into flash.  Once
the boot loader is in flash, it can be used to program the application into
flash as well.  Then, the boot loader can be used to replace the
application with another.

The boot_demo1 application can be used along with this application to
easily demonstrate that the boot loader is actually updating the on-chip
flash.

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
