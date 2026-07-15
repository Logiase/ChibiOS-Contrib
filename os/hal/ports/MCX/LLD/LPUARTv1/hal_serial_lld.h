/**
 * @file    LPUARTv1/hal_serial_lld.h
 * @brief   MCX serial subsystem low level driver header.
 *
 * @addtogroup SERIAL
 * @{
 */

#ifndef HAL_SERIAL_LLD_H
#define HAL_SERIAL_LLD_H

#if (HAL_USE_SERIAL == TRUE) || defined(__DOXYGEN__)

#include "fsl_lpuart.h"
#include "fsl_clock.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    MCX configuration options
 * @{
 */
/**
 * @brief   LPUART0 driver enable switch.
 * @details If set to @p TRUE the support for LPUART0 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART0) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART0             FALSE
#endif

/**
 * @brief   LPUART1 driver enable switch.
 * @details If set to @p TRUE the support for LPUART1 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART1) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART1             FALSE
#endif

/**
 * @brief   LPUART2 driver enable switch.
 * @details If set to @p TRUE the support for LPUART2 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART2) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART2             FALSE
#endif

/**
 * @brief   LPUART3 driver enable switch.
 * @details If set to @p TRUE the support for LPUART3 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART3) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART3             FALSE
#endif

/**
 * @brief   LPUART4 driver enable switch.
 * @details If set to @p TRUE the support for LPUART4 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART4) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART4             FALSE
#endif

/**
 * @brief   LPUART5 driver enable switch.
 * @details If set to @p TRUE the support for LPUART5 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART5) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART5             FALSE
#endif

/**
 * @brief   LPUART6 driver enable switch.
 * @details If set to @p TRUE the support for LPUART6 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART6) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART6             FALSE
#endif

/**
 * @brief   LPUART7 driver enable switch.
 * @details If set to @p TRUE the support for LPUART7 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART7) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART7             FALSE
#endif

/**
 * @brief   LPUART8 driver enable switch.
 * @details If set to @p TRUE the support for LPUART8 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART8) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART8             FALSE
#endif

/**
 * @brief   LPUART9 driver enable switch.
 * @details If set to @p TRUE the support for LPUART9 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_SERIAL_USE_LPUART9) || defined(__DOXYGEN__)
#define MCX_SERIAL_USE_LPUART9             FALSE
#endif

/**
 * @brief   LPUART interrupt priority level setting.
 */
#if !defined(MCX_SERIAL_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define MCX_SERIAL_IRQ_PRIORITY            7
#endif

#if !defined(MCX_SERIAL_LPUART0_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART0_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM0
#endif

#if !defined(MCX_SERIAL_LPUART1_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART1_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM1
#endif

#if !defined(MCX_SERIAL_LPUART2_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART2_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM2
#endif

#if !defined(MCX_SERIAL_LPUART3_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART3_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM3
#endif

#if !defined(MCX_SERIAL_LPUART4_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART4_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM4
#endif

#if !defined(MCX_SERIAL_LPUART5_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART5_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM5
#endif

#if !defined(MCX_SERIAL_LPUART6_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART6_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM6
#endif

#if !defined(MCX_SERIAL_LPUART7_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART7_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM7
#endif

#if !defined(MCX_SERIAL_LPUART8_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART8_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM8
#endif

#if !defined(MCX_SERIAL_LPUART9_CLK_ATTACH) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART9_CLK_ATTACH      kFRO_HF_DIV_to_FLEXCOMM9
#endif

#if !defined(MCX_SERIAL_LPUART0_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART0_CLK_DIV         1U
#endif

#if !defined(MCX_SERIAL_LPUART1_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART1_CLK_DIV         1U
#endif

#if !defined(MCX_SERIAL_LPUART2_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART2_CLK_DIV         1U
#endif

#if !defined(MCX_SERIAL_LPUART3_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART3_CLK_DIV         1U
#endif

#if !defined(MCX_SERIAL_LPUART4_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART4_CLK_DIV         1U
#endif

#if !defined(MCX_SERIAL_LPUART5_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART5_CLK_DIV         1U
#endif

#if !defined(MCX_SERIAL_LPUART6_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART6_CLK_DIV         1U
#endif

#if !defined(MCX_SERIAL_LPUART7_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART7_CLK_DIV         1U
#endif

#if !defined(MCX_SERIAL_LPUART8_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART8_CLK_DIV         1U
#endif

#if !defined(MCX_SERIAL_LPUART9_CLK_DIV) || defined(__DOXYGEN__)
#define MCX_SERIAL_LPUART9_CLK_DIV         1U
#endif

/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if !MCX_SERIAL_USE_LPUART0 && !MCX_SERIAL_USE_LPUART1 && \
    !MCX_SERIAL_USE_LPUART2 && !MCX_SERIAL_USE_LPUART3 && \
    !MCX_SERIAL_USE_LPUART4 && !MCX_SERIAL_USE_LPUART5 && \
    !MCX_SERIAL_USE_LPUART6 && !MCX_SERIAL_USE_LPUART7 && \
    !MCX_SERIAL_USE_LPUART8 && !MCX_SERIAL_USE_LPUART9
#error "SERIAL driver activated but no LPUART peripheral assigned"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   PLATFORM Serial Driver configuration structure.
 * @details An instance of this structure must be passed to @p sdStart()
 *          in order to configure and start a serial driver.
 * @note    This structure content is architecture dependent, each driver
 *          implementation defines its own version and the custom static
 *          initializers.
 */
typedef struct hal_serial_config {
  /**
   * @brief Bit rate.
   */
  uint32_t                  speed;
  /* End of the mandatory fields.*/
} SerialConfig;

/**
 * @brief   @p SerialDriver specific data.
 */
#define _serial_driver_data                                                 \
  _base_asynchronous_channel_data                                           \
  /* Driver state.*/                                                        \
  sdstate_t                 state;                                          \
  /* Input queue.*/                                                         \
  input_queue_t             iqueue;                                         \
  /* Output queue.*/                                                        \
  output_queue_t            oqueue;                                         \
  /* Input circular buffer.*/                                               \
  uint8_t                   ib[SERIAL_BUFFERS_SIZE];                        \
  /* Output circular buffer.*/                                              \
  uint8_t                   ob[SERIAL_BUFFERS_SIZE];                        \
  /* End of the mandatory fields.*/                                         \
  /* Pointer to the LPUART registers block. */                              \
  LPUART_Type               *lpuart;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if MCX_SERIAL_USE_LPUART0 && !defined(__DOXYGEN__)
extern SerialDriver SD0;
#endif

#if MCX_SERIAL_USE_LPUART1 && !defined(__DOXYGEN__)
extern SerialDriver SD1;
#endif

#if MCX_SERIAL_USE_LPUART2 && !defined(__DOXYGEN__)
extern SerialDriver SD2;
#endif

#if MCX_SERIAL_USE_LPUART3 && !defined(__DOXYGEN__)
extern SerialDriver SD3;
#endif

#if MCX_SERIAL_USE_LPUART4 && !defined(__DOXYGEN__)
extern SerialDriver SD4;
#endif

#if MCX_SERIAL_USE_LPUART5 && !defined(__DOXYGEN__)
extern SerialDriver SD5;
#endif

#if MCX_SERIAL_USE_LPUART6 && !defined(__DOXYGEN__)
extern SerialDriver SD6;
#endif

#if MCX_SERIAL_USE_LPUART7 && !defined(__DOXYGEN__)
extern SerialDriver SD7;
#endif

#if MCX_SERIAL_USE_LPUART8 && !defined(__DOXYGEN__)
extern SerialDriver SD8;
#endif

#if MCX_SERIAL_USE_LPUART9 && !defined(__DOXYGEN__)
extern SerialDriver SD9;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void sd_lld_init(void);
  void sd_lld_start(SerialDriver *sdp, const SerialConfig *config);
  void sd_lld_stop(SerialDriver *sdp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_SERIAL == TRUE */

#endif /* HAL_SERIAL_LLD_H */

/** @} */
