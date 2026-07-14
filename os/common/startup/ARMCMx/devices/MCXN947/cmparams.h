/**
 * @file    MCXN947/cmparams.h
 * @brief   ARM Cortex-M33 parameters for the MCXN947.
 * @pre     This file depends on the @p mcx_device.h .
 *
 * @defgroup    ARMCMx_MCXN947 MCXN947 Specific Parameters
 * @ingroup     ARMCMx_SPECIFIC
 * @details     This file contains the Cortex-M33 specific parameters for the
 *              MCXN947 platform.
 *
 * @{
 */

#ifndef CMPARAMS_H
#define CMPARAMS_H

/**
 * @brief   Cortex core model.
 */
#define CORTEX_MODEL            33

/**
 * @brief   Floating Point unit presence.
 */
#define CORTEX_HAS_FPU          1

/**
 * @brief   Number of bits in priority masks.
 */
#define CORTEX_PRIORITY_BITS    3U

/**
 * @brief   Number of interrupt vectors.
 * @note    This number does not include the 16 system vectors and must be
 *          rounded to a multiple of 8.
 */
#define CORTEX_NUM_VECTORS      160

/**
 * @brief   Reserved exception vectors must be emitted as zeroes.
 * @note    The MCXN ROM uses the reserved exception slots as boot image
 *          metadata.
 */
#define CORTEX_RESERVED_VECTORS_ZERO 1

/**
 * @brief   Number of MPU regions.
 */
#define CORTEX_MPU_REGIONS      8

/**
 * @brief   Number of secure MPU regions.
 */
#define CORTEX_MPU_S_REGIONS    12

/* The following code is not processed when the file is included from an
   asm module. */
#if !defined(_FROM_ASM_)

/* Including the device CMSIS header. Note, we are not using the definitions
   from this header because we need this file to be usable also from assembler
   source files. We verify that the info matches instead. */
#include "fsl_device_registers.h"

#if CORTEX_MODEL != __CORTEX_M
#error "CMSIS __CORTEX_M mismatch"
#endif

#if CORTEX_HAS_FPU != __FPU_PRESENT
#error "CMSIS __FPU_PRESENT mismatch"
#endif

#if CORTEX_PRIORITY_BITS != __NVIC_PRIO_BITS
#error "CMSIS __NVIC_PRIO_BITS mismatch"
#endif

#endif /* !defined(_FROM_ASM_) */

#endif /* CMPARAMS_H */

/** @} */