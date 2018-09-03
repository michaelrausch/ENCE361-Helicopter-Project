/**
 * Module for controlling the helicopters motors
 *
 * Michael Rausch - mra106
 * Tyler Holland - tho63
 */

#ifndef MOTORS_H_
#define MOTORS_H_

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
* PWM Constants
*/
#define PWM_FREQ_HZ                     150
#define PWM_DIVIDER                     4
#define PWM_DUTY_CYCLE_MAX              95
#define PWM_DUTY_CYCLE_MIN              5

/*
* Main Rotor Constants
*/
#define MAIN_ROTOR_PWM_BASE             PWM0_BASE
#define MAIN_ROTOR_PWM_GEN              PWM_GEN_3
#define MAIN_ROTOR_PWM_OUTNUM           PWM_OUT_7
#define MAIN_ROTOR_PWM_OUTBIT           PWM_OUT_7_BIT
#define MAIN_ROTOR_PWM_PERIPHERAL       SYSCTL_PERIPH_PWM0
#define MAIN_ROTOR_GPIO_PERIPHERAL      SYSCTL_PERIPH_GPIOC
#define MAIN_ROTOR_GPIO_BASE            GPIO_PORTC_BASE
#define MAIN_ROTOR_GPIO_CONFIG          GPIO_PC5_M0PWM7
#define MAIN_ROTOR_GPIO_PIN             GPIO_PIN_5

/*
* Tail Rotor Constants
*/
#define TAIL_ROTOR_PWM_BASE             PWM1_BASE
#define TAIL_ROTOR_PWM_GEN              PWM_GEN_2
#define TAIL_ROTOR_PWM_OUTNUM           PWM_OUT_5
#define TAIL_ROTOR_PWM_OUTBIT           PWM_OUT_5_BIT

#define TAIL_ROTOR_PWM_PERIPHERAL       SYSCTL_PERIPH_PWM1
#define TAIL_ROTOR_GPIO_PERIPHERAL      SYSCTL_PERIPH_GPIOF
#define TAIL_ROTOR_GPIO_BASE            GPIO_PORTF_BASE
#define TAIL_ROTOR_GPIO_CONFIG          GPIO_PF1_M1PWM5
#define TAIL_ROTOR_GPIO_PIN             GPIO_PIN_1

#define MOTOR_DEFAULT_PCT 10.0

/*
 * Enable PWM for both motors and set the duty cycle to the default value
 */
void
startMotors();

/*
 * Disable PWM for both rotors
 */
void
stopMotors();

/*
 * Init the pwm peripherals for the both motors
 */
void
initMotors();

/*
 * Set the duty cycle for the tail rotor
 */
void
setTailRotorPercent(double);

/*
 * Set the duty cycle for the main rotor
 */
void
setMainRotorPercent(double);

#endif /* MOTORS_H_ */
