/*
    ChibiOS - Copyright (C) 2006-2026 Giovanni Di Sirio.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    hal_serial_lld.c
 * @brief   MCX serial subsystem low level driver source.
 *
 * @addtogroup SERIAL
 * @{
 */

#include "hal.h"

#if (HAL_USE_SERIAL == TRUE) || defined(__DOXYGEN__)

#include "fsl_lpuart.h"
#include "fsl_clock.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define MCX_LPUART_RX_INTERRUPTS                                            \
  (kLPUART_RxDataRegFullInterruptEnable |                                   \
   kLPUART_RxOverrunInterruptEnable     |                                   \
   kLPUART_NoiseErrorInterruptEnable    |                                   \
   kLPUART_FramingErrorInterruptEnable  |                                   \
   kLPUART_ParityErrorInterruptEnable)

#define MCX_LPUART_ERROR_FLAGS                                              \
  (kLPUART_RxOverrunFlag    |                                               \
   kLPUART_NoiseErrorFlag   |                                               \
   kLPUART_FramingErrorFlag |                                               \
   kLPUART_ParityErrorFlag)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief LPUART0 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART0 || defined(__DOXYGEN__)
SerialDriver SD0;
#endif

/** @brief LPUART1 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART1 || defined(__DOXYGEN__)
SerialDriver SD1;
#endif

/** @brief LPUART2 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART2 || defined(__DOXYGEN__)
SerialDriver SD2;
#endif

/** @brief LPUART3 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART3 || defined(__DOXYGEN__)
SerialDriver SD3;
#endif

/** @brief LPUART4 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART4 || defined(__DOXYGEN__)
SerialDriver SD4;
#endif

/** @brief LPUART5 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART5 || defined(__DOXYGEN__)
SerialDriver SD5;
#endif

/** @brief LPUART6 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART6 || defined(__DOXYGEN__)
SerialDriver SD6;
#endif

/** @brief LPUART7 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART7 || defined(__DOXYGEN__)
SerialDriver SD7;
#endif

/** @brief LPUART8 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART8 || defined(__DOXYGEN__)
SerialDriver SD8;
#endif

/** @brief LPUART9 serial driver identifier.*/
#if MCX_SERIAL_USE_LPUART9 || defined(__DOXYGEN__)
SerialDriver SD9;
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/**
 * @brief   Driver default configuration.
 */
static const SerialConfig default_config = {
  SERIAL_DEFAULT_BITRATE
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static void set_error(SerialDriver *sdp, uint32_t flags) {
  eventflags_t sts = 0U;

  if ((flags & kLPUART_RxOverrunFlag) != 0U) {
    sts |= SD_OVERRUN_ERROR;
  }
  if ((flags & kLPUART_ParityErrorFlag) != 0U) {
    sts |= SD_PARITY_ERROR;
  }
  if ((flags & kLPUART_FramingErrorFlag) != 0U) {
    sts |= SD_FRAMING_ERROR;
  }
  if ((flags & kLPUART_NoiseErrorFlag) != 0U) {
    sts |= SD_NOISE_ERROR;
  }

  if (sts != 0U) {
    osalSysLockFromISR();
    chnAddFlagsI(sdp, sts);
    osalSysUnlockFromISR();
  }
}

/**
 * @brief   Attempts a TX preload.
 */
static void preload(SerialDriver *sdp) {
  LPUART_Type *base = sdp->lpuart;

  if (oqIsEmptyI(&sdp->oqueue)) {
    chnAddFlagsI(sdp, CHN_OUTPUT_EMPTY);
    return;
  }

  LPUART_EnableInterrupts(base, kLPUART_TxDataRegEmptyInterruptEnable);

  if ((LPUART_GetStatusFlags(base) & kLPUART_TxDataRegEmptyFlag) != 0U) {
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    uint32_t count = (uint32_t)FSL_FEATURE_LPUART_FIFO_SIZEn(base) -
                     (uint32_t)LPUART_GetTxFifoCount(base);
#else
    uint32_t count = 1U;
#endif

    while (count > 0U) {
      msg_t b = oqGetI(&sdp->oqueue);

      if (b < Q_OK) {
        chnAddFlagsI(sdp, CHN_OUTPUT_EMPTY);
        break;
      }

      LPUART_WriteByte(base, (uint8_t)b);
      count--;
    }
  }
}

/**
 * @brief   Driver output notification.
 */
static void notify(io_queue_t *qp) {

  preload(qp->q_link);
}

static void init_driver(SerialDriver *sdp, LPUART_Type *base) {

  sdObjectInit(sdp, NULL, notify);
  sdp->lpuart = base;
}

static void enable_lpuart(LPUART_Type *base,
                          IRQn_Type irq,
                          uint32_t instance,
                          clock_attach_id_t clk_attach,
                          clock_div_name_t clk_div,
                          uint32_t div,
                          const SerialConfig *config) {
  lpuart_config_t lpuart_config;
  status_t status;

  CLOCK_SetClkDiv(clk_div, div);
  CLOCK_AttachClk(clk_attach);

  LPUART_GetDefaultConfig(&lpuart_config);
  lpuart_config.baudRate_Bps = config->speed;
  lpuart_config.enableTx = true;
  lpuart_config.enableRx = true;

  status = LPUART_Init(base, &lpuart_config,
                       CLOCK_GetLPFlexCommClkFreq(instance));
  osalDbgAssert(status == kStatus_Success, "LPUART init failed");

  (void)LPUART_ClearStatusFlags(base, kLPUART_AllClearFlags);
  LPUART_EnableInterrupts(base, MCX_LPUART_RX_INTERRUPTS);
  nvicEnableVector(irq, MCX_SERIAL_IRQ_PRIORITY);
}

static void disable_lpuart(LPUART_Type *base, IRQn_Type irq) {

  LPUART_DisableInterrupts(base, kLPUART_AllInterruptEnable);
  nvicDisableVector(irq);
  LPUART_Deinit(base);
}

static void serve_interrupt(SerialDriver *sdp) {
  LPUART_Type *base = sdp->lpuart;
  uint32_t status = LPUART_GetStatusFlags(base);
  uint32_t enabled = LPUART_GetEnabledInterrupts(base);

  if (((status & kLPUART_RxDataRegFullFlag) != 0U) &&
      ((enabled & kLPUART_RxDataRegFullInterruptEnable) != 0U)) {
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    uint32_t count = (uint32_t)LPUART_GetRxFifoCount(base);
    if (count == 0U) {
      count = 1U;
    }
#else
    uint32_t count = 1U;
#endif

    while (count > 0U) {
      uint8_t b = LPUART_ReadByte(base);

      osalSysLockFromISR();
      if (iqIsEmptyI(&sdp->iqueue)) {
        chnAddFlagsI(sdp, CHN_INPUT_AVAILABLE);
      }
      if (iqPutI(&sdp->iqueue, b) < Q_OK) {
        chnAddFlagsI(sdp, SD_OVERRUN_ERROR);
      }
      osalSysUnlockFromISR();

      count--;
    }
  }

  if (((status & kLPUART_TxDataRegEmptyFlag) != 0U) &&
      ((enabled & kLPUART_TxDataRegEmptyInterruptEnable) != 0U)) {
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    uint32_t count = (uint32_t)FSL_FEATURE_LPUART_FIFO_SIZEn(base) -
                     (uint32_t)LPUART_GetTxFifoCount(base);
#else
    uint32_t count = 1U;
#endif

    while (count > 0U) {
      msg_t b;

      osalSysLockFromISR();
      b = oqGetI(&sdp->oqueue);
      osalSysUnlockFromISR();

      if (b < Q_OK) {
        LPUART_DisableInterrupts(base,
                                 kLPUART_TxDataRegEmptyInterruptEnable);

        osalSysLockFromISR();
        chnAddFlagsI(sdp, CHN_OUTPUT_EMPTY);
        osalSysUnlockFromISR();
        break;
      }

      LPUART_WriteByte(base, (uint8_t)b);
      count--;
    }
  }

  if ((status & MCX_LPUART_ERROR_FLAGS) != 0U) {
    set_error(sdp, status);
  }

  status &= kLPUART_AllClearFlags;
  if (status != 0U) {
    (void)LPUART_ClearStatusFlags(base, status);
  }
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if MCX_SERIAL_USE_LPUART0 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorCC) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD0);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if MCX_SERIAL_USE_LPUART1 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorD0) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD1);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if MCX_SERIAL_USE_LPUART2 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorD4) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD2);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if MCX_SERIAL_USE_LPUART3 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorD8) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD3);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if MCX_SERIAL_USE_LPUART4 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorDC) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD4);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if MCX_SERIAL_USE_LPUART5 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorE0) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD5);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if MCX_SERIAL_USE_LPUART6 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorE4) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD6);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if MCX_SERIAL_USE_LPUART7 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorE8) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD7);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if MCX_SERIAL_USE_LPUART8 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorEC) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD8);
  OSAL_IRQ_EPILOGUE();
}
#endif

#if MCX_SERIAL_USE_LPUART9 || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(VectorF0) {

  OSAL_IRQ_PROLOGUE();
  serve_interrupt(&SD9);
  OSAL_IRQ_EPILOGUE();
}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level serial driver initialization.
 *
 * @notapi
 */
void sd_lld_init(void) {

#if MCX_SERIAL_USE_LPUART0
  init_driver(&SD0, LPUART0);
#endif

#if MCX_SERIAL_USE_LPUART1
  init_driver(&SD1, LPUART1);
#endif

#if MCX_SERIAL_USE_LPUART2
  init_driver(&SD2, LPUART2);
#endif

#if MCX_SERIAL_USE_LPUART3
  init_driver(&SD3, LPUART3);
#endif

#if MCX_SERIAL_USE_LPUART4
  init_driver(&SD4, LPUART4);
#endif

#if MCX_SERIAL_USE_LPUART5
  init_driver(&SD5, LPUART5);
#endif

#if MCX_SERIAL_USE_LPUART6
  init_driver(&SD6, LPUART6);
#endif

#if MCX_SERIAL_USE_LPUART7
  init_driver(&SD7, LPUART7);
#endif

#if MCX_SERIAL_USE_LPUART8
  init_driver(&SD8, LPUART8);
#endif

#if MCX_SERIAL_USE_LPUART9
  init_driver(&SD9, LPUART9);
#endif
}

/**
 * @brief   Low level serial driver configuration and (re)start.
 *
 * @param[in] sdp       pointer to a @p SerialDriver object
 * @param[in] config    the architecture-dependent serial driver configuration.
 *                      If this parameter is set to @p NULL then a default
 *                      configuration is used.
 *
 * @notapi
 */
void sd_lld_start(SerialDriver *sdp, const SerialConfig *config) {

  if (config == NULL) {
    config = &default_config;
  }

  if (sdp->state == SD_STOP) {
#if MCX_SERIAL_USE_LPUART0
    if (sdp == &SD0) {
      enable_lpuart(LPUART0, LP_FLEXCOMM0_IRQn, 0U,
                    MCX_SERIAL_LPUART0_CLK_ATTACH,
                    kCLOCK_DivFlexcom0Clk,
                    MCX_SERIAL_LPUART0_CLK_DIV,
                    config);
    }
#endif

#if MCX_SERIAL_USE_LPUART1
    if (sdp == &SD1) {
      enable_lpuart(LPUART1, LP_FLEXCOMM1_IRQn, 1U,
                    MCX_SERIAL_LPUART1_CLK_ATTACH,
                    kCLOCK_DivFlexcom1Clk,
                    MCX_SERIAL_LPUART1_CLK_DIV,
                    config);
    }
#endif

#if MCX_SERIAL_USE_LPUART2
    if (sdp == &SD2) {
      enable_lpuart(LPUART2, LP_FLEXCOMM2_IRQn, 2U,
                    MCX_SERIAL_LPUART2_CLK_ATTACH,
                    kCLOCK_DivFlexcom2Clk,
                    MCX_SERIAL_LPUART2_CLK_DIV,
                    config);
    }
#endif

#if MCX_SERIAL_USE_LPUART3
    if (sdp == &SD3) {
      enable_lpuart(LPUART3, LP_FLEXCOMM3_IRQn, 3U,
                    MCX_SERIAL_LPUART3_CLK_ATTACH,
                    kCLOCK_DivFlexcom3Clk,
                    MCX_SERIAL_LPUART3_CLK_DIV,
                    config);
    }
#endif

#if MCX_SERIAL_USE_LPUART4
    if (sdp == &SD4) {
      enable_lpuart(LPUART4, LP_FLEXCOMM4_IRQn, 4U,
                    MCX_SERIAL_LPUART4_CLK_ATTACH,
                    kCLOCK_DivFlexcom4Clk,
                    MCX_SERIAL_LPUART4_CLK_DIV,
                    config);
    }
#endif

#if MCX_SERIAL_USE_LPUART5
    if (sdp == &SD5) {
      enable_lpuart(LPUART5, LP_FLEXCOMM5_IRQn, 5U,
                    MCX_SERIAL_LPUART5_CLK_ATTACH,
                    kCLOCK_DivFlexcom5Clk,
                    MCX_SERIAL_LPUART5_CLK_DIV,
                    config);
    }
#endif

#if MCX_SERIAL_USE_LPUART6
    if (sdp == &SD6) {
      enable_lpuart(LPUART6, LP_FLEXCOMM6_IRQn, 6U,
                    MCX_SERIAL_LPUART6_CLK_ATTACH,
                    kCLOCK_DivFlexcom6Clk,
                    MCX_SERIAL_LPUART6_CLK_DIV,
                    config);
    }
#endif

#if MCX_SERIAL_USE_LPUART7
    if (sdp == &SD7) {
      enable_lpuart(LPUART7, LP_FLEXCOMM7_IRQn, 7U,
                    MCX_SERIAL_LPUART7_CLK_ATTACH,
                    kCLOCK_DivFlexcom7Clk,
                    MCX_SERIAL_LPUART7_CLK_DIV,
                    config);
    }
#endif

#if MCX_SERIAL_USE_LPUART8
    if (sdp == &SD8) {
      enable_lpuart(LPUART8, LP_FLEXCOMM8_IRQn, 8U,
                    MCX_SERIAL_LPUART8_CLK_ATTACH,
                    kCLOCK_DivFlexcom8Clk,
                    MCX_SERIAL_LPUART8_CLK_DIV,
                    config);
    }
#endif

#if MCX_SERIAL_USE_LPUART9
    if (sdp == &SD9) {
      enable_lpuart(LPUART9, LP_FLEXCOMM9_IRQn, 9U,
                    MCX_SERIAL_LPUART9_CLK_ATTACH,
                    kCLOCK_DivFlexcom9Clk,
                    MCX_SERIAL_LPUART9_CLK_DIV,
                    config);
    }
#endif
  }
}

/**
 * @brief   Low level serial driver stop.
 * @details De-initializes the USART, stops the associated clock, resets the
 *          interrupt vector.
 *
 * @param[in] sdp       pointer to a @p SerialDriver object
 *
 * @notapi
 */
void sd_lld_stop(SerialDriver *sdp) {

  if (sdp->state == SD_READY) {
#if MCX_SERIAL_USE_LPUART0
    if (sdp == &SD0) {
      disable_lpuart(LPUART0, LP_FLEXCOMM0_IRQn);
    }
#endif

#if MCX_SERIAL_USE_LPUART1
    if (sdp == &SD1) {
      disable_lpuart(LPUART1, LP_FLEXCOMM1_IRQn);
    }
#endif

#if MCX_SERIAL_USE_LPUART2
    if (sdp == &SD2) {
      disable_lpuart(LPUART2, LP_FLEXCOMM2_IRQn);
    }
#endif

#if MCX_SERIAL_USE_LPUART3
    if (sdp == &SD3) {
      disable_lpuart(LPUART3, LP_FLEXCOMM3_IRQn);
    }
#endif

#if MCX_SERIAL_USE_LPUART4
    if (sdp == &SD4) {
      disable_lpuart(LPUART4, LP_FLEXCOMM4_IRQn);
    }
#endif

#if MCX_SERIAL_USE_LPUART5
    if (sdp == &SD5) {
      disable_lpuart(LPUART5, LP_FLEXCOMM5_IRQn);
    }
#endif

#if MCX_SERIAL_USE_LPUART6
    if (sdp == &SD6) {
      disable_lpuart(LPUART6, LP_FLEXCOMM6_IRQn);
    }
#endif

#if MCX_SERIAL_USE_LPUART7
    if (sdp == &SD7) {
      disable_lpuart(LPUART7, LP_FLEXCOMM7_IRQn);
    }
#endif

#if MCX_SERIAL_USE_LPUART8
    if (sdp == &SD8) {
      disable_lpuart(LPUART8, LP_FLEXCOMM8_IRQn);
    }
#endif

#if MCX_SERIAL_USE_LPUART9
    if (sdp == &SD9) {
      disable_lpuart(LPUART9, LP_FLEXCOMM9_IRQn);
    }
#endif
  }
}

#endif /* HAL_USE_SERIAL == TRUE */

/** @} */
