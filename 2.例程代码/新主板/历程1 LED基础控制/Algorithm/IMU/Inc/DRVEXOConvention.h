/**
 * @file DRVEXOConvention.h
 * @author Seb Madgwick
 * @brief Earth axes convention.
 */

#ifndef DRVEXO_CONVENTION_H
#define DRVEXO_CONVENTION_H

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Earth axes convention.
 */
typedef enum {
    DRVEXOConventionNwu, /* North (X), West (Y), Up (Z) */
    DRVEXOConventionEnu, /* East (X), North (Y), Up (Z) */
    DRVEXOConventionNed, /* North (X), East (Y), Down (Z) */
} DRVEXOConvention;

#endif

//------------------------------------------------------------------------------
// End of file
