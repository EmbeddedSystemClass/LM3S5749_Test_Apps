#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "spiflash.h"
#include "spiflash_cli.h"
#include "ustdlib.h"
#include "utils/uartstdio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PRINT_ROW_CNT  16

/* CLI Callback Definitions */
static portBASE_TYPE readStatusRegisterCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	unsigned char regVal;
	SPIFLASH_Read_Status_Register(&regVal);
	memcpy(writeBuffer, "cmd output", 10);
	usprintf((char*)writeBuffer, "Status Reg = 0x%x\n", regVal);
	writeBufferLen = strlen((char*)writeBuffer);
	return ret;
}

static portBASE_TYPE writeStatusRegisterCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	long int paramLen = 0;
	const int8_t* param = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);
	memcpy(writeBuffer, param, paramLen);
	writeBufferLen = paramLen;
	return ret;
}

static portBASE_TYPE dispCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	long int paramLen = 0;
	int addr, count, i;
	unsigned char numBlanks;
	unsigned char* data;

	// TODO: Clean up this mess of crap
	const int8_t* param = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);
	addr = atoi((char*)param);
	numBlanks = addr % PRINT_ROW_CNT;
	param = FreeRTOS_CLIGetParameter(cmdString, 2, &paramLen);
	count = atoi((char*)param);

	data = malloc(count);
	SPIFLASH_Read_Data_Bytes(addr, data, count);

	if(numBlanks)
	{
		UARTprintf("\r\n[0x%04x]  ", addr - numBlanks);
		for(; numBlanks > 0; numBlanks--)
			UARTprintf(".. ");
	}

	for(i = 0; count > 0; count--, addr++)
	{
		if(!(addr % PRINT_ROW_CNT))
			UARTprintf("\r\n[0x%04x]  ", addr);
		UARTprintf("%02x ", data[i++]);
	}
	UARTprintf("\r\n");

	*writeBuffer = 0;
	writeBufferLen = 0;
	return ret;
}

static portBASE_TYPE sectorEraseCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	unsigned int sector;
	long int paramLen = 0;
	const int8_t* param = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);
	sector = atoi((char*)param);
	UARTprintf("Erasing Sector %d...\r\n", sector / FLASH_SECTOR_SIZE);
	SPIFLASH_Sector_Erase(sector);
	*writeBuffer = 0;
	writeBufferLen = 0;
	return ret;
}

static portBASE_TYPE writeCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	long int paramLen = 0;
	int addr;
	unsigned char data;

	const int8_t* param = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);
	addr = atoi((char*)param);
	param = FreeRTOS_CLIGetParameter(cmdString, 2, &paramLen);
	data = atoi((char*)param);
	SPIFLASH_Page_Program(addr, &data, 1);

	*writeBuffer = 0;
	writeBufferLen = 0;
	return ret;
}





/* CLI Command Definitions */
static const CLI_Command_Definition_t readStatusRegisterCmd =
{
	(const int8_t* const) "SpiFlashReadStatusReg",
	(const int8_t* const) "SpiFlashReadStatusReg:\r\n Reads the SPI Flash status register\r\n\r\n",
	readStatusRegisterCbk,
	0
};

static const CLI_Command_Definition_t writeStatusRegisterCmd =
{
	(const int8_t* const) "SpiFlashWriteStatusReg",
	(const int8_t* const) "SpiFlashWriteStatusReg:\r\n Write [param1] to SPI Flash status register\r\n\r\n",
	writeStatusRegisterCbk,
	1
};

static const CLI_Command_Definition_t dispCmd =
{
	(const int8_t* const) "disp",
	(const int8_t* const) "disp:\r\n Display [param2] number of bytes of SPI Flash data starting at address [param1]\r\n\r\n",
	dispCbk,
	2
};

static const CLI_Command_Definition_t sectorEraseCmd =
{
	(const int8_t* const) "SectorErase",
	(const int8_t* const) "SectorErase:\r\n Erase sector [param1] of SPI Flash\r\n\r\n",
	sectorEraseCbk,
	1
};

static const CLI_Command_Definition_t writeCmd =
{
	(const int8_t* const) "w",
	(const int8_t* const) "w:\r\n Write byte [param2] to address [param1] of SPI Flash\r\n\r\n",
	writeCbk,
	2
};




/* Called at startup to register commands */
void SpiFlashRegisterCmds(void)
{
	FreeRTOS_CLIRegisterCommand(&readStatusRegisterCmd);
	FreeRTOS_CLIRegisterCommand(&writeStatusRegisterCmd);
	FreeRTOS_CLIRegisterCommand(&dispCmd);
	FreeRTOS_CLIRegisterCommand(&sectorEraseCmd);
	FreeRTOS_CLIRegisterCommand(&writeCmd);
}
