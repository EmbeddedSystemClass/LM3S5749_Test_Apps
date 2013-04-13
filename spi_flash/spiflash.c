/*
 * spiflash.c
 *
 *  Created on: Dec 20, 2012
 *      Author: root
 */
#include "spiflash.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

#include "utils/uartstdio.h"

// Instruction Definitions
#define WREN		0x06 // Write Enable
#define WRDI		0x04 // Write Disable
#define RDID		0x9F // Read Identification
#define RDSR		0x05 // Read Status Register
#define WRSR		0x01 // Write Status Register
#define READ		0x03 // Read Data Bytes
#define FAST_READ	0x0B // Read Data Bytes at Higher Speed
#define PP			0x02 // Page Program
#define SE			0xD8 // Sector Erase
#define BE			0xC7 // Bulk Erase
#define DP			0xB9 // Deep Power-Down
#define RES			0xAB // Release from Deep Power-Down

// Status Register Definitions
#define SRWD		0x80 // Status Register Write Protect
#define BP2			0x10 //
#define BP1			0x08 // Block Protect bits
#define BP0			0x04 //
#define WEL			0x02 // Write Enable Latch bit
#define WIP			0x01 // Writ in Progress bit

// Upper level interface
//void flashWrite(unsigned int addr, unsigned char* data, unsigned int count);
//void flashRead(unsigned int addr, unsigned char* data, unsigned int count);

// Low Level Calls
void SPIFLASH_Init(void);
void SPIFLASH_Write_Enable(void);
void SPIFLASH_Write_Disable(void);
void SPIFLASH_Read_Identification(unsigned char*);
void SPIFLASH_Read_Status_Register(unsigned char*);
void SPIFLASH_Write_Status_Register(unsigned char);
void SPIFLASH_Read_Data_Bytes(unsigned int, unsigned char*, unsigned int);
void SPIFLASH_Fast_Read_Data_Bytes(unsigned int, unsigned char*, unsigned int);
void SPIFLASH_Page_Program(unsigned int, unsigned char*, unsigned int);
void SPIFLASH_Sector_Erase(unsigned int);
void SPIFLASH_Bulk_Erase(void);
bool SPIFLASH_Write_In_Progress(void);

// TODO: Define these with real driver calls
#define SPI_Write(x, n)
#define SPI_Read(x, n)

static unsigned long int rx_buff;

void SPIFLASH_Init(void)
{
	/*
	PA2 - CLK
	PA3 - SS
	PA4 - RX
	PA5 - TX
	*/
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	ROM_GPIOPinTypeSSI(GPIO_PORTA_BASE, (GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5));
	ROM_SSIDisable(SSI0_BASE);
	ROM_SSIConfigSetExpClk(SSI0_BASE, ROM_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);
	ROM_SSIEnable(SSI0_BASE);

	// GPIO control of SS pin
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

__inline__ void SPIFLASH_Write_Enable(void)
{
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	ROM_SSIDataPut(SSI0_BASE, WREN);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

__inline__ void SPIFLASH_Write_Disable(void)
{
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	ROM_SSIDataPut(SSI0_BASE, WRDI);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

void SPIFLASH_Read_Identification(unsigned char* data)
{
	// NOT IMPLEMENTED IN SILICON REV
	unsigned long int id[19];
	unsigned char i;
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	ROM_SSIDataPut(SSI0_BASE, RDID);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	for(i = 0; i < 19; i++)
	{
		ROM_SSIDataPut(SSI0_BASE, 0);
		ROM_SSIDataGet(SSI0_BASE, &id[i]);
	}
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
	for(i = 0; i < 19; i++)
		data[i] = id[i];
}

void SPIFLASH_Read_Status_Register(unsigned char* status)
{
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	ROM_SSIDataPut(SSI0_BASE, RDSR);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, 0);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
	*status = rx_buff & 0xff;
}

void SPIFLASH_Write_Status_Register(unsigned char status)
{
	SPIFLASH_Write_Enable();
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	ROM_SSIDataPut(SSI0_BASE, WRSR);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, status);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

void SPIFLASH_Read_Data_Bytes(unsigned int addr, unsigned char* data, unsigned int count)
{
	unsigned int i;
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	ROM_SSIDataPut(SSI0_BASE, READ);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, (addr >> 16) & 0xff);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, (addr >> 8) & 0xff);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, addr & 0xff);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	for(i = 0; i < count; i++)
	{
		ROM_SSIDataPut(SSI0_BASE, 0);
		ROM_SSIDataGet(SSI0_BASE, &rx_buff);
		data[i] = rx_buff & 0xff;
	}
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

void SPIFLASH_Fast_Read_Data_Bytes(unsigned int addr, unsigned char* data, unsigned int count)
{
	// Can set serial clock to 75MHz
	/*
	SPI_Write(FAST_READ, 1);
	SPI_Write((addr >> 16) & 0xff, 1);
	SPI_Write((addr >> 8) & 0xff, 1);
	SPI_Write(addr & 0xff, 1);
	SPI_Read(data, count);
	*/
}

void SPIFLASH_Page_Program(unsigned int addr, unsigned char* data, unsigned int count)
{
	unsigned int i;
	SPIFLASH_Write_Enable();
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	ROM_SSIDataPut(SSI0_BASE, PP);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, (addr >> 16) & 0xff);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, (addr >> 8) & 0xff);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, addr & 0xff);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);

	for(i = 0; i < count; i++)
	{
		ROM_SSIDataPut(SSI0_BASE, data[i]);
		ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	}

	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
	// Check WIP bit in status register
	while(SPIFLASH_Write_In_Progress())
		;
}

void SPIFLASH_Sector_Erase(unsigned int addr)
{
	SPIFLASH_Write_Enable();
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	ROM_SSIDataPut(SSI0_BASE, SE);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, (addr >> 16) & 0xff);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, (addr >> 8) & 0xff);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_SSIDataPut(SSI0_BASE, addr & 0xff);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
	// Check WIP bit in status register
	while(SPIFLASH_Write_In_Progress())
		;
}

void SPIFLASH_Bulk_Erase(void)
{
	SPIFLASH_Write_Enable();
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	ROM_SSIDataPut(SSI0_BASE, BE);
	ROM_SSIDataGet(SSI0_BASE, &rx_buff);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
	// Check WIP bit in status register
	while(SPIFLASH_Write_In_Progress())
		;
}

bool SPIFLASH_Write_In_Progress(void)
{
	unsigned char status = 0;
	SPIFLASH_Read_Status_Register(&status);
	return (status & WIP);
}
