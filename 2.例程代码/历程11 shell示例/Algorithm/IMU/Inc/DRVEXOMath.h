/**
 * @file DRVEXOMath.h
 * @author Seb Madgwick
 * @brief Math library.
 */

#ifndef DRVEXO_MATH_H
#define DRVEXO_MATH_H

//------------------------------------------------------------------------------
// Includes

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief 3D vector.
 */
typedef union {
    float array[3];

    struct {
        float x;
        float y;
        float z;
    } axis;
} DRVEXOVector;

/**
 * @brief Quaternion.
 */
typedef union {
    float array[4];

    struct {
        float w;
        float x;
        float y;
        float z;
    } element;
} DRVEXOQuaternion;

/**
 * @brief 3x3 matrix in row-major order.
 */
typedef union {
    float array[9];

    struct {
        float xx;
        float xy;
        float xz;
        float yx;
        float yy;
        float yz;
        float zx;
        float zy;
        float zz;
    } element;
} DRVEXOMatrix;

/**
 * @brief ZYX Euler angles in degrees. Roll, pitch, and yaw are rotations
 * around X, Y, and Z respectively.
 */
typedef union {
    float array[3];

    struct {
        float roll;
        float pitch;
        float yaw;
    } angle;
} DRVEXOEuler;

/**
 * @brief Vector of zeros.
 */
#define DRVEXO_VECTOR_ZERO ((DRVEXOVector){ .array = {0.0f, 0.0f, 0.0f} })

/**
 * @brief Vector of ones.
 */
#define DRVEXO_VECTOR_ONES ((DRVEXOVector){ .array = {1.0f, 1.0f, 1.0f} })

/**
 * @brief Identity quaternion.
 */
#define DRVEXO_QUATERNION_IDENTITY ((DRVEXOQuaternion){ .array = {1.0f, 0.0f, 0.0f, 0.0f} })

/**
 * @brief Identity matrix.
 */
#define DRVEXO_MATRIX_IDENTITY ((DRVEXOMatrix){ .array = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f} })

/**
 * @brief Euler angles of zero.
 */
#define DRVEXO_EULER_ZERO ((DRVEXOEuler){ .array = {0.0f, 0.0f, 0.0f} })

/**
 * @brief Pi. May not be defined in math.h.
 */
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

/**
 * @brief Include this definition or add as a preprocessor definition to use
 * normal square root operations.
 */
//#define DRVEXO_USE_NORMAL_SQRT

//------------------------------------------------------------------------------
// Inline functions - Degrees and radians conversion

/**
 * @brief Converts degrees to radians.
 * @param degrees Degrees.
 * @return Radians.
 */
static inline float DRVEXODegreesToRadians(const float degrees) {
    return degrees * ((float) M_PI / 180.0f);
}

/**
 * @brief Converts radians to degrees.
 * @param radians Radians.
 * @return Degrees.
 */
static inline float DRVEXORadiansToDegrees(const float radians) {
    return radians * (180.0f / (float) M_PI);
}

//------------------------------------------------------------------------------
// Inline functions - Arc sine

/**
 * @brief Returns the arc sine of a value. Out of range values are clamped to
 * avoid a NaN result.
 * @param value Value.
 * @return Arc sine of the value.
 */
static inline float DRVEXOArcSin(const float value) {
    if (value <= -1.0f) {
        return (float) M_PI / -2.0f;
    }
    if (value >= 1.0f) {
        return (float) M_PI / 2.0f;
    }
    return asinf(value);
}

//------------------------------------------------------------------------------
// Inline functions - Fast inverse square root

#ifndef DRVEXO_USE_NORMAL_SQRT

/**
 * @brief Calculates the reciprocal of the square root.
 * @param x Operand.
 * @return Reciprocal of the square root of x.
 */
static inline float DRVEXOFastInverseSqrt(const float x) {
    typedef union {
        float f;
        int32_t i;
    } Union32;

    Union32 union32 = {.f = x};
    union32.i = 0x5F1F1412 - (union32.i >> 1);
    return union32.f * (1.69000231f - 0.714158168f * x * union32.f * union32.f);
}

#endif

//------------------------------------------------------------------------------
// Inline functions - Vector operations

/**
 * @brief Returns true if the vector is zero.
 * @param v Vector.
 * @return True if the vector is zero.
 */
static inline bool DRVEXOVectorIsZero(const DRVEXOVector v) {
    return (v.axis.x == 0.0f) && (v.axis.y == 0.0f) && (v.axis.z == 0.0f);
}

/**
 * @brief Returns the sum of two vectors.
 * @param a Vector a.
 * @param b Vector b.
 * @return Sum of two vectors.
 */
static inline DRVEXOVector DRVEXOVectorAdd(const DRVEXOVector a, const DRVEXOVector b) {
    const DRVEXOVector result = {
        .axis = {
            .x = a.axis.x + b.axis.x,
            .y = a.axis.y + b.axis.y,
            .z = a.axis.z + b.axis.z,
        }
    };
    return result;
}

/**
 * @brief Returns the subtraction of two vectors: a - b.
 * @param a Vector a.
 * @param b Vector b.
 * @return Subtraction of two vectors.
 */
static inline DRVEXOVector DRVEXOVectorSubtract(const DRVEXOVector a, const DRVEXOVector b) {
    const DRVEXOVector result = {
        .axis = {
            .x = a.axis.x - b.axis.x,
            .y = a.axis.y - b.axis.y,
            .z = a.axis.z - b.axis.z,
        }
    };
    return result;
}

/**
 * @brief Returns a scaled vector.
 * @param v Vector.
 * @param s Scalar.
 * @return Scaled vector.
 */
static inline DRVEXOVector DRVEXOVectorScale(const DRVEXOVector v, const float s) {
    const DRVEXOVector result = {
        .axis = {
            .x = v.axis.x * s,
            .y = v.axis.y * s,
            .z = v.axis.z * s,
        }
    };
    return result;
}

/**
 * @brief Returns the sum of the elements.
 * @param v Vector.
 * @return Sum of the elements.
 */
static inline float DRVEXOVectorSum(const DRVEXOVector v) {
    return v.axis.x + v.axis.y + v.axis.z;
}

/**
 * @brief Returns the Hadamard (element-wise) product.
 * @param a Vector a.
 * @param b Vector b.
 * @return Hadamard (element-wise) product.
 */
static inline DRVEXOVector DRVEXOVectorHadamard(const DRVEXOVector a, const DRVEXOVector b) {
    const DRVEXOVector result = {
        .axis = {
            .x = a.axis.x * b.axis.x,
            .y = a.axis.y * b.axis.y,
            .z = a.axis.z * b.axis.z,
        }
    };
    return result;
}

/**
 * @brief Returns the cross product: a cross b.
 * @param a Vector a.
 * @param b Vector b.
 * @return Cross product.
 */
static inline DRVEXOVector DRVEXOVectorCross(const DRVEXOVector a, const DRVEXOVector b) {
    const DRVEXOVector result = {
        .axis = {
            .x = a.axis.y * b.axis.z - a.axis.z * b.axis.y,
            .y = a.axis.z * b.axis.x - a.axis.x * b.axis.z,
            .z = a.axis.x * b.axis.y - a.axis.y * b.axis.x,
        }
    };
    return result;
}

/**
 * @brief Returns the dot product.
 * @param a Vector a.
 * @param b Vector b.
 * @return Dot product.
 */
static inline float DRVEXOVectorDot(const DRVEXOVector a, const DRVEXOVector b) {
    return DRVEXOVectorSum(DRVEXOVectorHadamard(a, b));
}

/**
 * @brief Returns the vector magnitude squared.
 * @param v Vector.
 * @return Vector magnitude squared.
 */
static inline float DRVEXOVectorNormSquared(const DRVEXOVector v) {
    return DRVEXOVectorSum(DRVEXOVectorHadamard(v, v));
}

/**
 * @brief Returns the vector magnitude.
 * @param v Vector.
 * @return Vector magnitude.
 */
static inline float DRVEXOVectorNorm(const DRVEXOVector v) {
    return sqrtf(DRVEXOVectorNormSquared(v));
}

/**
 * @brief Returns the normalised vector.
 * @param v Vector.
 * @return Normalised vector.
 */
static inline DRVEXOVector DRVEXOVectorNormalise(const DRVEXOVector v) {
#ifdef DRVEXO_USE_NORMAL_SQRT
    return DRVEXOVectorScale(v, 1.0f / DRVEXOVectorNorm(v));
#else
    return DRVEXOVectorScale(v, DRVEXOFastInverseSqrt(DRVEXOVectorNormSquared(v)));
#endif
}

//------------------------------------------------------------------------------
// Inline functions - Quaternion operations

/**
 * @brief Returns the sum of two quaternions.
 * @param a Quaternion a.
 * @param b Quaternion b.
 * @return Sum of two quaternions.
 */
static inline DRVEXOQuaternion DRVEXOQuaternionAdd(const DRVEXOQuaternion a, const DRVEXOQuaternion b) {
    const DRVEXOQuaternion result = {
        .element = {
            .w = a.element.w + b.element.w,
            .x = a.element.x + b.element.x,
            .y = a.element.y + b.element.y,
            .z = a.element.z + b.element.z,
        }
    };
    return result;
}

/**
 * @brief Returns a scaled quaternion.
 * @param q Quaternion.
 * @param s Scalar.
 * @return Scaled quaternion.
 */
static inline DRVEXOQuaternion DRVEXOQuaternionScale(const DRVEXOQuaternion q, const float s) {
    const DRVEXOQuaternion result = {
        .element = {
            .w = q.element.w * s,
            .x = q.element.x * s,
            .y = q.element.y * s,
            .z = q.element.z * s,
        }
    };
    return result;
}

/**
 * @brief Returns the sum of the elements.
 * @param q Quaternion.
 * @return Sum of the elements.
 */
static inline float DRVEXOQuaternionSum(const DRVEXOQuaternion q) {
    return q.element.w + q.element.x + q.element.y + q.element.z;
}

/**
 * @brief Returns the Hadamard (element-wise) product.
 * @param a Quaternion a.
 * @param b Quaternion b.
 * @return Hadamard (element-wise) product.
 */
static inline DRVEXOQuaternion DRVEXOQuaternionHadamard(const DRVEXOQuaternion a, const DRVEXOQuaternion b) {
    const DRVEXOQuaternion result = {
        .element = {
            .w = a.element.w * b.element.w,
            .x = a.element.x * b.element.x,
            .y = a.element.y * b.element.y,
            .z = a.element.z * b.element.z,
        }
    };
    return result;
}

/**
 * @brief Returns the quaternion product: a * b.
 * @param a Quaternion a.
 * @param b Quaternion b.
 * @return Quaternion product.
 */
static inline DRVEXOQuaternion DRVEXOQuaternionProduct(const DRVEXOQuaternion a, const DRVEXOQuaternion b) {
#define A a.element
#define B b.element
    const DRVEXOQuaternion result = {
        .element = {
            .w = A.w * B.w - A.x * B.x - A.y * B.y - A.z * B.z,
            .x = A.w * B.x + A.x * B.w + A.y * B.z - A.z * B.y,
            .y = A.w * B.y - A.x * B.z + A.y * B.w + A.z * B.x,
            .z = A.w * B.z + A.x * B.y - A.y * B.x + A.z * B.w,
        }
    };
#undef A
#undef B
    return result;
}

/**
 * @brief Returns the quaternion-vector product: q * v. The vector is treated
 * as a quaternion with w = 0.
 * @param q Quaternion.
 * @param v Vector.
 * @return Quaternion-vector product.
 */
static inline DRVEXOQuaternion DRVEXOQuaternionVectorProduct(const DRVEXOQuaternion q, const DRVEXOVector v) {
#define Q q.element
#define V v.axis
    const DRVEXOQuaternion result = {
        .element = {
            .w = -Q.x * V.x - Q.y * V.y - Q.z * V.z,
            .x = Q.w * V.x + Q.y * V.z - Q.z * V.y,
            .y = Q.w * V.y - Q.x * V.z + Q.z * V.x,
            .z = Q.w * V.z + Q.x * V.y - Q.y * V.x,
        }
    };
#undef Q
#undef V
    return result;
}

/**
 * @brief Returns the quaternion norm squared.
 * @param q Quaternion.
 * @return Quaternion norm squared.
 */
static inline float DRVEXOQuaternionNormSquared(const DRVEXOQuaternion q) {
    return DRVEXOQuaternionSum(DRVEXOQuaternionHadamard(q, q));
}

/**
 * @brief Returns the quaternion norm.
 * @param q Quaternion.
 * @return Quaternion norm.
 */
static inline float DRVEXOQuaternionNorm(const DRVEXOQuaternion q) {
    return sqrtf(DRVEXOQuaternionNormSquared(q));
}

/**
 * @brief Returns the normalised quaternion.
 * @param q Quaternion.
 * @return Normalised quaternion.
 */
static inline DRVEXOQuaternion DRVEXOQuaternionNormalise(const DRVEXOQuaternion q) {
#ifdef DRVEXO_USE_NORMAL_SQRT
    return DRVEXOQuaternionScale(q, 1.0f / DRVEXOQuaternionNorm(q));
#else
    return DRVEXOQuaternionScale(q, DRVEXOFastInverseSqrt(DRVEXOQuaternionNormSquared(q)));
#endif
}

//------------------------------------------------------------------------------
// Inline functions - Matrix operations

/**
 * @brief Returns the multiplication of a matrix and a vector: M * v.
 * @param m Matrix.
 * @param v Vector.
 * @return Multiplication of a matrix and a vector.
 */
static inline DRVEXOVector DRVEXOMatrixMultiply(const DRVEXOMatrix m, const DRVEXOVector v) {
#define M m.element
#define V v.axis
    const DRVEXOVector result = {
        .axis = {
            .x = M.xx * V.x + M.xy * V.y + M.xz * V.z,
            .y = M.yx * V.x + M.yy * V.y + M.yz * V.z,
            .z = M.zx * V.x + M.zy * V.y + M.zz * V.z,
        }
    };
#undef M
#undef V
    return result;
}

//------------------------------------------------------------------------------
// Inline functions - Conversions

/**
 * @brief Converts a quaternion to a rotation matrix.
 *
 * Quaternions and Rotation Sequences by Jack B. Kuipers, ISBN 0-691-10298-8,
 * Page 168. The matrix is the transpose of that shown in the book.
 *
 * @param q Quaternion.
 * @return Rotation matrix.
 */
static inline DRVEXOMatrix DRVEXOQuaternionToMatrix(const DRVEXOQuaternion q) {
#define Q q.element
    const float twoQw = 2.0f * Q.w;
    const float twoQx = 2.0f * Q.x;
    const float twoQy = 2.0f * Q.y;
    const float twoQz = 2.0f * Q.z;
    const DRVEXOMatrix matrix = {
        .element = {
            .xx = twoQw * Q.w - 1.0f + twoQx * Q.x,
            .xy = twoQx * Q.y - twoQw * Q.z,
            .xz = twoQx * Q.z + twoQw * Q.y,
            .yx = twoQx * Q.y + twoQw * Q.z,
            .yy = twoQw * Q.w - 1.0f + twoQy * Q.y,
            .yz = twoQy * Q.z - twoQw * Q.x,
            .zx = twoQx * Q.z - twoQw * Q.y,
            .zy = twoQy * Q.z + twoQw * Q.x,
            .zz = twoQw * Q.w - 1.0f + twoQz * Q.z,
        }
    };
#undef Q
    return matrix;
}

/**
 * @brief Converts a quaternion to Euler angles.
 *
 * Quaternions and Rotation Sequences by Jack B. Kuipers, ISBN 0-691-10298-8,
 * Page 168.
 *
 * @param q Quaternion.
 * @return Euler angles.
 */
static inline DRVEXOEuler DRVEXOQuaternionToEuler(const DRVEXOQuaternion q) {
#define Q q.element
    const DRVEXOEuler euler = {
        .angle = {
            .roll = DRVEXORadiansToDegrees(atan2f(Q.y * Q.z + Q.w * Q.x, Q.w * Q.w + Q.z * Q.z - 0.5f)),
            .pitch = DRVEXORadiansToDegrees(DRVEXOArcSin(2.0f * (Q.w * Q.y - Q.x * Q.z))),
            .yaw = DRVEXORadiansToDegrees(atan2f(Q.x * Q.y + Q.w * Q.z, Q.w * Q.w + Q.x * Q.x - 0.5f)),
        }
    };
#undef Q
    return euler;
}

#endif

//------------------------------------------------------------------------------
// End of file
