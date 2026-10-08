/**
 * @file DRVEXOCompass.h
 * @author Seb Madgwick
 * @brief Tilt-compensated compass to calculate magnetic heading.
 */

#ifndef DRVEXO_COMPASS_H
#define DRVEXO_COMPASS_H

//------------------------------------------------------------------------------
// Includes

#include "DRVEXOConvention.h"
#include "DRVEXOMath.h"

//------------------------------------------------------------------------------
// Function declarations

float DRVEXOCompass(const DRVEXOVector accelerometer, const DRVEXOVector magnetometer, const DRVEXOConvention convention);

#endif

//------------------------------------------------------------------------------
// End of file
