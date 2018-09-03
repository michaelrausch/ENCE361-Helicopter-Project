#include "display.h"

/*
* Display duty cycles and frequencies on oled display
* @param Altitude - The current altitude
* @param Yaw - The current yaw
* @param trDutyCycle - The tail rotor duty cycle
* @param mrDutyCycle - The main rotor duty cycle
*/
void 
display(uint32_t altitude, int32_t yaw, uint32_t trDutyCycle, uint32_t mrDutyCycle)
{
    char displayContent[30];

    OLEDStringDraw("Altitude: ", 0 ,0);
    sprintf(displayContent,"%d  ", altitude);
    OLEDStringDraw(displayContent, 11 ,0);

    OLEDStringDraw("Yaw: ", 0 ,1);
    sprintf(displayContent,"%d ", yaw);
    OLEDStringDraw(displayContent, 6 ,1);

    OLEDStringDraw("MR Duty: ", 0 ,2);
    sprintf(displayContent,"%d", mrDutyCycle);
    OLEDStringDraw(displayContent, 10 ,2);

    OLEDStringDraw("TR Duty: ", 0 ,3);
    sprintf(displayContent,"%d", trDutyCycle);
    OLEDStringDraw(displayContent, 10 ,3);
}
