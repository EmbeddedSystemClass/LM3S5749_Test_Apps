#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "utils/uartstdio.h"
#include "test_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>


#include "lcd.h"

#include "spiflash.h"

//*****************************************************************************
//
// The stack size for the LED toggle task.
//
//*****************************************************************************
#define TESTTASKSTACKSIZE        128         // Stack size in words

//*****************************************************************************
//
// Default LED toggle delay value. LED toggling frequency is twice this number.
//
//*****************************************************************************
#define LED_TOGGLE_DELAY        250

//*****************************************************************************
//
// This task toggles the user selected LED at a user selected frequency. User
// can make the selections by pressing the left and right buttons.
//
//*****************************************************************************
static void TestTask(void *pvParameters)
{
	lcdInit();

    // Loop forever.
    while(1)
    {
        {
        	/*
           	unsigned char id[10];
           	unsigned int i;

           	SPIFLASH_Sector_Erase(0);
           	SPIFLASH_Bulk_Erase();
           	SPIFLASH_Page_Program(tmp % 10, &tmp, 1);

           	SPIFLASH_Read_Data_Bytes(0, id, sizeof(id));
           	UARTprintf("READ: ");
           	for(i = 0; i < sizeof(id); i++)
           		UARTprintf("0x%X ", id[i]);

           	tmp++;
           	UARTprintf("\n");
           	*/
        }

        {
        	//i++;
        	//char buff;
        	//int cnt = USBSerialRead(&buff, 1);
        	//printf("count: %d\n\r", i);
        	//printf("%d\n", cnt);
        	//printf("printf test\n\r");
        }

        // Wait for the required amount of time.
        vTaskDelay(1000 * portTICK_RATE_MS);
    }
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
unsigned long TestTaskInit(void)
{
    // Create the LED task.
    if(xTaskCreate(TestTask, (signed portCHAR *)"Test", TESTTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_LED_TASK, NULL) != pdTRUE)
        return(1);

    // Success.
    return(0);
}
