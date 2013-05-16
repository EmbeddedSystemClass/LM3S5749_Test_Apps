//---------------------------------------------------------
/*
2.4in_TFT.c
Program for writing to Newhaven Display 2.4" TFT with ILI9340 controller
*/
//---------------------------------------------------------
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "lcd.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

/**************** I/O Setting **********************/
/*
[IM3..IM0] = 1001 8080 MCU 8-bit bus interface II

[DB15..DB12]= PB7..PB4 (yellow)
[DB11..DB8]	= PE7..PE4 (yellow)

(RESX)nRESET	= PA3 (red)
(CSX)nCS		= PA4 (red)
(D/CX)RS		= PA5 (red)
(WRX)nWR		= PA6 (red)
(RDX)nRD		= PA7 (red)
*/

// PIXEL INFO pg.66

#define PORTA_PINS (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)
#define PORTB_PINS (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)
#define PORTE_PINS (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)

#define RESX(x)	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, x ? GPIO_PIN_3 : 0)
#define CSX(x)	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, x ? GPIO_PIN_4 : 0)
#define DCX(x)	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, x ? GPIO_PIN_5 : 0)
#define WRX(x)	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, x ? GPIO_PIN_6 : 0)
#define RDX(x)	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, x ? GPIO_PIN_7 : 0)

#define SET_DATA_LINES_INPUT()  do{ GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, PORTB_PINS); \
									GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, PORTE_PINS); } while(0)
#define SET_DATA_LINES_OUTPUT() do{ GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, PORTB_PINS); \
									GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, PORTE_PINS); } while(0)
/***************************************************/

void configureGPIOs(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	// Configure control lines as outputs
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, PORTA_PINS);

	// Configure data lines as inputs
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, PORTB_PINS);
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, PORTE_PINS);

	// Set control lines
	RESX(0);
	CSX(1);
	DCX(1);
	WRX(1);
	RDX(1);
}

void delayNS(unsigned int delay)
{
	//vTaskDelay(delay * (portTICK_RATE_MS / 1000));
}

void delayMS(unsigned int delay)
{
	vTaskDelay(delay * portTICK_RATE_MS);
}

void setDataBus(unsigned char data)
{
	GPIOPinWrite(GPIO_PORTE_BASE, PORTE_PINS, (data & 0x0F) << 4);
	GPIOPinWrite(GPIO_PORTB_BASE, PORTB_PINS, data & 0xF0);
}

unsigned char getDataBus(void)
{
	unsigned char data;
	data = (GPIOPinRead(GPIO_PORTE_BASE, PORTE_PINS) & 0xF0) >> 4;
	data |= GPIOPinRead(GPIO_PORTB_BASE, PORTB_PINS) & 0xF0;
	return data;
}

void writeCycle(unsigned char cmd, unsigned char *data, unsigned int count)
{
	int i;
	CSX(0);
	SET_DATA_LINES_OUTPUT();
	DCX(0);
	WRX(0);
	setDataBus(cmd);
	delayNS(33);
	WRX(1);
	delayNS(10);
	DCX(1);
	for(i = 0; i < count; i++)
	{
		WRX(0);
		setDataBus(data[i]);
		delayNS(33);
		WRX(1);
		delayNS(33);
	}
	CSX(1);
}

void readCycle(unsigned char cmd, unsigned char *data, unsigned int count)
{
	int i;
	CSX(0);
	SET_DATA_LINES_OUTPUT();
	DCX(0);
	WRX(0);
	setDataBus(cmd);
	delayNS(33);
	WRX(1);
	delayNS(10);
	DCX(1);
	SET_DATA_LINES_INPUT();
	delayNS(33);
	RDX(0);
	delayNS(400);
	RDX(1);
	delayNS(100);
	for(i = 0; i < count; i++)
	{
		RDX(0);
		delayNS(400);
		data[i] = getDataBus();
		RDX(1);
		delayNS(100);
	}
	CSX(1);
}

void fillDisplay(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned int i;
	printf("fill rgb[0x%x][0x%x][0x%x]\n\r", r, g, b);

	writeCycle(0x2C, 0, 0); 		// Memory Write

	CSX(0);
	SET_DATA_LINES_OUTPUT();
	delayNS(33);

	for(i = 0; i < (320 * 240); i++)
	{
		WRX(0);
		setDataBus(r);
		delayNS(33);
		WRX(1);
		delayNS(33);
		WRX(0);
		setDataBus(g);
		delayNS(33);
		WRX(1);
		delayNS(33);
		WRX(0);
		setDataBus(b);
		delayNS(33);
		WRX(1);
		delayNS(33);
	}
	CSX(1);
}

void lcdInit(void)
{
	unsigned char buffer[128];
	configureGPIOs();

	// Reset the chip
	RESX(0);
	delayMS(100);
	RESX(1);
	delayMS(500);

	writeCycle(0x01, 0, 0); 		// Software Reset
	delayMS(500);

	writeCycle(0x29, 0, 0); 		// Display ON
	buffer[0] = 0x00;
	writeCycle(0x11, buffer, 1); 	// Exit Sleep Mode
	buffer[0] = 0x26;
	buffer[1] = 0x04;
	writeCycle(0xC0, buffer, 2);	// Power Control 1
	buffer[0] = 0x04;
	writeCycle(0xC1, buffer, 1);	// Power Control 2
	buffer[0] = 0x34;
	buffer[1] = 0x40;
	writeCycle(0xC5, buffer, 2);	// VCOM Control 1
	buffer[0] = 0x88;
	writeCycle(0x36, buffer, 1);	// Memory Access Control BGR
	buffer[0] = 0x00;
	buffer[1] = 0x18;
	writeCycle(0xB1, buffer, 2);	// Frame Rate Control
	buffer[0] = 0x0A;
	buffer[1] = 0xA2;
	writeCycle(0xB6, buffer, 2);	// Display Function Control
	buffer[0] = 0xC0;
	writeCycle(0xC7, buffer, 1); 	// VCOM Control 2
	//buffer[0] = 0x55;
	//writeCycle(0x3A, buffer, 1); 	// Pixel Format 16-bpp
	buffer[0] = 0x66;
	writeCycle(0x3A, buffer, 1); 	// Pixel Format 18-bpp

	buffer[0] = 0x1F;
	buffer[1] = 0x1B;
	buffer[2] = 0x18;
	buffer[3] = 0x0B;
	buffer[4] = 0x0F;
	buffer[5] = 0x09;
	buffer[6] = 0x46;
	buffer[7] = 0xB5;
	buffer[8] = 0x37;
	buffer[9] = 0x0A;
	buffer[10] = 0x0C;
	buffer[11] = 0x07;
	buffer[12] = 0x07;
	buffer[13] = 0x05;
	buffer[14] = 0x00;
	writeCycle(0xE0, buffer, 15); 	// Positive Gamma Correction

	buffer[0] = 0x00;
	buffer[1] = 0x24;
	buffer[2] = 0x27;
	buffer[3] = 0x04;
	buffer[4] = 0x10;
	buffer[5] = 0x06;
	buffer[6] = 0x39;
	buffer[7] = 0x74;
	buffer[8] = 0x48;
	buffer[9] = 0x05;
	buffer[10] = 0x13;
	buffer[11] = 0x38;
	buffer[12] = 0x38;
	buffer[13] = 0x3A;
	buffer[14] = 0x1F;
	writeCycle(0xE1, buffer, 15); 	// Negative Gamma Correction

	buffer[0] = 0x00;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0xEF;
	writeCycle(0x2A, buffer, 4); 	// Column Address SC=0x00 EC=0xEF

	buffer[0] = 0x00;
	buffer[1] = 0x00;
	buffer[2] = 0x01;
	buffer[3] = 0x3F;
	writeCycle(0x2B, buffer, 4); 	// Page Address SP=0x00 EP=0x13F
}
