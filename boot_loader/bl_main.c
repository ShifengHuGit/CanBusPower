//*****************************************************************************
//
// bl_main.c - The file holds the main control loop of the boot loader.
//
// Copyright (c) 2006-2009 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 4423 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "inc/hw_gpio.h"
#include "inc/hw_flash.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_ssi.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "bl_config.h"
#include "boot_loader/bl_commands.h"
#include "boot_loader/bl_decrypt.h"
#include "boot_loader/bl_i2c.h"
#include "boot_loader/bl_packet.h"
#include "boot_loader/bl_ssi.h"
#include "boot_loader/bl_uart.h"

//*****************************************************************************
//
// Make sure that the application start address is a multiple of 1024 bytes.
//
//*****************************************************************************
#if (APP_START_ADDRESS & 0x3ff)
#error ERROR: APP_START_ADDRESS must be a multiple of 1024 bytes!
#endif

//*****************************************************************************
//
// Make sure that the flash reserved space is a multiple of 1024 bytes.
//
//*****************************************************************************
#if (FLASH_RSVD_SPACE & 0x3ff)
#error ERROR: FLASH_RSVD_SPACE must be a multiple of 1024 bytes!
#endif

//*****************************************************************************
//
//! \addtogroup bl_main_api
//! @{
//
//*****************************************************************************
#if defined(I2C_ENABLE_UPDATE) || defined(SSI_ENABLE_UPDATE) || \
    defined(UART_ENABLE_UPDATE) || defined(DOXYGEN)

//*****************************************************************************
//
// A prototype for the function (in the startup code) for calling the
// application.
//
//*****************************************************************************
extern void CallApplication(unsigned long ulBase);

//*****************************************************************************
//
// A prototype for the function (in the startup code) for a predictable length
// delay.
//
//*****************************************************************************
extern void Delay(unsigned long ulCount);

//*****************************************************************************
//
// Holds the current status of the last command that was issued to the boot
// loader.
//
//*****************************************************************************
unsigned char g_ucStatus;

//*****************************************************************************
//
// This holds the current remaining size in bytes to be downloaded.
//
//*****************************************************************************
unsigned long g_ulTransferSize;

//*****************************************************************************
//
// This holds the current address that is being written to during a download
// command.
//
//*****************************************************************************
unsigned long g_ulTransferAddress;

//*****************************************************************************
//
// This is the data buffer used during transfers to the boot loader.
//
//*****************************************************************************
unsigned long g_pulDataBuffer[BUFFER_SIZE];

//*****************************************************************************
//
// This is an specially aligned buffer pointer to g_pulDataBuffer to make
// copying to the buffer simpler.  It must be offset to end on an address that
// ends with 3.
//
//*****************************************************************************
unsigned char *g_pucDataBuffer;

//*****************************************************************************
//
// Converts a word from big endian to little endian.  This macro uses compiler-
// specific constructs to perform an inline insertion of the "rev" instruction,
// which performs the byte swap directly.
//
//*****************************************************************************
#if defined(ewarm)
#include <intrinsics.h>
#define SwapWord(x)             __REV(x)
#endif
#if defined(codered) || defined(gcc) || defined(sourcerygxx)
#define SwapWord(x) __extension__                                \
        ({                                                       \
             register unsigned long __ret, __inp = x;            \
             __asm__("rev %0, %1" : "=r" (__ret) : "r" (__inp)); \
             __ret;                                              \
        })
#endif
#if defined(rvmdk) || defined(__ARMCC_VERSION)
#define SwapWord(x)             __rev(x)
#endif

//*****************************************************************************
//
//! Configures the microcontroller.
//!
//! This function configures the peripherals and GPIOs of the microcontroller,
//! preparing it for use by the boot loader.  The interface that has been
//! selected as the update port will be configured, and auto-baud will be
//! performed if required.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigureDevice(void)
{
#ifdef UART_ENABLE_UPDATE
    unsigned long ulProcRatio;
#endif

#ifdef CRYSTAL_FREQ
    //
    // Since the crystal frequency was specified, enable the main oscillator
    // and clock the processor from it.
    //
    HWREG(SYSCTL_RCC) &= ~(SYSCTL_RCC_MOSCDIS);
    Delay(524288);
    HWREG(SYSCTL_RCC) = ((HWREG(SYSCTL_RCC) & ~(SYSCTL_RCC_OSCSRC_M)) |
                         SYSCTL_RCC_OSCSRC_MAIN);

    //
    // Set the flash programming time based on the specified crystal frequency.
    //
    HWREG(FLASH_USECRL) = ((CRYSTAL_FREQ + 999999) / 1000000) - 1;
#else
    //
    // Set the flash to program at 16 MHz since that is just beyond the fastest
    // that we could run.
    //
    HWREG(FLASH_USECRL) = 15;
#endif

#ifdef I2C_ENABLE_UPDATE
    //
    // Enable the clocks to the I2C and GPIO modules.
    //
    HWREG(SYSCTL_RCGC2) = SYSCTL_RCGC2_GPIOB;
    HWREG(SYSCTL_RCGC1) = SYSCTL_RCGC1_I2C0;

    //
    // Configure the GPIO pins for hardware control, open drain with pull-up,
    // and enable them.
    //
    HWREG(GPIO_PORTB_BASE + GPIO_O_AFSEL) = (1 << 7) | I2C_PINS;
    HWREG(GPIO_PORTB_BASE + GPIO_O_DEN) = (1 << 7) | I2C_PINS;
    HWREG(GPIO_PORTB_BASE + GPIO_O_ODR) = I2C_PINS;

    //
    // Enable the I2C Slave Mode.
    //
    HWREG(I2C0_MASTER_BASE + I2C_O_MCR) = I2C_MCR_MFE | I2C_MCR_SFE;

    //
    // Setup the I2C Slave Address.
    //
    HWREG(I2C0_SLAVE_BASE + I2C_O_SOAR) = I2C_SLAVE_ADDR;

    //
    // Enable the I2C Slave Device on the I2C bus.
    //
    HWREG(I2C0_SLAVE_BASE + I2C_O_SCSR) = I2C_SCSR_DA;
#endif

#ifdef SSI_ENABLE_UPDATE
    //
    // Enable the clocks to the SSI and GPIO modules.
    //
    HWREG(SYSCTL_RCGC2) = SYSCTL_RCGC2_GPIOA;
    HWREG(SYSCTL_RCGC1) = SYSCTL_RCGC1_SSI0;

    //
    // Make the pin be peripheral controlled.
    //
    HWREG(GPIO_PORTA_BASE + GPIO_O_AFSEL) = SSI_PINS;
    HWREG(GPIO_PORTA_BASE + GPIO_O_DEN) = SSI_PINS;

    //
    // Set the SSI protocol to Motorola with default clock high and data
    // valid on the rising edge.
    //
    HWREG(SSI0_BASE + SSI_O_CR0) = (SSI_CR0_SPH | SSI_CR0_SPO |
                                    (DATA_BITS_SSI - 1));

    //
    // Enable the SSI interface in slave mode.
    //
    HWREG(SSI0_BASE + SSI_O_CR1) = SSI_CR1_MS | SSI_CR1_SSE;
#endif

#ifdef UART_ENABLE_UPDATE
    //
    // Enable the the clocks to the UART and GPIO modules.
    //
    HWREG(SYSCTL_RCGC2) = SYSCTL_RCGC2_GPIOA;
    HWREG(SYSCTL_RCGC1) = SYSCTL_RCGC1_UART0;

    //
    // Keep attempting to sync until we are successful.
    //
#ifdef UART_AUTOBAUD
    while(UARTAutoBaud(&ulProcRatio) < 0)
    {
    }
#else
    ulProcRatio = UART_BAUD_RATIO(UART_FIXED_BAUDRATE);
#endif

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    HWREG(GPIO_PORTA_BASE + GPIO_O_AFSEL) = UART_PINS;

    //
    // Set the pin type.
    //
    HWREG(GPIO_PORTA_BASE + GPIO_O_DEN) = UART_PINS;

    //
    // Set the baud rate.
    //
    HWREG(UART0_BASE + UART_O_IBRD) = ulProcRatio >> 6;
    HWREG(UART0_BASE + UART_O_FBRD) = ulProcRatio & UART_FBRD_DIVFRAC_M;

    //
    // Set data length, parity, and number of stop bits to 8-N-1.
    //
    HWREG(UART0_BASE + UART_O_LCRH) = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    //
    // Enable RX, TX, and the UART.
    //
    HWREG(UART0_BASE + UART_O_CTL) = (UART_CTL_UARTEN | UART_CTL_TXE |
                                      UART_CTL_RXE);

#ifdef UART_AUTOBAUD
    //
    // Need to ack in the UART case to hold it up while we get things set up.
    //
    AckPacket();
#endif
#endif
}

//*****************************************************************************
//
//! This function performs the update on the selected port.
//!
//! This function is called directly by the boot loader or it is called as a
//! result of an update request from the application.
//!
//! \return Never returns.
//
//*****************************************************************************
void
Updater(void)
{
    unsigned long ulSize, ulTemp, ulFlashSize;

    //
    // This ensures proper alignment of the global buffer so that the one byte
    // size parameter used by the packetized format is easily skipped for data
    // transfers.
    //
    g_pucDataBuffer = ((unsigned char *)g_pulDataBuffer) + 3;

    //
    // Insure that the COMMAND_SEND_DATA cannot be sent to erase the boot
    // loader before the application is erased.
    //
    g_ulTransferAddress = 0xffffffff;

    //
    // Read any data from the serial port in use.
    //
    while(1)
    {
        //
        // Receive a packet from the port in use.
        //
        ulSize = sizeof(g_pulDataBuffer) - 3;
        if(ReceivePacket(g_pucDataBuffer, &ulSize) != 0)
        {
            continue;
        }

        //
        // The first byte of the data buffer has the command and determines
        // the format of the rest of the bytes.
        //
        switch(g_pucDataBuffer[0])
        {
            //
            // This was a simple ping command.
            //
            case COMMAND_PING:
            {
                //
                // This command always sets the status to COMMAND_RET_SUCCESS.
                //
                g_ucStatus = COMMAND_RET_SUCCESS;

                //
                // Just acknowledge that the command was received.
                //
                AckPacket();

                //
                // Go back and wait for a new command.
                //
                break;
            }

            //
            // This command indicates the start of a download sequence.
            //
            case COMMAND_DOWNLOAD:
            {
                //
                // Until determined otherwise, the command status is success.
                //
                g_ucStatus = COMMAND_RET_SUCCESS;

                //
                // A simple do/while(0) control loop to make error exits
                // easier.
                //
                do
                {
                    //
                    // See if a full packet was received.
                    //
                    if(ulSize != 9)
                    {
                        //
                        // Indicate that an invalid command was received.
                        //
                        g_ucStatus = COMMAND_RET_INVALID_CMD;

                        //
                        // This packet has been handled.
                        //
                        break;
                    }

                    //
                    // Get the address and size from the command.
                    //
                    g_ulTransferAddress = SwapWord(g_pulDataBuffer[1]);
                    g_ulTransferSize = SwapWord(g_pulDataBuffer[2]);

                    //
                    // This determines the size of the flash available on the
                    // part in use.
                    //
                    ulFlashSize = (((HWREG(SYSCTL_DC0) &
                                     SYSCTL_DC0_FLASHSZ_M) + 1) << 11);

                    //
                    // If we are reserving space at the top of flash then this
                    // space is not available for application download but it
                    // is availble to be updated directly.
                    //
#ifdef FLASH_RSVD_SPACE
                    if((ulFlashSize - FLASH_RSVD_SPACE) != g_ulTransferAddress)
                    {
                        ulFlashSize -= FLASH_RSVD_SPACE;
                    }
#endif

                    //
                    // Check for a valid starting address and image size.
                    //
                    if((
#ifdef ENABLE_BL_UPDATE
                        (g_ulTransferAddress != 0) &&
#endif
#ifdef FLASH_RSVD_SPACE
                        (g_ulTransferAddress !=
                         (ulFlashSize - FLASH_RSVD_SPACE)) &&
#endif
                        (g_ulTransferAddress != APP_START_ADDRESS)) ||
                       ((g_ulTransferAddress + g_ulTransferSize) >
                        ulFlashSize) ||
                       ((g_ulTransferAddress & 3) != 0))
                    {
                        //
                        // Set the code to an error to indicate that the last
                        // command failed.  This informs the updater program
                        // that the download command failed.
                        //
                        g_ucStatus = COMMAND_RET_INVALID_ADR;

                        //
                        // This packet has been handled.
                        //
                        break;
                    }


                    //
                    // Only erase the space that we need if we are not
                    // protecting the code.
                    //
#ifndef FLASH_CODE_PROTECTION
                    ulFlashSize = g_ulTransferAddress + g_ulTransferSize;
#endif

                    //
                    // Clear the flash access interrupt.
                    //
                    HWREG(FLASH_FCMISC) = FLASH_FCMISC_AMISC;

                    //
                    // Leave the boot loader present until we start getting an
                    // image.
                    //
                    for(ulTemp = g_ulTransferAddress; ulTemp < ulFlashSize;
                        ulTemp += 0x400)
                    {
                        //
                        // Erase this block.
                        //
                        HWREG(FLASH_FMA) = ulTemp;
                        HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_ERASE;

                        //
                        // Wait until this block has been erased.
                        //
                        while(HWREG(FLASH_FMC) & FLASH_FMC_ERASE)
                        {
                        }
                    }

                    //
                    // Return an error if an access violation occurred.
                    //
                    if(HWREG(FLASH_FCRIS) & FLASH_FCRIS_ARIS)
                    {
                        g_ucStatus = COMMAND_RET_FLASH_FAIL;
                    }
                }
                while(0);

                //
                // See if the command was successful.
                //
                if(g_ucStatus != COMMAND_RET_SUCCESS)
                {
                    //
                    // Setting g_ulTransferSize to zero makes COMMAND_SEND_DATA
                    // fail to accept any data.
                    //
                    g_ulTransferSize = 0;
                }

                //
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.
                //
                AckPacket();

                //
                // Go back and wait for a new command.
                //
                break;
            }

            //
            // This command indicates that control should be transferred to
            // the specified address.
            //
            case COMMAND_RUN:
            {
                //
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.
                //
                AckPacket();

                //
                // See if a full packet was received.
                //
                if(ulSize != 5)
                {
                    //
                    // Indicate that an invalid command was received.
                    //
                    g_ucStatus = COMMAND_RET_INVALID_CMD;

                    //
                    // This packet has been handled.
                    //
                    break;
                }

                //
                // Get the address to which control should be transferred.
                //
                g_ulTransferAddress = SwapWord(g_pulDataBuffer[1]);

                //
                // This determines the size of the flash available on the
                // device in use.
                //
                ulFlashSize = (((HWREG(SYSCTL_DC0) &
                                 SYSCTL_DC0_FLASHSZ_M) + 1) << 11);

                //
                // Test if the transfer address is valid for this device.
                //
                if(g_ulTransferAddress >= ulFlashSize)
                {
                    //
                    // Indicate that an invalid address was specified.
                    //
                    g_ucStatus = COMMAND_RET_INVALID_ADR;

                    //
                    // This packet has been handled.
                    //
                    break;
                }

                //
                // Make sure that the ACK packet has been sent.
                //
                FlushData();

                //
                // Reset and disable the peripherals used by the boot loader.
                //
                HWREG(SYSCTL_RCGC1) = 0;
                HWREG(SYSCTL_RCGC2) = 0;
                HWREG(SYSCTL_SRCR1) = (SYSCTL_SRCR1_I2C0 | SYSCTL_SRCR1_SSI0 |
                                       SYSCTL_SRCR1_UART0);
                HWREG(SYSCTL_SRCR2) = SYSCTL_SRCR2_GPIOA | SYSCTL_SRCR2_GPIOB;
                HWREG(SYSCTL_SRCR1) = 0;
                HWREG(SYSCTL_SRCR2) = 0;

                //
                // Branch to the specified address.  This should never return.
                // If it does, very bad things will likely happen since it is
                // likely that the copy of the boot loader in SRAM will have
                // been overwritten.
                //
                ((void (*)(void))g_ulTransferAddress)();

                //
                // In case this ever does return and the boot loader is still
                // intact, simply reset the device.
                //
                HWREG(NVIC_APINT) = (NVIC_APINT_VECTKEY |
                                     NVIC_APINT_SYSRESETREQ);

                //
                // The microcontroller should have reset, so this should
                // never be reached.  Just in case, loop forever.
                //
                while(1)
                {
                }
            }

            //
            // This command just returns the status of the last command that
            // was sent.
            //
            case COMMAND_GET_STATUS:
            {
                //
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.
                //
                AckPacket();

                //
                // Return the status to the updater.
                //
                SendPacket(&g_ucStatus, 1);

                //
                // Go back and wait for a new command.
                //
                break;
            }

            //
            // This command is sent to transfer data to the device following
            // a download command.
            //
            case COMMAND_SEND_DATA:
            {
                //
                // Until determined otherwise, the command status is success.
                //
                g_ucStatus = COMMAND_RET_SUCCESS;

                //
                // If this is overwriting the boot loader then the application
                // has already been erased so now erase the boot loader.
                //
                if(g_ulTransferAddress == 0)
                {
                    //
                    // Clear the flash access interrupt.
                    //
                    HWREG(FLASH_FCMISC) = FLASH_FCMISC_AMISC;

                    //
                    // Erase the application before the boot loader.
                    //
                    for(ulTemp = 0; ulTemp < APP_START_ADDRESS;
                        ulTemp += 0x400)
                    {
                        //
                        // Erase this block.
                        //
                        HWREG(FLASH_FMA) = ulTemp;
                        HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_ERASE;

                        //
                        // Wait until this block has been erased.
                        //
                        while(HWREG(FLASH_FMC) & FLASH_FMC_ERASE)
                        {
                        }
                    }

                    //
                    // Return an error if an access violation occurred.
                    //
                    if(HWREG(FLASH_FCRIS) & FLASH_FCRIS_ARIS)
                    {
                        //
                        // Setting g_ulTransferSize to zero makes
                        // COMMAND_SEND_DATA fail to accept any more data.
                        //
                        g_ulTransferSize = 0;

                        //
                        // Indicate that the flash erase failed.
                        //
                        g_ucStatus = COMMAND_RET_FLASH_FAIL;
                    }
                }

                //
                // Take one byte off for the command.
                //
                ulSize = ulSize - 1;

                //
                // Check if there are any more bytes to receive.
                //
                if(g_ulTransferSize >= ulSize)
                {
                    //
                    // This function is a stub to show where to insert a
                    // function to decrypt the data as it is received.
                    //
#ifdef ENABLE_DECRYPTION
                    DecryptData(g_pucDataBuffer + 1, ulSize);
#endif

                    //
                    // Clear the flash access interrupt.
                    //
                    HWREG(FLASH_FCMISC) = FLASH_FCMISC_AMISC;

                    //
                    // Loop over the words to program.
                    //
                    for(ulTemp = 0; ulTemp < ((ulSize + 3) & ~3); ulTemp += 4)
                    {
                        //
                        // Program the next word.
                        //
                        HWREG(FLASH_FMA) = g_ulTransferAddress + ulTemp;
                        HWREG(FLASH_FMD) = g_pulDataBuffer[(ulTemp >> 2) + 1];
                        HWREG(FLASH_FMC) = FLASH_FMC_WRKEY | FLASH_FMC_WRITE;

                        //
                        // Wait until the word has been programmed.
                        //
                        while(HWREG(FLASH_FMC) & FLASH_FMC_WRITE)
                        {
                        }
                    }

                    //
                    // Return an error if an access violation occurred.
                    //
                    if(HWREG(FLASH_FCRIS) & FLASH_FCRIS_ARIS)
                    {
                        //
                        // Indicate that the flash programming failed.
                        //
                        g_ucStatus = COMMAND_RET_FLASH_FAIL;
                    }
                    else
                    {
                        //
                        // Now update the address to program.
                        //
                        g_ulTransferSize -= ulSize;
                        g_ulTransferAddress += ulSize;
                    }
                }
                else
                {
                    //
                    // This indicates that too much data is being sent to the
                    // device.
                    //
                    g_ucStatus = COMMAND_RET_INVALID_ADR;
                }

                //
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.
                //
                AckPacket();

                //
                // Go back and wait for a new command.
                //
                break;
            }

            //
            // This command is used to reset the device.
            //
            case COMMAND_RESET:
            {
                //
                // Send out a one-byte ACK to ensure the byte goes back to the
                // host before we reset everything.
                //
                AckPacket();

                //
                // Make sure that the ACK packet has been sent.
                //
                FlushData();

                //
                // Perform a software reset request.  This will cause the
                // microcontroller to reset; no further code will be executed.
                //
                HWREG(NVIC_APINT) = (NVIC_APINT_VECTKEY |
                                     NVIC_APINT_SYSRESETREQ);

                //
                // The microcontroller should have reset, so this should never
                // be reached.  Just in case, loop forever.
                //
                while(1)
                {
                }
            }

            //
            // Just acknowledge the command and set the error to indicate that
            // a bad command was sent.
            //
            default:
            {
                //
                // Acknowledge that this command was received correctly.  This
                // does not indicate success, just that the command was
                // received.
                //
                AckPacket();

                //
                // Indicate that a bad comand was sent.
                //
                g_ucStatus = COMMAND_RET_UNKNOWN_CMD;

                //
                // Go back and wait for a new command.
                //
                break;
            }
        }
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#endif
