/*
 * usb_serial.c
 *
 *  Created on: Apr 14, 2013
 *      Author: root
 */
#include "inc/hw_types.h"
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcdc.h"
#include "usb_serial_structs.h"
#include "usb_serial.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>

#define RX_QUEUE_SIZE 128

//*****************************************************************************
//
// Global flag indicating that a USB configuration has been set.
//
//*****************************************************************************
static volatile tBoolean g_bUSBConfigured = false;

// Receive and Transmit Queues
static xQueueHandle rxQueue;

//*****************************************************************************
//
// Get the communication parameters in use on the UART.
//
//*****************************************************************************
static void GetLineCoding(tLineCoding *psLineCoding)
{
	/* Hardcoded line coding */
    psLineCoding->ulRate = 115200;
    psLineCoding->ucDatabits = 8;
    psLineCoding->ucParity = USB_CDC_PARITY_NONE;
    psLineCoding->ucStop = USB_CDC_STOP_BITS_1;
}

//*****************************************************************************
//
// Handles CDC driver notifications related to control and setup of the device.
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ulEvent identifies the event we are being notified about.
// \param ulMsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to perform control-related
// operations on behalf of the USB host.  These functions include setting
// and querying the serial communication parameters, setting handshake line
// states and sending break conditions.
//
// \return The return value is event-specific.
//
//*****************************************************************************
unsigned long ControlHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgValue, void *pvMsgData)
{
    // Which event are we being asked to process?
    switch(ulEvent)
    {
        // We are connected to a host and communication is now possible.
        case USB_EVENT_CONNECTED:
            g_bUSBConfigured = true;
            // Flush our buffers.
            USBBufferFlush(&g_sTxBuffer);
            USBBufferFlush(&g_sRxBuffer);
            break;

        // The host has disconnected.
        case USB_EVENT_DISCONNECTED:
            g_bUSBConfigured = false;
            break;

        // Return the current serial communication parameters.
        case USBD_CDC_EVENT_GET_LINE_CODING:
            GetLineCoding(pvMsgData);
            break;

        // Ignore SUSPEND and RESUME for now.
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
            break;

        // We don't expect to receive any other events.
        default:
            break;
    }

    return(0);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to the transmit channel (data to
// the USB host).
//
// \param ulCBData is the client-supplied callback pointer for this channel.
// \param ulEvent identifies the event we are being notified about.
// \param ulMsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the transmit data channel (the IN channel carrying
// data to the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
unsigned long TxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgValue, void *pvMsgData)
{
    return(0);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to the receive channel (data from
// the USB host).
//
// \param ulCBData is the client-supplied callback data value for this channel.
// \param ulEvent identifies the event we are being notified about.
// \param ulMsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the receive data channel (the OUT channel carrying
// data from the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
unsigned long RxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgValue, void *pvMsgData)
{
	unsigned long ulRead;
	char ucChar;

    // Which event are we being sent?
    switch(ulEvent)
    {
        // A new packet has been received.
        case USB_EVENT_RX_AVAILABLE:
        {
			// Get a character from the buffer.
        	while(1 == (ulRead = USBBufferRead((tUSBBuffer *)&g_sRxBuffer, (unsigned char*)&ucChar, 1)))
        	{
				xQueueSendToBackFromISR(rxQueue, &ucChar, NULL);
        	}
            break;
        }

        // We are being asked how much unprocessed data we have still to
        // process. We return 0 if the UART is currently idle or 1 if it is
        // in the process of transmitting something. The actual number of
        // bytes in the UART FIFO is not important here, merely whether or
        // not everything previously sent to us has been transmitted.
        case USB_EVENT_DATA_REMAINING:
        {
            // Get the number of bytes in the buffer and add 1 if some data
            // still has to clear the transmitter.
        	return uxQueueMessagesWaitingFromISR(rxQueue);
        }

        // We are being asked to provide a buffer into which the next packet
        // can be read. We do not support this mode of receiving data so let
        // the driver know by returning 0. The CDC driver should not be sending
        // this message but this is included just for illustration and
        // completeness.
        case USB_EVENT_REQUEST_BUFFER:
        {
            return(0);
        }

        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        default:
            break;
    }
    return(0);
}

void USBSerialInit(void)
{
	// Create receive data queue
	rxQueue = xQueueCreate(RX_QUEUE_SIZE, sizeof(char));

	// Initialize the USB transmit and receive buffers.
	USBBufferInit((tUSBBuffer *)&g_sTxBuffer);
	USBBufferInit((tUSBBuffer *)&g_sRxBuffer);

	// Pass our device information to the USB library and place the device on the bus.
	USBDCDCInit(0, (tUSBDCDCDevice *)&g_sCDCDevice);

	// Wait until USB is configured
	while(!g_bUSBConfigured)
		;
}

int USBSerialWrite(char* buffer, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		while(1 != USBBufferWrite((tUSBBuffer *)&g_sTxBuffer, (unsigned char*)&buffer[i], 1))
			;
	}
	return len;
}

int USBSerialRead(char* buffer, int len)
{
	// Block forever waiting on receive queue
	xQueueReceive(rxQueue, buffer, portMAX_DELAY);
	return 1;
}
