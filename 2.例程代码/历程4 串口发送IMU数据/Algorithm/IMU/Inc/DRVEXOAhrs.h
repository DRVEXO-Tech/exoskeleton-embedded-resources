/**
 * @file DRVEXOAhrs.h
 * @author Seb Madgwick
 * @brief Attitude and Heading Reference System (AHRS) algorithm.
 */

#ifndef DRVEXO_AHRS_H
#define DRVEXO_AHRS_H

//------------------------------------------------------------------------------
// Includes

#include "DRVEXOConvention.h"
#include "DRVEXOMath.h"
#include <stdbool.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Settings.
 */
typedef struct {
    DRVEXOConvention convention;
    float gain;
    float gyroscopeRange;
    float accelerationRejection;
    float magneticRejection;
    unsigned int recoveryTriggerPeriod;
} DRVEXOAhrsSettings;

/**
 * @brief AHRS structure. All members are private.
 */
typedef struct {
    DRVEXOAhrsSettings settings;
    DRVEXOQuaternion quaternion;
    DRVEXOVector accelerometer;
    bool initialising;
    float rampedGain;
    float rampedGainStep;
    bool angularRateRecovery;
    DRVEXOVector halfAccelerometerFeedback;
    DRVEXOVector halfMagnetometerFeedback;
    bool accelerometerIgnored;
    int accelerationRecoveryTrigger;
    int accelerationRecoveryTimeout;
    bool magnetometerIgnored;
    int magneticRecoveryTrigger;
    int magneticRecoveryTimeout;
} DRVEXOAhrs;

/**
 * @brief Internal states.
 */
typedef struct {
    float accelerationError;
    bool accelerometerIgnored;
    float accelerationRecoveryTrigger;
    float magneticError;
    bool magnetometerIgnored;
    float magneticRecoveryTrigger;
} DRVEXOAhrsInternalStates;

/**
 * @brief Flags.
 */
typedef struct {
    bool initialising;
    bool angularRateRecovery;
    bool accelerationRecovery;
    bool magneticRecovery;
} DRVEXOAhrsFlags;

//------------------------------------------------------------------------------
// Variable declarations

extern const DRVEXOAhrsSettings DRVEXOAhrsDefaultSettings;

//------------------------------------------------------------------------------
// Function declarations

void DRVEXOAhrsInitialise(DRVEXOAhrs *const ahrs);

void DRVEXOAhrsReset(DRVEXOAhrs *const ahrs);

void DRVEXOAhrsSetSettings(DRVEXOAhrs *const ahrs, const DRVEXOAhrsSettings *const settings);

void DRVEXOAhrsUpdate(DRVEXOAhrs *const ahrs, const DRVEXOVector gyroscope, const DRVEXOVector accelerometer, const DRVEXOVector magnetometer, const float deltaTime);

void DRVEXOAhrsUpdateNoMagnetometer(DRVEXOAhrs *const ahrs, const DRVEXOVector gyroscope, const DRVEXOVector accelerometer, const float deltaTime);

void DRVEXOAhrsUpdateExternalHeading(DRVEXOAhrs *const ahrs, const DRVEXOVector gyroscope, const DRVEXOVector accelerometer, const float heading, const float deltaTime);

DRVEXOQuaternion DRVEXOAhrsGetQuaternion(const DRVEXOAhrs *const ahrs);

void DRVEXOAhrsSetQuaternion(DRVEXOAhrs *const ahrs, const DRVEXOQuaternion quaternion);

DRVEXOVector DRVEXOAhrsGetGravity(const DRVEXOAhrs *const ahrs);

DRVEXOVector DRVEXOAhrsGetLinearAcceleration(const DRVEXOAhrs *const ahrs);

DRVEXOVector DRVEXOAhrsGetEarthAcceleration(const DRVEXOAhrs *const ahrs);

DRVEXOAhrsInternalStates DRVEXOAhrsGetInternalStates(const DRVEXOAhrs *const ahrs);

DRVEXOAhrsFlags DRVEXOAhrsGetFlags(const DRVEXOAhrs *const ahrs);

void DRVEXOAhrsSetHeading(DRVEXOAhrs *const ahrs, const float heading);

#endif

//------------------------------------------------------------------------------
// End of file
