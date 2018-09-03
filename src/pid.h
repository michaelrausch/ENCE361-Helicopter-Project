#ifndef PID_H_
#define PID_H_

#include <stdbool.h>
#include <stdint.h>
#include "math.h"

#define MAX_DUTY_CYCLE 60
#define MIN_DUTY_CYCLE 5

typedef struct {
    int32_t setPoint; // The desired measurement

    double kp; // Proportional gain
    double ki; // Integral gain
    double kd; // Derivative gain

    double proportionalError; // Current proportional error
    double integratedError; // Current integral error
    double derivativeError; // Current derivative error

    double previousError; // Previous error between the setpoint and the measured value

    int32_t dutyCycle; // Current duty cycle of the motor
} pid_state_t;

/**
 * Update the pid values for the specified motor
 * @param pidState     - The pidState for the motor
 * @param measurement  - The yaw or altitude measurement
 * @param timeDiff     - Time elapsed since the last update
 */
pid_state_t
pidUpdate(pid_state_t pidState, int32_t measurement, uint32_t timeDiff);

#endif /* PID_H_ */
