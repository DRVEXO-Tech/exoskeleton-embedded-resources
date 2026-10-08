/**
 * @file DRVEXOBias.h
 * @author Seb Madgwick
 * @brief Run-time estimation and compensation of gyroscope offset.
 */

#ifndef DRVEXO_BIAS_H
#define DRVEXO_BIAS_H

//------------------------------------------------------------------------------
// Includes

#include "DRVEXOMath.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Bias structure. All members are private.
 */
typedef struct {
    float filterCoefficient;
    unsigned int timeout;
    unsigned int timer;
    DRVEXOVector gyroscopeOffset;
} DRVEXOBias;

//------------------------------------------------------------------------------
// Function declarations

void DRVEXOBiasInitialise(DRVEXOBias *const bias, const unsigned int sampleRate);

DRVEXOVector DRVEXOBiasUpdate(DRVEXOBias *const bias, DRVEXOVector gyroscope);

#endif

//------------------------------------------------------------------------------
// End of file
