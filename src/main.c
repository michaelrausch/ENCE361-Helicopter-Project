#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "stdio.h"
#include <string.h>
#include "buttons2.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "motors.h"
#include "display.h"
#include "altitude.h"
#include "pid.h"
#include "yaw.h"
#include "console.h"
#include "buttons2.h"

#define SYSTICK_RATE_HZ 100
#define TAIL_ROTOR_SEARCH_DUTY_CYCLE 23

#define DEBUG

typedef enum heli_status_s{
    FLYING,
    LANDING,
    LANDED,
    STARTUP
} heli_status_t;

typedef struct {
    heli_status_t status;
    bool inProgress; // A state transition is in progress

    int32_t desiredAltitude;
    int32_t desiredYaw;
    bool hasYawRef; // Yaw reference has been obtained
} heli_state_t;

static pid_state_t mainRotorPid;
static pid_state_t tailRotorPid;
static heli_state_t heliState;

static uint32_t sysTicks = 0;
static uint32_t lastpidUpdate = 0;
static bool hasLanded = false; // Mode switch has been switched to LAND since the program has started

void 
initClock()
{
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
            SYSCTL_XTAL_16MHZ);
}

void 
SysTickIntHandler ()
{
    updateButtons();
    sysTicks++;
}

void 
initSysTick()
{
    SysTickPeriodSet(SysCtlClockGet() / SYSTICK_RATE_HZ);
    SysTickIntRegister(SysTickIntHandler);
    SysTickIntEnable();
    SysTickEnable();
}

void 
resetInrpt()
{
    GPIOIntClear (GPIO_PORTA_BASE, GPIO_PIN_6);
    if (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6) == GPIO_PIN_6){
        SysCtlReset();
    }
}

/*
 * Initialize the state of the tail rotor pid
 */
void 
initTailRotorPid()
{
    tailRotorPid.setPoint = 0;
    tailRotorPid.kp = 0.40;
    tailRotorPid.ki = 0.0008;
    tailRotorPid.kd = 0.15;
    tailRotorPid.proportionalError = 0.0;
    tailRotorPid.integratedError = 0.0;
    tailRotorPid.derivativeError = 0.0;
    tailRotorPid.previousError = 0.0;
    tailRotorPid.dutyCycle = 0;
}

/*
 * Initialize the state of the main rotor pid
 */
void 
initMainRotorPid(){
    mainRotorPid.setPoint = 0;
    mainRotorPid.kp = 0.15;
    mainRotorPid.ki = 0.001;
    mainRotorPid.kd = 0.00001;
    mainRotorPid.proportionalError = 0.0;
    mainRotorPid.integratedError = 0.0;
    mainRotorPid.derivativeError = 0.0;
    mainRotorPid.previousError = 0.0;
    mainRotorPid.dutyCycle = 0;
}

/*
* Init everything except the clock
*/
void 
init()
{
    heliState.status = LANDED;
    heliState.inProgress = false;
    heliState.desiredAltitude = 0;
    heliState.desiredYaw = 0;
    heliState.hasYawRef = false;

    initMainRotorPid();
    initTailRotorPid();

    initButtons();
    initAltitude();
    initConsole();
    initMotors();
    initYaw();
    initSysTick();
    OLEDInitialise();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA,
           GPIO_PIN_TYPE_STD_WPD);

    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOIntRegister (GPIO_PORTA_BASE, resetInrpt);
    GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA,
               GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet (GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_FALLING_EDGE);
        GPIOIntEnable (GPIO_PORTA_BASE, GPIO_PIN_6);
        IntEnable (INT_GPIOA);

    SysCtlDelay(30);
}

/*
 * Reset peripherals
 */
void 
resetPeripherals()
{
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralReset(TAIL_ROTOR_PWM_PERIPHERAL);
    SysCtlPeripheralReset(TAIL_ROTOR_GPIO_PERIPHERAL);
    SysCtlPeripheralReset(MAIN_ROTOR_PWM_PERIPHERAL);
    SysCtlPeripheralReset(MAIN_ROTOR_GPIO_PERIPHERAL);
    SysCtlPeripheralReset(UP_BUT_PERIPH);
    SysCtlPeripheralReset(DOWN_BUT_PERIPH);
}

/*
 * Check the mode switch, and update the helicopters
 * status if it isn't already in progress
 */
void
checkModeSwitch()
{
    if (GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7) == GPIO_PIN_7){
        if (!heliState.inProgress && heliState.status != STARTUP && heliState.status != FLYING && hasLanded){
            heliState.status = STARTUP;
        }
    }
    else{
        hasLanded = true;
        if (heliState.status != LANDING && heliState.status != LANDED && !heliState.inProgress){
            heliState.status = LANDING;

        }
    }
}

/*
 * Check buttons and mode switch
 */
void 
buttonsTask()
{
    checkModeSwitch();

    if ((checkButton(UP) == PUSHED)){
        if (heliState.desiredAltitude <= 90){
            heliState.desiredAltitude += 10;
        }
    }

    if ((checkButton(DOWN) == PUSHED)){
        if (heliState.desiredAltitude >= 10){
            heliState.desiredAltitude -= 10;
        }
    }

    if (checkButton(LEFT) == PUSHED){
        heliState.desiredYaw -= 15;
    }

    if (checkButton(RIGHT) == PUSHED){
        heliState.desiredYaw += 15;
    }
}

/*
 * Update the helicopters state
 */
void 
heliStateTask()
{
    // Startup selected and not in progress
    if (heliState.status == STARTUP && !heliState.inProgress){
        startMotors();
        heliState.inProgress = true;
    }

    // Heli is searching for yaw reference
    if (heliState.status == STARTUP && heliState.inProgress){
        if (heliState.hasYawRef != true){
            if (getHasRef()){
                heliState.hasYawRef = true;
                heliState.desiredYaw = 0;
            }
        }
    }

    // Heli has yaw ref, change to flying
    if (heliState.status == STARTUP && heliState.inProgress && heliState.hasYawRef){
        heliState.inProgress = false;
        heliState.status = FLYING;
    }

    // Heli needs to land, but isn't currently doing so
    if (heliState.status == LANDING && !heliState.inProgress){
        heliState.desiredAltitude = 0;
        heliState.desiredYaw = 0;
        heliState.inProgress = true;
    }

    // Heli is landing, check to see if landed
    if (heliState.status == LANDING && heliState.inProgress){
        if (getAltitudePercentage() < 5){
            stopMotors();
            heliState.status = LANDED;
            heliState.inProgress = false;
        }
    }
}

/*
 * Update the duty cycle for the tail rotor
 */
void
tailRotorTask()
{
    tailRotorPid.setPoint = heliState.desiredYaw;

    tailRotorPid = pidUpdate(tailRotorPid, getYawVal(), (sysTicks-lastpidUpdate));

    setTailRotorPercent((double) tailRotorPid.dutyCycle);
}

/*
 * Update the duty cycle for the main rotor
 */
void
mainRotorTask()
{
    mainRotorPid.setPoint = heliState.desiredAltitude;

    mainRotorPid = pidUpdate(mainRotorPid, getAltitudePercentage(), (sysTicks-lastpidUpdate));

    setMainRotorPercent((double) mainRotorPid.dutyCycle);
}

/*
 * Update the duty cycle for each motor
 */
void
motorTask()
{
    if (heliState.status == STARTUP){
        tailRotorPid.dutyCycle = TAIL_ROTOR_SEARCH_DUTY_CYCLE;
        setTailRotorPercent((double) tailRotorPid.dutyCycle);
        return;
    }
    else{
        tailRotorTask();
    }

    if (heliState.status != STARTUP){
        mainRotorTask();
    }

    lastpidUpdate = sysTicks;
}

/*
 * Display the helicopters current state on the OLED display
 */
void 
oledDisplay()
{
    display(getAltitudePercentage(), getYawVal(), tailRotorPid.dutyCycle, mainRotorPid.dutyCycle);
}

/*
 * Display the current helicopter status via the UART
 * Only runs if DEBUG is defined
 */
void 
uartDisplay()
{
#ifdef DEBUG
    UARTprintf("Set Altitude: %d\n", heliState.desiredAltitude);
    UARTprintf("Altitude: %d\n", getAltitudePercentage());
    UARTprintf("Set Yaw: %d\n", heliState.desiredYaw % 360);
    UARTprintf("Yaw: %d\n", getDisplayYawVal());
    UARTprintf("Tail Rotor: %d\n", tailRotorPid.dutyCycle);
    UARTprintf("Main Rotor: %d\n", mainRotorPid.dutyCycle);

    if (heliState.status == LANDED){
        UARTprintf("Landed\n");
    }
    else{
        UARTprintf("Flying\n");
    }
    UARTprintf("=========\n");
#endif
}

/*
 * Returns true when sysTicks % 100 / timesPerSecond equals the offset
 * because 100 sysTicks is 1 second  100 / timesPerSecond gives us a range to schedule
 * tasks on between 0 and (100 / timesPersoncond) - 1 the offset is the value in this
 * range to run the task on to prevent task scheduling clashes.
 */
bool 
rrScheduler(uint32_t timesPerSecond, uint32_t offset)
{
    if(sysTicks % (100 / timesPerSecond) == offset){
        return true;
    }
    return false;
}

int main(){
    resetPeripherals();
    initClock();
    init();

    IntMasterEnable();

    while(1){

        if(rrScheduler(50, 1)){
            buttonsTask();
        }

        if(rrScheduler(20, 3)){
            altitudeUpdate();
        }

        if(rrScheduler(2, 20)){
            heliStateTask();
        }

        if (rrScheduler(4, 0) && heliState.status != LANDED){
            motorTask();
        }

        if(rrScheduler(4, 15)){
            oledDisplay();
            uartDisplay();
        }
    }
}
