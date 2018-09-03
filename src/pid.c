#include "pid.h"

/**
 * Update the pid values for the specified motor
 * @param pidState     - The pidState for the motor
 * @param measurement  - The yaw or altitude measurement
 * @param timeDiff     - Time elapsed since the last update
 */
pid_state_t
pidUpdate(pid_state_t pidState, int32_t measurement, uint32_t timeDiff)
{
    double error = (double) pidState.setPoint - (double)measurement;
    double feedback;

    pidState.proportionalError = pidState.kp * error;
    pidState.integratedError += error * pidState.ki * (double)timeDiff;
    pidState.derivativeError = pidState.kd * (error - pidState.previousError) ;

    /**
     * Prevent the integrated error from becoming too large
     */
    if (pidState.integratedError > 100.0){
        pidState.integratedError = 100.0;
    }

    if (pidState.integratedError < -100.0){
        pidState.integratedError = -100.0;
    }

    feedback = pidState.proportionalError + pidState.derivativeError + pidState.integratedError;

    pidState.dutyCycle = (int32_t) round(feedback);

    /*
     * Check if duty cycle is within allowed values
     */
    if (pidState.dutyCycle < MIN_DUTY_CYCLE){
        pidState.dutyCycle = MIN_DUTY_CYCLE;
    }

    if (pidState.dutyCycle > MAX_DUTY_CYCLE){
        pidState.dutyCycle = MAX_DUTY_CYCLE;
    }

    pidState.previousError = error;
    return pidState;
}

