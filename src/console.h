#ifndef CONSOLE_H_
#define CONSOLE_H_

#define BAUD_RATE 9600
#define UART_CLK_FREQ 16000000  // The UART clock freq. See p.12 of TI migration guide, spma050a

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/adc.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "stdio.h"
#include <string.h>
#include "buttons2.h"
#include "circBufT.h"
#include "OrbitOLEDInterface.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "limits.h"

/*
 * Initialize the console
 */
void initConsole();

#endif /* CONSOLE_H_ */
