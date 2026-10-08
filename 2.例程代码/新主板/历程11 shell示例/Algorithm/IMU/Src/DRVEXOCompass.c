/**
 * @file DRVEXOCompass.c
 * @author Seb Madgwick
 * @brief Tilt-compensated compass to calculate magnetic heading.
 */

//------------------------------------------------------------------------------
// Includes

#include "DRVEXOCompass.h"
#include <math.h>

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Calculates magnetic heading.
 * @param accelerometer Accelerometer in any calibrated units.
 * @param magnetometer Magnetometer in any calibrated units.
 * @param convention Earth axes convention.
 * @return Magnetic heading in degrees.
 */
float DRVEXOCompass(const DRVEXOVector accelerometer, const DRVEXOVector magnetometer, const DRVEXOConvention convention) {
    switch (convention) {
        case DRVEXOConventionNwu: {
            const DRVEXOVector west = DRVEXOVectorNormalise(DRVEXOVectorCross(accelerometer, magnetometer));
            const DRVEXOVector north = DRVEXOVectorNormalise(DRVEXOVectorCross(west, accelerometer));
            return DRVEXORadiansToDegrees(atan2f(west.axis.x, north.axis.x));
        }
        case DRVEXOConventionEnu: {
            const DRVEXOVector west = DRVEXOVectorNormalise(DRVEXOVectorCross(accelerometer, magnetometer));
            const DRVEXOVector north = DRVEXOVectorNormalise(DRVEXOVectorCross(west, accelerometer));
            const DRVEXOVector east = DRVEXOVectorScale(west, -1.0f);
            return DRVEXORadiansToDegrees(atan2f(north.axis.x, east.axis.x));
        }
        case DRVEXOConventionNed: {
            const DRVEXOVector up = DRVEXOVectorScale(accelerometer, -1.0f);
            const DRVEXOVector west = DRVEXOVectorNormalise(DRVEXOVectorCross(up, magnetometer));
            const DRVEXOVector north = DRVEXOVectorNormalise(DRVEXOVectorCross(west, up));
            return DRVEXORadiansToDegrees(atan2f(west.axis.x, north.axis.x));
        }
    }
    return 0.0f; // avoid compiler warning
}

//------------------------------------------------------------------------------
// End of file
