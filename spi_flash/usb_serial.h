/*
 * usb_serial.h
 *
 *  Created on: Apr 14, 2013
 *      Author: root
 */

#ifndef USB_SERIAL_H_
#define USB_SERIAL_H_

void USBSerialInit(void);
int USBSerialWrite(char* buffer, int len);
int USBSerialRead(char* buffer, int len);

#endif /* USB_SERIAL_H_ */
