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
 * @file    hal_pal_lld.c
 * @brief   MCX GPIOv1 PAL subsystem low level driver source.
 *
 * @addtogroup PAL
 * @{
 */

#include "hal.h"

#if (HAL_USE_PAL == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define MCX_AHBCLKCTRL0_PORT_MASK                                           \
  (SYSCON_AHBCLKCTRL0_PORT0_MASK | SYSCON_AHBCLKCTRL0_PORT1_MASK |          \
   SYSCON_AHBCLKCTRL0_PORT2_MASK | SYSCON_AHBCLKCTRL0_PORT3_MASK |          \
   SYSCON_AHBCLKCTRL0_PORT4_MASK)

#define MCX_AHBCLKCTRL0_GPIO_MASK                                           \
  (SYSCON_AHBCLKCTRL0_GPIO0_MASK | SYSCON_AHBCLKCTRL0_GPIO1_MASK |          \
   SYSCON_AHBCLKCTRL0_GPIO2_MASK | SYSCON_AHBCLKCTRL0_GPIO3_MASK |          \
   SYSCON_AHBCLKCTRL0_GPIO4_MASK)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE) || defined(__DOXYGEN__)
palevent_t _pal_events[PAL_MCX_EVENT_COUNT];
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static PORT_Type *gpio_to_port(GPIO_Type *gpiop) {

  if (gpiop == GPIO0) {
    return PORT0;
  }
  if (gpiop == GPIO1) {
    return PORT1;
  }
  if (gpiop == GPIO2) {
    return PORT2;
  }
  if (gpiop == GPIO3) {
    return PORT3;
  }
  if (gpiop == GPIO4) {
    return PORT4;
  }
  if (gpiop == GPIO5) {
    return PORT5;
  }

#if defined(GPIO0_ALIAS1)
  if (gpiop == GPIO0_ALIAS1) {
    return PORT0;
  }
  if (gpiop == GPIO1_ALIAS1) {
    return PORT1;
  }
  if (gpiop == GPIO2_ALIAS1) {
    return PORT2;
  }
  if (gpiop == GPIO3_ALIAS1) {
    return PORT3;
  }
  if (gpiop == GPIO4_ALIAS1) {
    return PORT4;
  }
  if (gpiop == GPIO5_ALIAS1) {
    return PORT5;
  }
#endif

  return NULL;
}

static uint32_t gpio_to_index(GPIO_Type *gpiop) {

  if (gpiop == GPIO0) {
    return 0U;
  }
  if (gpiop == GPIO1) {
    return 1U;
  }
  if (gpiop == GPIO2) {
    return 2U;
  }
  if (gpiop == GPIO3) {
    return 3U;
  }
  if (gpiop == GPIO4) {
    return 4U;
  }
  if (gpiop == GPIO5) {
    return 5U;
  }

#if defined(GPIO0_ALIAS1)
  if (gpiop == GPIO0_ALIAS1) {
    return 0U;
  }
  if (gpiop == GPIO1_ALIAS1) {
    return 1U;
  }
  if (gpiop == GPIO2_ALIAS1) {
    return 2U;
  }
  if (gpiop == GPIO3_ALIAS1) {
    return 3U;
  }
  if (gpiop == GPIO4_ALIAS1) {
    return 4U;
  }
  if (gpiop == GPIO5_ALIAS1) {
    return 5U;
  }
#endif

  return PAL_MCX_IOPORTS;
}

#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE) || defined(__DOXYGEN__)
static uint32_t event_index(GPIO_Type *gpiop, uint32_t pad) {

  return (gpio_to_index(gpiop) * PAL_IOPORTS_WIDTH) + pad;
}

static void gpio_isr(GPIO_Type *gpiop) {
  uint32_t flags;
  uint32_t portidx;

  flags = gpiop->ISFR[0];
  gpiop->ISFR[0] = flags;

  portidx = gpio_to_index(gpiop) * PAL_IOPORTS_WIDTH;

  while (flags != 0U) {
    uint32_t pad = (uint32_t)__builtin_ctz(flags);

    _pal_isr_code(portidx + pad);
    flags &= ~PAL_PORT_BIT(pad);
  }
}

static uint32_t event_mode_to_irqc(ioeventmode_t mode) {

  switch (mode & PAL_EVENT_MODE_EDGES_MASK) {
  case PAL_EVENT_MODE_RISING_EDGE:
    return PAL_MCX_GPIO_IRQC_RISING;
  case PAL_EVENT_MODE_FALLING_EDGE:
    return PAL_MCX_GPIO_IRQC_FALLING;
  case PAL_EVENT_MODE_BOTH_EDGES:
    return PAL_MCX_GPIO_IRQC_BOTH;
  default:
    return PAL_MCX_GPIO_IRQC_DISABLED;
  }
}
#endif

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE) || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(Vector84) {

  OSAL_IRQ_PROLOGUE();
  gpio_isr(GPIO0);
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(Vector8C) {

  OSAL_IRQ_PROLOGUE();
  gpio_isr(GPIO1);
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(Vector94) {

  OSAL_IRQ_PROLOGUE();
  gpio_isr(GPIO2);
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(Vector9C) {

  OSAL_IRQ_PROLOGUE();
  gpio_isr(GPIO3);
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(VectorA4) {

  OSAL_IRQ_PROLOGUE();
  gpio_isr(GPIO4);
  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(VectorAC) {

  OSAL_IRQ_PROLOGUE();
  gpio_isr(GPIO5);
  OSAL_IRQ_EPILOGUE();
}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   PAL driver initialization.
 *
 * @notapi
 */
void _pal_lld_init(void) {

  CLOCK_EnableClock(kCLOCK_Port0);
  CLOCK_EnableClock(kCLOCK_Port1);
  CLOCK_EnableClock(kCLOCK_Port2);
  CLOCK_EnableClock(kCLOCK_Port3);
  CLOCK_EnableClock(kCLOCK_Port4);
  CLOCK_EnableClock(kCLOCK_Gpio0);
  CLOCK_EnableClock(kCLOCK_Gpio1);
  CLOCK_EnableClock(kCLOCK_Gpio2);
  CLOCK_EnableClock(kCLOCK_Gpio3);
  CLOCK_EnableClock(kCLOCK_Gpio4);

#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE) || defined(__DOXYGEN__)
  for (uint32_t i = 0U; i < PAL_MCX_EVENT_COUNT; i++) {
    _pal_init_event(i);
  }

  GPIO0->ISFR[0] = PAL_WHOLE_PORT;
  GPIO1->ISFR[0] = PAL_WHOLE_PORT;
  GPIO2->ISFR[0] = PAL_WHOLE_PORT;
  GPIO3->ISFR[0] = PAL_WHOLE_PORT;
  GPIO4->ISFR[0] = PAL_WHOLE_PORT;

  nvicEnableVector(GPIO00_IRQn, MCX_GPIO_IRQ_PRIORITY);
  nvicEnableVector(GPIO10_IRQn, MCX_GPIO_IRQ_PRIORITY);
  nvicEnableVector(GPIO20_IRQn, MCX_GPIO_IRQ_PRIORITY);
  nvicEnableVector(GPIO30_IRQn, MCX_GPIO_IRQ_PRIORITY);
  nvicEnableVector(GPIO40_IRQn, MCX_GPIO_IRQ_PRIORITY);
  nvicEnableVector(GPIO50_IRQn, MCX_GPIO_IRQ_PRIORITY);
#endif
}

/**
 * @brief   Pads mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 *
 * @param[in] port      the port identifier
 * @param[in] mask      the group mask
 * @param[in] mode      the mode
 *
 * @notapi
 */
void _pal_lld_setgroupmode(ioportid_t port,
                           ioportmask_t mask,
                           iomode_t mode) {
  PORT_Type *portp;
  uint32_t pcr;
  uint32_t pad;

  portp = gpio_to_port(port);
  if (portp == NULL) {
    return;
  }

  pcr = mode & PAL_MCX_MODE_PCR_MASK;

  for (pad = 0U; pad < PAL_IOPORTS_WIDTH; pad++) {
    if ((mask & PAL_PORT_BIT(pad)) != 0U) {
      portp->PCR[pad] = pcr;
    }
  }

  if ((mode & PAL_MCX_MODE_OUTPUT) != 0U) {
    port->PDDR |= mask;
  }
  else {
    port->PDDR &= ~mask;
  }

  if ((pcr & PORT_PCR_IBE_MASK) != 0U) {
    port->PIDR &= ~mask;
  }
  else {
    port->PIDR |= mask;
  }
}

#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE) || defined(__DOXYGEN__)
/**
 * @brief   Pad event enable.
 * @note    Programming an unknown or unsupported mode is silently ignored.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @param[in] mode      pad event mode
 *
 * @notapi
 */
void _pal_lld_enablepadevent(ioportid_t port,
                             iopadid_t pad,
                             ioeventmode_t mode) {
  uint32_t irqc;

  osalDbgCheck(pad < PAL_IOPORTS_WIDTH);

  irqc = event_mode_to_irqc(mode);
  port->ISFR[0] = PAL_PORT_BIT(pad);
  port->ICR[pad] = (port->ICR[pad] & ~(GPIO_ICR_IRQC_MASK |
                                       GPIO_ICR_IRQS_MASK |
                                       GPIO_ICR_ISF_MASK)) |
                   GPIO_ICR_IRQC(irqc) |
                   GPIO_ICR_IRQS(0U);
}

/**
 * @brief   Pad event disable.
 * @details This function disables previously programmed event callbacks.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
void _pal_lld_disablepadevent(ioportid_t port, iopadid_t pad) {

  osalDbgCheck(pad < PAL_IOPORTS_WIDTH);

  port->ICR[pad] &= ~(GPIO_ICR_IRQC_MASK | GPIO_ICR_IRQS_MASK);
  port->ISFR[0] = PAL_PORT_BIT(pad);

  _pal_clear_event(event_index(port, pad));
}

/**
 * @brief   Returns a PAL event structure associated to a pad.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
palevent_t *_pal_lld_get_pad_event(ioportid_t port, iopadid_t pad) {

  osalDbgCheck((gpio_to_index(port) < PAL_MCX_IOPORTS) &&
               (pad < PAL_IOPORTS_WIDTH));

  return &_pal_events[event_index(port, pad)];
}
#endif

#endif /* HAL_USE_PAL == TRUE */

/** @} */
