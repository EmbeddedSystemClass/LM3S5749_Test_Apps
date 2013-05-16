/*
 * timer.c
 *
 *  Created on: May 11, 2013
 *      Author: root
 */
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "FreeRTOS.h"
#include "timer.h"

/* The set frequency of the interrupt.  Deviations from this are measured as
the jitter. */
#define timerINTERRUPT_FREQUENCY		( 20000UL )
/* The highest available interrupt priority. */
#define timerHIGHEST_PRIORITY			( 0 )
/* Misc defines. */
#define timerMAX_32BIT_VALUE			( 0xffffffffUL )

/* Counts the total number of times that the high frequency timer has 'ticked'.
This value is used by the run time stats function to work out what percentage
of CPU time each task is taking. */
volatile unsigned portLONG ulHighFrequencyTimerTicks = 0UL;

void Timer0IntHandler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	/* Keep a count of the total number of 20KHz ticks.  This is used by the
	run time stats functionality to calculate how much CPU time is used by
	each task. */
	ulHighFrequencyTimerTicks++;
}

void vSetupHighFrequencyTimer(void)
{
	unsigned long ulFrequency;

	/* Timer zero is used to generate the interrupts */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);

	/* Set the timer interrupt to be above the kernel - highest. */
	IntPrioritySet(INT_TIMER0A, timerHIGHEST_PRIORITY);

	/* Ensure interrupts do not start until the scheduler is running. */
	portDISABLE_INTERRUPTS();

	/* The rate at which the timer will interrupt. */
	ulFrequency = configCPU_CLOCK_HZ / timerINTERRUPT_FREQUENCY;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ulFrequency);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	/* Enable both timers. */
    TimerEnable(TIMER0_BASE, TIMER_A);
}
