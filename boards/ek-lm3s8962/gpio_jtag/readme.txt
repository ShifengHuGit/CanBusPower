GPIO JTAG Recovery

This example demonstrates changing the JTAG pins into GPIOs, along with a
mechanism to revert them to JTAG pins.  When first run, the pins remain in
JTAG mode.  Pressing the select push button will toggle the pins between
JTAG mode and GPIO mode.  Because there is no debouncing of the push button
(either in hardware or software), a button press will occasionally result
in more than one mode change.

In this example, all five pins (PB7, PC0, PC1, PC2, and PC3) are switched,
though the more typical use would be to change PB7 into a GPIO.

-------------------------------------------------------------------------------

Copyright (c) 2006-2009 Luminary Micro, Inc.  All rights reserved.
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
