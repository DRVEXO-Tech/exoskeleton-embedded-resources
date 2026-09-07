/**
 * @file DRVEXOAhrs.c
 * @author Seb Madgwick
 * @brief Attitude and Heading Reference System (AHRS) algorithm.
 */

//------------------------------------------------------------------------------
// Includes

#include <float.h>
#include "DRVEXOAhrs.h"
#include <math.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Initial gain used during the initialisation.
 */
#define INITIAL_GAIN (10.0f)

/**
 * @brief Initialisation period in seconds.
 */
#define INITIALISATION_PERIOD (3.0f)

//------------------------------------------------------------------------------
// Function declarations

static inline DRVEXOVector HalfGravity(const DRVEXOAhrs *const ahrs);

static inline DRVEXOVector HalfMagnetic(const DRVEXOAhrs *const ahrs);

static inline DRVEXOVector Feedback(const DRVEXOVector sensor, const DRVEXOVector reference);

static inline int Clamp(const int value, const int min, const int max);

//------------------------------------------------------------------------------
// Variables

const DRVEXOAhrsSettings DRVEXOAhrsDefaultSettings = {
    .convention = DRVEXOConventionNwu,
    .gain = 0.5f,
    .gyroscopeRange = 0.0f,
    .accelerationRejection = 90.0f,
    .magneticRejection = 90.0f,
    .recoveryTriggerPeriod = 0,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the AHRS structure.
 * @param ahrs AHRS structure.
 */
void DRVEXOAhrsInitialise(DRVEXOAhrs *const ahrs) {
    DRVEXOAhrsSetSettings(ahrs, &DRVEXOAhrsDefaultSettings);
    DRVEXOAhrsReset(ahrs);
}

/**
 * @brief Resets the AHRS algorithm.
 * @param ahrs AHRS structure.
 */
void DRVEXOAhrsReset(DRVEXOAhrs *const ahrs) {
    ahrs->quaternion = DRVEXO_QUATERNION_IDENTITY;
    ahrs->accelerometer = DRVEXO_VECTOR_ZERO;
    ahrs->initialising = true;
    ahrs->rampedGain = INITIAL_GAIN;
    ahrs->angularRateRecovery = false;
    ahrs->halfAccelerometerFeedback = DRVEXO_VECTOR_ZERO;
    ahrs->halfMagnetometerFeedback = DRVEXO_VECTOR_ZERO;
    ahrs->accelerometerIgnored = false;
    ahrs->accelerationRecoveryTrigger = 0;
    ahrs->accelerationRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
    ahrs->magnetometerIgnored = false;
    ahrs->magneticRecoveryTrigger = 0;
    ahrs->magneticRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
}

/**
 * @brief Sets the settings.
 * @param ahrs AHRS structure.
 * @param settings Settings.
 */
void DRVEXOAhrsSetSettings(DRVEXOAhrs *const ahrs, const DRVEXOAhrsSettings *const settings) {
    ahrs->settings.convention = settings->convention;
    ahrs->settings.gain = settings->gain;
    ahrs->settings.gyroscopeRange = settings->gyroscopeRange == 0.0f ? FLT_MAX : 0.98f * settings->gyroscopeRange;
    ahrs->settings.accelerationRejection = settings->accelerationRejection == 0.0f ? FLT_MAX : powf(0.5f * sinf(DRVEXODegreesToRadians(settings->accelerationRejection)), 2);
    ahrs->settings.magneticRejection = settings->magneticRejection == 0.0f ? FLT_MAX : powf(0.5f * sinf(DRVEXODegreesToRadians(settings->magneticRejection)), 2);
    ahrs->settings.recoveryTriggerPeriod = settings->recoveryTriggerPeriod;
    ahrs->accelerationRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
    ahrs->magneticRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
    if ((settings->gain == 0.0f) || (settings->recoveryTriggerPeriod == 0)) {
        ahrs->settings.accelerationRejection = FLT_MAX; // disable acceleration and magnetic rejection features if gain is zero
        ahrs->settings.magneticRejection = FLT_MAX;
    }
    if (ahrs->initialising == false) {
        ahrs->rampedGain = ahrs->settings.gain;
    }
    ahrs->rampedGainStep = (INITIAL_GAIN - ahrs->settings.gain) / INITIALISATION_PERIOD;
}

/**
 * @brief Updates the AHRS algorithm using the gyroscope, accelerometer, and
 * magnetometer.
 * @param ahrs AHRS structure.
 * @param gyroscope Gyroscope in degrees per second.
 * @param accelerometer Accelerometer in g.
 * @param magnetometer Magnetometer in any calibrated units.
 * @param deltaTime Delta time in seconds.
 */
void DRVEXOAhrsUpdate(DRVEXOAhrs *const ahrs, const DRVEXOVector gyroscope, const DRVEXOVector accelerometer, const DRVEXOVector magnetometer, const float deltaTime) {
    ahrs->accelerometer = accelerometer;

    // Reinitialise if gyroscope range exceeded
    if ((fabsf(gyroscope.axis.x) > ahrs->settings.gyroscopeRange) || (fabsf(gyroscope.axis.y) > ahrs->settings.gyroscopeRange) || (fabsf(gyroscope.axis.z) > ahrs->settings.gyroscopeRange)) {
        const DRVEXOQuaternion quaternion = ahrs->quaternion;
        DRVEXOAhrsReset(ahrs);
        ahrs->quaternion = quaternion;
        ahrs->angularRateRecovery = true;
    }

    // Ramp down gain during initialisation
    if (ahrs->initialising) {
        ahrs->rampedGain -= ahrs->rampedGainStep * deltaTime;
        if ((ahrs->rampedGain < ahrs->settings.gain) || (ahrs->settings.gain == 0.0f)) {
            ahrs->rampedGain = ahrs->settings.gain;
            ahrs->initialising = false;
            ahrs->angularRateRecovery = false;
        }
    }

    // Calculate direction of gravity indicated by algorithm
    const DRVEXOVector halfGravity = HalfGravity(ahrs);

    // Calculate accelerometer feedback
    DRVEXOVector halfAccelerometerFeedback = DRVEXO_VECTOR_ZERO;
    ahrs->accelerometerIgnored = true;
    if (DRVEXOVectorIsZero(accelerometer) == false) {
        // Calculate accelerometer feedback scaled by 0.5
        ahrs->halfAccelerometerFeedback = Feedback(DRVEXOVectorNormalise(accelerometer), halfGravity);

        // Don't ignore accelerometer if acceleration error below threshold
        if (ahrs->initialising || (DRVEXOVectorNormSquared(ahrs->halfAccelerometerFeedback) <= ahrs->settings.accelerationRejection)) {
            ahrs->accelerometerIgnored = false;
            ahrs->accelerationRecoveryTrigger -= 9;
        } else {
            ahrs->accelerationRecoveryTrigger += 1;
        }

        // Don't ignore accelerometer during acceleration recovery
        if (ahrs->accelerationRecoveryTrigger > ahrs->accelerationRecoveryTimeout) {
            ahrs->accelerationRecoveryTimeout = 0;
            ahrs->accelerometerIgnored = false;
        } else {
            ahrs->accelerationRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
        }
        ahrs->accelerationRecoveryTrigger = Clamp(ahrs->accelerationRecoveryTrigger, 0, ahrs->settings.recoveryTriggerPeriod);

        // Apply accelerometer feedback
        if (ahrs->accelerometerIgnored == false) {
            halfAccelerometerFeedback = ahrs->halfAccelerometerFeedback;
        }
    }

    // Calculate magnetometer feedback
    DRVEXOVector halfMagnetometerFeedback = DRVEXO_VECTOR_ZERO;
    ahrs->magnetometerIgnored = true;
    if (DRVEXOVectorIsZero(magnetometer) == false) {
        // Calculate direction of magnetic field indicated by algorithm
        const DRVEXOVector halfMagnetic = HalfMagnetic(ahrs);

        // Calculate magnetometer feedback scaled by 0.5
        ahrs->halfMagnetometerFeedback = Feedback(DRVEXOVectorNormalise(DRVEXOVectorCross(halfGravity, magnetometer)), halfMagnetic);

        // Don't ignore magnetometer if magnetic error below threshold
        if (ahrs->initialising || (DRVEXOVectorNormSquared(ahrs->halfMagnetometerFeedback) <= ahrs->settings.magneticRejection)) {
            ahrs->magnetometerIgnored = false;
            ahrs->magneticRecoveryTrigger -= 9;
        } else {
            ahrs->magneticRecoveryTrigger += 1;
        }

        // Don't ignore magnetometer during magnetic recovery
        if (ahrs->magneticRecoveryTrigger > ahrs->magneticRecoveryTimeout) {
            ahrs->magneticRecoveryTimeout = 0;
            ahrs->magnetometerIgnored = false;
        } else {
            ahrs->magneticRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
        }
        ahrs->magneticRecoveryTrigger = Clamp(ahrs->magneticRecoveryTrigger, 0, ahrs->settings.recoveryTriggerPeriod);

        // Apply magnetometer feedback
        if (ahrs->magnetometerIgnored == false) {
            halfMagnetometerFeedback = ahrs->halfMagnetometerFeedback;
        }
    }

    // Convert gyroscope to radians per second scaled by 0.5
    const DRVEXOVector halfGyroscope = DRVEXOVectorScale(gyroscope, DRVEXODegreesToRadians(0.5f));

    // Apply feedback to gyroscope
    const DRVEXOVector adjustedHalfGyroscope = DRVEXOVectorAdd(halfGyroscope, DRVEXOVectorScale(DRVEXOVectorAdd(halfAccelerometerFeedback, halfMagnetometerFeedback), ahrs->rampedGain));

    // Integrate rate of change of quaternion
    ahrs->quaternion = DRVEXOQuaternionAdd(ahrs->quaternion, DRVEXOQuaternionVectorProduct(ahrs->quaternion, DRVEXOVectorScale(adjustedHalfGyroscope, deltaTime)));

    // Normalise quaternion
    ahrs->quaternion = DRVEXOQuaternionNormalise(ahrs->quaternion);
}

/**
 * @brief Returns the direction of gravity scaled by 0.5.
 * @param ahrs AHRS structure.
 * @return Direction of gravity scaled by 0.5.
 */
static inline DRVEXOVector HalfGravity(const DRVEXOAhrs *const ahrs) {
#define Q ahrs->quaternion.element
    switch (ahrs->settings.convention) {
        case DRVEXOConventionNwu:
        case DRVEXOConventionEnu: {
            const DRVEXOVector halfGravity = {
                .axis = {
                    .x = Q.x * Q.z - Q.w * Q.y,
                    .y = Q.y * Q.z + Q.w * Q.x,
                    .z = Q.w * Q.w - 0.5f + Q.z * Q.z,
                }
            }; // third column of transposed rotation matrix scaled by 0.5
            return halfGravity;
        }
        case DRVEXOConventionNed: {
            const DRVEXOVector halfGravity = {
                .axis = {
                    .x = Q.w * Q.y - Q.x * Q.z,
                    .y = -1.0f * (Q.y * Q.z + Q.w * Q.x),
                    .z = 0.5f - Q.w * Q.w - Q.z * Q.z,
                }
            }; // third column of transposed rotation matrix scaled by -0.5
            return halfGravity;
        }
    }
#undef Q
    return DRVEXO_VECTOR_ZERO; // avoid compiler warning
}

/**
 * @brief Returns the direction of the magnetic field scaled by 0.5.
 * @param ahrs AHRS structure.
 * @return Direction of the magnetic field scaled by 0.5.
 */
static inline DRVEXOVector HalfMagnetic(const DRVEXOAhrs *const ahrs) {
#define Q ahrs->quaternion.element
    switch (ahrs->settings.convention) {
        case DRVEXOConventionNwu: {
            const DRVEXOVector halfMagnetic = {
                .axis = {
                    .x = Q.x * Q.y + Q.w * Q.z,
                    .y = Q.w * Q.w - 0.5f + Q.y * Q.y,
                    .z = Q.y * Q.z - Q.w * Q.x,
                }
            }; // second column of transposed rotation matrix scaled by 0.5
            return halfMagnetic;
        }
        case DRVEXOConventionEnu: {
            const DRVEXOVector halfMagnetic = {
                .axis = {
                    .x = 0.5f - Q.w * Q.w - Q.x * Q.x,
                    .y = Q.w * Q.z - Q.x * Q.y,
                    .z = -1.0f * (Q.x * Q.z + Q.w * Q.y),
                }
            }; // first column of transposed rotation matrix scaled by -0.5
            return halfMagnetic;
        }
        case DRVEXOConventionNed: {
            const DRVEXOVector halfMagnetic = {
                .axis = {
                    .x = -1.0f * (Q.x * Q.y + Q.w * Q.z),
                    .y = 0.5f - Q.w * Q.w - Q.y * Q.y,
                    .z = Q.w * Q.x - Q.y * Q.z,
                }
            }; // second column of transposed rotation matrix scaled by -0.5
            return halfMagnetic;
        }
    }
#undef Q
    return DRVEXO_VECTOR_ZERO; // avoid compiler warning
}

/**
 * @brief Returns the feedback.
 * @param sensor Sensor.
 * @param reference Reference.
 * @return Feedback.
 */
static inline DRVEXOVector Feedback(const DRVEXOVector sensor, const DRVEXOVector reference) {
    if (DRVEXOVectorDot(sensor, reference) < 0.0f) {
        return DRVEXOVectorNormalise(DRVEXOVectorCross(sensor, reference)); // if error is >90 degrees
    }
    return DRVEXOVectorCross(sensor, reference);
}

/**
 * @brief Returns a value limited to maximum and minimum.
 * @param value Value.
 * @param min Minimum value.
 * @param max Maximum value.
 * @return Value limited to maximum and minimum.
 */
static inline int Clamp(const int value, const int min, const int max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

/**
 * @brief Updates the AHRS algorithm using the gyroscope and accelerometer.
 * @param ahrs AHRS structure.
 * @param gyroscope Gyroscope in degrees per second.
 * @param accelerometer Accelerometer in g.
 * @param deltaTime Delta time in seconds.
 */
void DRVEXOAhrsUpdateNoMagnetometer(DRVEXOAhrs *const ahrs, const DRVEXOVector gyroscope, const DRVEXOVector accelerometer, const float deltaTime) {
    DRVEXOAhrsUpdate(ahrs, gyroscope, accelerometer, DRVEXO_VECTOR_ZERO, deltaTime);

    // Zero heading during initialisation
    if (ahrs->initialising) {
        DRVEXOAhrsSetHeading(ahrs, 0.0f);
    }
}

/**
 * @brief Updates the AHRS algorithm using the gyroscope, accelerometer, and an
 * external measurement of heading.
 * @param ahrs AHRS structure.
 * @param gyroscope Gyroscope in degrees per second.
 * @param accelerometer Accelerometer in g.
 * @param heading Heading in degrees.
 * @param deltaTime Delta time in seconds.
 */
void DRVEXOAhrsUpdateExternalHeading(DRVEXOAhrs *const ahrs, const DRVEXOVector gyroscope, const DRVEXOVector accelerometer, const float heading, const float deltaTime) {
#define Q ahrs->quaternion.element
    const float roll = atan2f(Q.w * Q.x + Q.y * Q.z, 0.5f - Q.y * Q.y - Q.x * Q.x);
#undef Q

    // Calculate equivalent magnetometer
    const float headingRadians = DRVEXODegreesToRadians(heading);
    const float sinHeadingRadians = sinf(headingRadians);
    const DRVEXOVector magnetometer = {
        .axis = {
            .x = cosf(headingRadians),
            .y = -1.0f * cosf(roll) * sinHeadingRadians,
            .z = sinHeadingRadians * sinf(roll),
        }
    };

    // Update algorithm
    DRVEXOAhrsUpdate(ahrs, gyroscope, accelerometer, magnetometer, deltaTime);
}

/**
 * @brief Returns the quaternion.
 * @param ahrs AHRS structure.
 * @return Quaternion.
 */
DRVEXOQuaternion DRVEXOAhrsGetQuaternion(const DRVEXOAhrs *const ahrs) {
    return ahrs->quaternion;
}

/**
 * @brief Sets the quaternion.
 * @param ahrs AHRS structure.
 * @param quaternion Quaternion.
 */
void DRVEXOAhrsSetQuaternion(DRVEXOAhrs *const ahrs, const DRVEXOQuaternion quaternion) {
    ahrs->quaternion = quaternion;
}

/**
 * @brief Returns the direction of gravity.
 * @param ahrs AHRS structure.
 * @return Direction of gravity as a unit vector.
 */
DRVEXOVector DRVEXOAhrsGetGravity(const DRVEXOAhrs *const ahrs) {
#define Q ahrs->quaternion.element
    const DRVEXOVector gravity = {
        .axis = {
            .x = 2.0f * (Q.x * Q.z - Q.w * Q.y),
            .y = 2.0f * (Q.y * Q.z + Q.w * Q.x),
            .z = 2.0f * (Q.w * Q.w - 0.5f + Q.z * Q.z),
        }
    }; // third column of transposed rotation matrix
#undef Q
    return gravity;
}

/**
 * @brief Returns the linear acceleration.
 * @param ahrs AHRS structure.
 * @return Linear acceleration in g.
 */
DRVEXOVector DRVEXOAhrsGetLinearAcceleration(const DRVEXOAhrs *const ahrs) {
    switch (ahrs->settings.convention) {
        case DRVEXOConventionNwu:
        case DRVEXOConventionEnu: {
            return DRVEXOVectorSubtract(ahrs->accelerometer, DRVEXOAhrsGetGravity(ahrs));
        }
        case DRVEXOConventionNed: {
            return DRVEXOVectorAdd(ahrs->accelerometer, DRVEXOAhrsGetGravity(ahrs));
        }
    }
    return DRVEXO_VECTOR_ZERO; // avoid compiler warning
}

/**
 * @brief Returns the Earth acceleration.
 * @param ahrs AHRS structure.
 * @return Earth acceleration in g.
 */
DRVEXOVector DRVEXOAhrsGetEarthAcceleration(const DRVEXOAhrs *const ahrs) {
    // Calculate accelerometer in the Earth frame
#define Q ahrs->quaternion.element
#define A ahrs->accelerometer.axis
    DRVEXOVector accelerometer = {
        .axis = {
            .x = 2.0f * ((Q.w * Q.w - 0.5f + Q.x * Q.x) * A.x + (Q.x * Q.y - Q.w * Q.z) * A.y + (Q.x * Q.z + Q.w * Q.y) * A.z),
            .y = 2.0f * ((Q.x * Q.y + Q.w * Q.z) * A.x + (Q.w * Q.w - 0.5f + Q.y * Q.y) * A.y + (Q.y * Q.z - Q.w * Q.x) * A.z),
            .z = 2.0f * ((Q.x * Q.z - Q.w * Q.y) * A.x + (Q.y * Q.z + Q.w * Q.x) * A.y + (Q.w * Q.w - 0.5f + Q.z * Q.z) * A.z),
        }
    }; // rotation matrix multiplied with the accelerometer
#undef Q
#undef A

    // Remove gravity in the Earth frame
    switch (ahrs->settings.convention) {
        case DRVEXOConventionNwu:
        case DRVEXOConventionEnu:
            accelerometer.axis.z -= 1.0f;
            break;
        case DRVEXOConventionNed:
            accelerometer.axis.z += 1.0f;
            break;
    }
    return accelerometer;
}

/**
 * @brief Returns the internal states.
 * @param ahrs AHRS structure.
 * @return Internal states.
 */
DRVEXOAhrsInternalStates DRVEXOAhrsGetInternalStates(const DRVEXOAhrs *const ahrs) {
    const DRVEXOAhrsInternalStates internalStates = {
        .accelerationError = DRVEXORadiansToDegrees(DRVEXOArcSin(2.0f * DRVEXOVectorNorm(ahrs->halfAccelerometerFeedback))),
        .accelerometerIgnored = ahrs->accelerometerIgnored,
        .accelerationRecoveryTrigger = ahrs->settings.recoveryTriggerPeriod == 0 ? 0.0f : (float) ahrs->accelerationRecoveryTrigger / (float) ahrs->settings.recoveryTriggerPeriod,
        .magneticError = DRVEXORadiansToDegrees(DRVEXOArcSin(2.0f * DRVEXOVectorNorm(ahrs->halfMagnetometerFeedback))),
        .magnetometerIgnored = ahrs->magnetometerIgnored,
        .magneticRecoveryTrigger = ahrs->settings.recoveryTriggerPeriod == 0 ? 0.0f : (float) ahrs->magneticRecoveryTrigger / (float) ahrs->settings.recoveryTriggerPeriod,
    };
    return internalStates;
}

/**
 * @brief Returns the flags.
 * @param ahrs AHRS structure.
 * @return Flags.
 */
DRVEXOAhrsFlags DRVEXOAhrsGetFlags(const DRVEXOAhrs *const ahrs) {
    const DRVEXOAhrsFlags flags = {
        .initialising = ahrs->initialising,
        .angularRateRecovery = ahrs->angularRateRecovery,
        .accelerationRecovery = ahrs->accelerationRecoveryTrigger > ahrs->accelerationRecoveryTimeout,
        .magneticRecovery = ahrs->magneticRecoveryTrigger > ahrs->magneticRecoveryTimeout,
    };
    return flags;
}

/**
 * @brief Sets the heading.
 * @param ahrs AHRS structure.
 * @param heading Heading in degrees.
 */
void DRVEXOAhrsSetHeading(DRVEXOAhrs *const ahrs, const float heading) {
#define Q ahrs->quaternion.element
    const float yaw = atan2f(Q.w * Q.z + Q.x * Q.y, 0.5f - Q.y * Q.y - Q.z * Q.z);
#undef Q
    const float halfYawMinusHeading = 0.5f * (yaw - DRVEXODegreesToRadians(heading));
    const DRVEXOQuaternion rotation = {
        .element = {
            .w = cosf(halfYawMinusHeading),
            .x = 0.0f,
            .y = 0.0f,
            .z = -1.0f * sinf(halfYawMinusHeading),
        }
    };
    ahrs->quaternion = DRVEXOQuaternionProduct(rotation, ahrs->quaternion);
}

//------------------------------------------------------------------------------
// End of file
