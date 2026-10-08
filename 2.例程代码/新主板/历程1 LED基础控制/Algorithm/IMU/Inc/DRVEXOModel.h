/**
 * @file DRVEXOModel.h
 * @author Seb Madgwick
 * @brief Sensor models for applying calibration parameters.
 */

#ifndef DRVEXO_MODEL_H
#define DRVEXO_MODEL_H

//------------------------------------------------------------------------------
// Includes

#include "DRVEXOMath.h"

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Gyroscope and accelerometer sensor model.
 * @param uncalibrated Uncalibrated gyroscope or accelerometer.
 * @param misalignment Misalignment matrix.
 * @param sensitivity Sensitivity.
 * @param offset Offset.
 * @return Calibrated gyroscope or accelerometer.
 */
static inline DRVEXOVector DRVEXOModelInertial(const DRVEXOVector uncalibrated, const DRVEXOMatrix misalignment, const DRVEXOVector sensitivity, const DRVEXOVector offset) {
    return DRVEXOMatrixMultiply(misalignment, DRVEXOVectorHadamard(DRVEXOVectorSubtract(uncalibrated, offset), sensitivity));
}

/**
 * @brief Magnetometer sensor model.
 * @param uncalibrated Uncalibrated magnetometer.
 * @param softIronMatrix Soft-iron matrix.
 * @param hardIronOffset Hard-iron offset.
 * @return Calibrated magnetometer.
 */
static inline DRVEXOVector DRVEXOModelMagnetic(const DRVEXOVector uncalibrated, const DRVEXOMatrix softIronMatrix, const DRVEXOVector hardIronOffset) {
    return DRVEXOMatrixMultiply(softIronMatrix, DRVEXOVectorSubtract(uncalibrated, hardIronOffset));
}

#endif

//------------------------------------------------------------------------------
// End of file
