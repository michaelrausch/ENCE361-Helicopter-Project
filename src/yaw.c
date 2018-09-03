#include "yaw.h"

static uint32_t yawQuadState = 0;
static int32_t yawVal = 0;
static int32_t refYawVal = 0;
static bool bVal = 0;
static bool aVal = 0;
static bool hasRef = false;

/*
 * Yaw reference interrupt handler. Sets hasRef to true
 */
void 
yawRefIntHandler()
{
    GPIOIntClear (GPIO_PORTC_BASE, GPIO_PIN_4);
    if (hasRef != 1){
        if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4)){
            hasRef = true;
            refYawVal = yawVal;
            resetYawVal();
        }
    }
}

void 
yawPinChangeIntHandler()
{

    GPIOIntClear (GPIO_PORTB_BASE, GPIO_PIN_0);
    GPIOIntClear (GPIO_PORTB_BASE, GPIO_PIN_1);

    bVal = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);
    aVal = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);

    if(yawQuadState == 0){
        if (bVal == 0 && aVal == 1){
            yawQuadState = 1;
            yawVal--;
        } else if (bVal == 1 && aVal == 0){
            yawQuadState = 3;
            yawVal++;
        }
    }
    else if(yawQuadState == 1){
        if (bVal == 1 && aVal == 1) {
            yawQuadState = 2;
            yawVal--;
        } else if (bVal == 0 && aVal == 0) {
            yawQuadState = 0;
            yawVal++;
        }
    }
    else if(yawQuadState == 2){
        if (bVal == 1 && aVal == 0){
            yawQuadState = 3;
            yawVal--;
        } else if (bVal == 0 && aVal == 1){
            yawQuadState = 1;
            yawVal++;
        }
    }
    else if(yawQuadState == 3){
        if (bVal == 0 && aVal == 0){
            yawQuadState = 0;
            yawVal--;
        } else if (bVal == 1 && aVal == 1){
            yawQuadState = 2;
            yawVal++;
        }
    }
}

/*
 * Return the yaw value in degrees
 */
double 
getYawVal()
{
    return yawVal * 0.8035f;
}

/**
 * Get the yaw value in the range 0-360
 */
int32_t 
getDisplayYawVal()
{
    return ((int32_t) getYawVal() % 360);
};

/**
 * Return whether or not the reference point
 * has been detected
 */
bool 
getHasRef()
{
    return hasRef;
}

/*
 * Return the reference yaw value
 */
int32_t 
getRefYawVal()
{
    return refYawVal;
}

/**
 * Initialize yaw peripherals and interrupts
 */
void 
initYaw(void)
{
    /*
     * Initialize yaw quadrature decoding
     */
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);
    GPIOIntRegister (GPIO_PORTB_BASE, yawPinChangeIntHandler);

    GPIOPinTypeGPIOInput (GPIO_PORTB_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput (GPIO_PORTB_BASE, GPIO_PIN_1);
    GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet (GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOIntTypeSet (GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_BOTH_EDGES);
    GPIOIntTypeSet (GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_BOTH_EDGES);
    GPIOIntEnable (GPIO_PORTB_BASE, GPIO_PIN_0);
    GPIOIntEnable (GPIO_PORTB_BASE, GPIO_PIN_1);
    IntEnable (INT_GPIOB);

    /*
     * Initialize yaw reference detection
     */
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOC);
    GPIOIntRegister (GPIO_PORTC_BASE, yawRefIntHandler);
    GPIOPinTypeGPIOInput (GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPadConfigSet (GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet (GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
    GPIOIntEnable (GPIO_PORTC_BASE, GPIO_PIN_4);
    IntEnable (INT_GPIOC);
}

/*
 * Reset the current yaw to zero
 */
void 
resetYawVal()
{
    yawVal = 0;
}
