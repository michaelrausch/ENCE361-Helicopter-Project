#include "motors.h"

void
initMainRotorPwm()
{
    SysCtlPeripheralEnable(MAIN_ROTOR_PWM_PERIPHERAL);
    SysCtlPeripheralEnable(MAIN_ROTOR_GPIO_PERIPHERAL);

    GPIOPinConfigure(MAIN_ROTOR_GPIO_CONFIG);
    GPIOPinTypePWM(MAIN_ROTOR_GPIO_BASE, MAIN_ROTOR_GPIO_PIN);

    PWMGenConfigure(MAIN_ROTOR_PWM_BASE, MAIN_ROTOR_PWM_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    setMainRotorPercent(MOTOR_DEFAULT_PCT);
    PWMGenEnable(MAIN_ROTOR_PWM_BASE, MAIN_ROTOR_PWM_GEN);
    PWMOutputState(MAIN_ROTOR_PWM_BASE, MAIN_ROTOR_PWM_OUTBIT, false);
}

void
initTailRotorPwm()
{
    SysCtlPeripheralEnable(TAIL_ROTOR_PWM_PERIPHERAL);
    SysCtlPeripheralEnable(TAIL_ROTOR_GPIO_PERIPHERAL);

    GPIOPinConfigure(TAIL_ROTOR_GPIO_CONFIG);
    GPIOPinTypePWM(TAIL_ROTOR_GPIO_BASE, TAIL_ROTOR_GPIO_PIN);

    PWMGenConfigure(TAIL_ROTOR_PWM_BASE, TAIL_ROTOR_PWM_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenEnable(TAIL_ROTOR_PWM_BASE, TAIL_ROTOR_PWM_GEN);
    PWMOutputState(TAIL_ROTOR_PWM_BASE, TAIL_ROTOR_PWM_OUTBIT, false);
}

void 
initMotors()
{
    SysCtlPWMClockSet(SYSCTL_PWMDIV_4);
    initTailRotorPwm();
    initMainRotorPwm();
}

/*
 * Set the duty cycle for the main motor
 */
void 
setMainRotorPercent(double dutyCycle)
{
    if (dutyCycle > 80.0){
        dutyCycle = 80.0;
    }

    double period = SysCtlClockGet() / PWM_DIVIDER / PWM_FREQ_HZ;

    PWMGenPeriodSet(MAIN_ROTOR_PWM_BASE, MAIN_ROTOR_PWM_GEN, period);
    uint32_t asd = (uint32_t) (period * dutyCycle / 100.0);
    PWMPulseWidthSet(MAIN_ROTOR_PWM_BASE, MAIN_ROTOR_PWM_OUTNUM, asd);
}

/*
 * Set the duty cycle for the tail motor
 */
void 
setTailRotorPercent(double dutyCycle)
{

    double period = SysCtlClockGet() / PWM_DIVIDER / PWM_FREQ_HZ;

    PWMGenPeriodSet(TAIL_ROTOR_PWM_BASE, TAIL_ROTOR_PWM_GEN, period);
    uint32_t asd = (uint32_t) (period * dutyCycle / 100.0);

    PWMPulseWidthSet(TAIL_ROTOR_PWM_BASE, TAIL_ROTOR_PWM_OUTNUM, asd);
}

/*
 * Enable PWM for the tail rotor
 */
void
enableTailRotor()
{
    PWMOutputState(TAIL_ROTOR_PWM_BASE, TAIL_ROTOR_PWM_OUTBIT, true);
}

/*
 * Disable PWM for the tail rotor
 */
void
disableTailRotor()
{
    PWMOutputState(TAIL_ROTOR_PWM_BASE, TAIL_ROTOR_PWM_OUTBIT, false);
}

/*
 * Enable PWM for the main rotor
 */
void
enableMainRotor()
{
    PWMOutputState(MAIN_ROTOR_PWM_BASE, MAIN_ROTOR_PWM_OUTBIT, true);
}

/*
 * Disable PWM for the main rotor
 */
void
disableMainRotor()
{
    PWMOutputState(MAIN_ROTOR_PWM_BASE, MAIN_ROTOR_PWM_OUTBIT, false);
}

/*
 * Enable PWM for both motors and set the duty cycle to the default value
 */
void
startMotors()
{
    enableMainRotor();
    enableTailRotor();

    setTailRotorPercent(MOTOR_DEFAULT_PCT);
    setMainRotorPercent(MOTOR_DEFAULT_PCT);
}

/*
 * Disable PWM for both rotors
 */
void
stopMotors()
{
    disableMainRotor();
    disableTailRotor();
}
