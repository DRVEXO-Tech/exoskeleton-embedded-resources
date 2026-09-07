/**
 * @file DRVEXORemap.h
 * @author Seb Madgwick
 * @brief Remaps the sensor axes to the body frame.
 */

#ifndef DRVEXO_REMAP_H
#define DRVEXO_REMAP_H

//------------------------------------------------------------------------------
// Includes

#include "DRVEXOMath.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Alignment of the sensor axes relative to the body frame. For example,
 * if the body X axis is aligned with the sensor Y axis and the body Y axis is
 * aligned with the sensor X axis but pointing the opposite direction, then
 * alignment is +Y-X+Z.
 */
typedef enum {
    DRVEXORemapAlignmentPXPYPZ, /* +X+Y+Z */
    DRVEXORemapAlignmentPXPZNY, /* +X+Z-Y */
    DRVEXORemapAlignmentPXNZPY, /* +X-Z+Y */
    DRVEXORemapAlignmentPXNYNZ, /* +X-Y-Z */
    DRVEXORemapAlignmentPYPXNZ, /* +Y+X-Z */
    DRVEXORemapAlignmentPYPZPX, /* +Y+Z+X */
    DRVEXORemapAlignmentPYNZNX, /* +Y-Z-X */
    DRVEXORemapAlignmentPYNXPZ, /* +Y-X+Z */
    DRVEXORemapAlignmentPZPXPY, /* +Z+X+Y */
    DRVEXORemapAlignmentPZPYNX, /* +Z+Y-X */
    DRVEXORemapAlignmentPZNYPX, /* +Z-Y+X */
    DRVEXORemapAlignmentPZNXNY, /* +Z-X-Y */
    DRVEXORemapAlignmentNZPXNY, /* -Z+X-Y */
    DRVEXORemapAlignmentNZPYPX, /* -Z+Y+X */
    DRVEXORemapAlignmentNZNYNX, /* -Z-Y-X */
    DRVEXORemapAlignmentNZNXPY, /* -Z-X+Y */
    DRVEXORemapAlignmentNYPXPZ, /* -Y+X+Z */
    DRVEXORemapAlignmentNYPZNX, /* -Y+Z-X */
    DRVEXORemapAlignmentNYNZPX, /* -Y-Z+X */
    DRVEXORemapAlignmentNYNXNZ, /* -Y-X-Z */
    DRVEXORemapAlignmentNXPYNZ, /* -X+Y-Z */
    DRVEXORemapAlignmentNXPZPY, /* -X+Z+Y */
    DRVEXORemapAlignmentNXNZNY, /* -X-Z-Y */
    DRVEXORemapAlignmentNXNYPZ, /* -X-Y+Z */
} DRVEXORemapAlignment;

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Remaps the sensor axes to the body frame.
 * @param sensor Sensor.
 * @param alignment Alignment.
 * @return Sensor remapped to the body frame.
 */
static inline DRVEXOVector DRVEXORemap(const DRVEXOVector sensor, const DRVEXORemapAlignment alignment) {
    DRVEXOVector result;
    switch (alignment) {
        case DRVEXORemapAlignmentPXPYPZ:
            break;
        case DRVEXORemapAlignmentPXPZNY:
            result.axis.x = +sensor.axis.x;
            result.axis.y = +sensor.axis.z;
            result.axis.z = -sensor.axis.y;
            return result;
        case DRVEXORemapAlignmentPXNZPY:
            result.axis.x = +sensor.axis.x;
            result.axis.y = -sensor.axis.z;
            result.axis.z = +sensor.axis.y;
            return result;
        case DRVEXORemapAlignmentPXNYNZ:
            result.axis.x = +sensor.axis.x;
            result.axis.y = -sensor.axis.y;
            result.axis.z = -sensor.axis.z;
            return result;
        case DRVEXORemapAlignmentPYPXNZ:
            result.axis.x = +sensor.axis.y;
            result.axis.y = +sensor.axis.x;
            result.axis.z = -sensor.axis.z;
            return result;
        case DRVEXORemapAlignmentPYPZPX:
            result.axis.x = +sensor.axis.y;
            result.axis.y = +sensor.axis.z;
            result.axis.z = +sensor.axis.x;
            return result;
        case DRVEXORemapAlignmentPYNZNX:
            result.axis.x = +sensor.axis.y;
            result.axis.y = -sensor.axis.z;
            result.axis.z = -sensor.axis.x;
            return result;
        case DRVEXORemapAlignmentPYNXPZ:
            result.axis.x = +sensor.axis.y;
            result.axis.y = -sensor.axis.x;
            result.axis.z = +sensor.axis.z;
            return result;
        case DRVEXORemapAlignmentPZPXPY:
            result.axis.x = +sensor.axis.z;
            result.axis.y = +sensor.axis.x;
            result.axis.z = +sensor.axis.y;
            return result;
        case DRVEXORemapAlignmentPZPYNX:
            result.axis.x = +sensor.axis.z;
            result.axis.y = +sensor.axis.y;
            result.axis.z = -sensor.axis.x;
            return result;
        case DRVEXORemapAlignmentPZNYPX:
            result.axis.x = +sensor.axis.z;
            result.axis.y = -sensor.axis.y;
            result.axis.z = +sensor.axis.x;
            return result;
        case DRVEXORemapAlignmentPZNXNY:
            result.axis.x = +sensor.axis.z;
            result.axis.y = -sensor.axis.x;
            result.axis.z = -sensor.axis.y;
            return result;
        case DRVEXORemapAlignmentNZPXNY:
            result.axis.x = -sensor.axis.z;
            result.axis.y = +sensor.axis.x;
            result.axis.z = -sensor.axis.y;
            return result;
        case DRVEXORemapAlignmentNZPYPX:
            result.axis.x = -sensor.axis.z;
            result.axis.y = +sensor.axis.y;
            result.axis.z = +sensor.axis.x;
            return result;
        case DRVEXORemapAlignmentNZNYNX:
            result.axis.x = -sensor.axis.z;
            result.axis.y = -sensor.axis.y;
            result.axis.z = -sensor.axis.x;
            return result;
        case DRVEXORemapAlignmentNZNXPY:
            result.axis.x = -sensor.axis.z;
            result.axis.y = -sensor.axis.x;
            result.axis.z = +sensor.axis.y;
            return result;
        case DRVEXORemapAlignmentNYPXPZ:
            result.axis.x = -sensor.axis.y;
            result.axis.y = +sensor.axis.x;
            result.axis.z = +sensor.axis.z;
            return result;
        case DRVEXORemapAlignmentNYPZNX:
            result.axis.x = -sensor.axis.y;
            result.axis.y = +sensor.axis.z;
            result.axis.z = -sensor.axis.x;
            return result;
        case DRVEXORemapAlignmentNYNZPX:
            result.axis.x = -sensor.axis.y;
            result.axis.y = -sensor.axis.z;
            result.axis.z = +sensor.axis.x;
            return result;
        case DRVEXORemapAlignmentNYNXNZ:
            result.axis.x = -sensor.axis.y;
            result.axis.y = -sensor.axis.x;
            result.axis.z = -sensor.axis.z;
            return result;
        case DRVEXORemapAlignmentNXPYNZ:
            result.axis.x = -sensor.axis.x;
            result.axis.y = +sensor.axis.y;
            result.axis.z = -sensor.axis.z;
            return result;
        case DRVEXORemapAlignmentNXPZPY:
            result.axis.x = -sensor.axis.x;
            result.axis.y = +sensor.axis.z;
            result.axis.z = +sensor.axis.y;
            return result;
        case DRVEXORemapAlignmentNXNZNY:
            result.axis.x = -sensor.axis.x;
            result.axis.y = -sensor.axis.z;
            result.axis.z = -sensor.axis.y;
            return result;
        case DRVEXORemapAlignmentNXNYPZ:
            result.axis.x = -sensor.axis.x;
            result.axis.y = -sensor.axis.y;
            result.axis.z = +sensor.axis.z;
            return result;
    }
    return sensor; // avoid compiler warning
}

#endif

//------------------------------------------------------------------------------
// End of file
