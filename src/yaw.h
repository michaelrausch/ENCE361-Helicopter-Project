#ifndef YAW_H_
#define YAW_H_

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
 * Initialize yaw peripherals and interrupts
 */
void
initYaw();

/*
 * Return the yaw value in degrees
 */
double 
getYawVal();

/*
 * Set the current yaw to zero.
 * This is called when the
 * reference position is found
 */
void 
resetYawVal();

/*
 * Return whether or not the reference point
 * has been detected
 */
bool 
getHasRef();

/*
 * Return the reference yaw value
 */
int32_t 
getRefYawVal();

/**
 * Get the yaw value in the range 0-360
 */
int32_t 
getDisplayYawVal();

#endif /* YAW_H_ */
