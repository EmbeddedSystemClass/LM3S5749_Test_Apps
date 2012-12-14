//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
//
// Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the EK-LM4F120XL Firmware Package.
//
//*****************************************************************************

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
static void
TestTask(void *pvParameters)
{
    portTickType ulWakeTime;
    unsigned long ulLEDToggleDelay;

    //
    // Initialize the LED Toggle Delay to default value.
    //
    ulLEDToggleDelay = LED_TOGGLE_DELAY;

    //
    // Get the current tick count.
    //
    ulWakeTime = xTaskGetTickCount();

    //
    // Loop forever.
    //
    while(1)
    {

        UARTprintf("Test.\n");

        //
        // Wait for the required amount of time.
        //
        vTaskDelayUntil(&ulWakeTime, ulLEDToggleDelay / portTICK_RATE_MS);
    }
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
unsigned long
TestTaskInit(void)
{
    //
    // Print the current loggling LED and frequency.
    //
    UARTprintf("\nTestTaskInit\n");

    //
    // Create the LED task.
    //
    if(xTaskCreate(TestTask, (signed portCHAR *)"Test", TESTTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_LED_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
