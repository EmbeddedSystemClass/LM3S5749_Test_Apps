/*
 * misc_cli.c
 *
 *  Created on: May 11, 2013
 *      Author: root
 */

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include <string.h>
#include <stdlib.h>
#include "lcd.h"

/* CLI Callback Definitions */
static portBASE_TYPE printRunTimeStatsCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	vTaskGetRunTimeStats(writeBuffer);
	writeBufferLen = strlen((char*)writeBuffer);
	return ret;
}

static portBASE_TYPE testCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	unsigned char r, g, b;
	long int paramLen = 0;
	const int8_t* param = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);
	r = atoi((char*)param);
	param = FreeRTOS_CLIGetParameter(cmdString, 2, &paramLen);
	g = atoi((char*)param);
	param = FreeRTOS_CLIGetParameter(cmdString, 3, &paramLen);
	b = atoi((char*)param);

	fillDisplay(r, g, b);

	*writeBuffer = 0;
	writeBufferLen = 0;
	return ret;
}

static portBASE_TYPE boxCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	unsigned short x1, y1, x2, y2;
	unsigned char r, g, b;
	long int paramLen = 0;
	const int8_t* param;
	param = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);
	x1 = atoi((char*)param);
	param = FreeRTOS_CLIGetParameter(cmdString, 2, &paramLen);
	y1 = atoi((char*)param);
	param = FreeRTOS_CLIGetParameter(cmdString, 3, &paramLen);
	x2 = atoi((char*)param);
	param = FreeRTOS_CLIGetParameter(cmdString, 4, &paramLen);
	y2 = atoi((char*)param);
	param = FreeRTOS_CLIGetParameter(cmdString, 5, &paramLen);
	r = atoi((char*)param);
	param = FreeRTOS_CLIGetParameter(cmdString, 6, &paramLen);
	g = atoi((char*)param);
	param = FreeRTOS_CLIGetParameter(cmdString, 7, &paramLen);
	b = atoi((char*)param);

	drawBox(x1, y1, x2, y2, r, g, b);

	*writeBuffer = 0;
	writeBufferLen = 0;
	return ret;
}

/* CLI Command Definitions */
static const CLI_Command_Definition_t printRunTimeStatsCmd =
{
	(const int8_t* const) "printRunTimeStats",
	(const int8_t* const) "printRunTimeStats:\r\n  Prints the FreeRTOS runtime stats\r\n",
	printRunTimeStatsCbk,
	0
};

static const CLI_Command_Definition_t testCmd =
{
	(const int8_t* const) "t",
	(const int8_t* const) "t:\r\n  test\r\n",
	testCbk,
	3
};

static const CLI_Command_Definition_t boxCmd =
{
	(const int8_t* const) "box",
	(const int8_t* const) "box:\r\n  Draw box x1 y1 x2 y2 r g b\r\n",
	boxCbk,
	7
};

/* Called at startup to register commands */
void registerMiscCmds(void)
{
	FreeRTOS_CLIRegisterCommand(&printRunTimeStatsCmd);
	FreeRTOS_CLIRegisterCommand(&testCmd);
	FreeRTOS_CLIRegisterCommand(&boxCmd);
}
