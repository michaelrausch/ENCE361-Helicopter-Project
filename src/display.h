#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "OrbitOLEDInterface.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include <stdint.h>
#include <stdbool.h>
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include <string.h>
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"

/*
* Display duty cycles and frequencies on oled display
* @param Altitude - The current altitude
* @param Yaw - The current yaw
* @param trDutyCycle - The tail rotor duty cycle
* @param mrDutyCycle - The main rotor duty cycle
*/
void
display(uint32_t altitude, int32_t yaw, uint32_t trDutyCycle, uint32_t mrDutyCycle);

#endif /* DISPLAY_H_ */
