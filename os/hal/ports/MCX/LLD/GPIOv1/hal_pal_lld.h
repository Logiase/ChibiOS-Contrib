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
 * @file    hal_pal_lld.h
 * @brief   MCX GPIOv1 PAL subsystem low level driver header.
 *
 * @addtogroup PAL
 * @{
 */

#ifndef HAL_PAL_LLD_H
#define HAL_PAL_LLD_H

#if (HAL_USE_PAL == TRUE) || defined(__DOXYGEN__)

/*===========================================================================*/
/* Unsupported modes and specific modes                                      */
/*===========================================================================*/

/* Specifies palInit() without parameter, required until all platforms will
   be updated to the new style.*/
#define PAL_NEW_INIT

#if !defined(MCX_GPIO_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define MCX_GPIO_IRQ_PRIORITY           7
#endif

#undef PAL_MODE_RESET
#undef PAL_MODE_UNCONNECTED
#undef PAL_MODE_INPUT
#undef PAL_MODE_INPUT_PULLUP
#undef PAL_MODE_INPUT_PULLDOWN
#undef PAL_MODE_INPUT_ANALOG
#undef PAL_MODE_OUTPUT_PUSHPULL
#undef PAL_MODE_OUTPUT_OPENDRAIN

/**
 * @name    MCX-specific I/O mode flags
 * @{
 */
#define PAL_MCX_MODE_OUTPUT             (1UL << 31)
#define PAL_MCX_MODE_PCR_MASK           (PORT_PCR_PS_MASK  |                \
                                         PORT_PCR_PE_MASK  |                \
                                         PORT_PCR_PV_MASK  |                \
                                         PORT_PCR_SRE_MASK |                \
                                         PORT_PCR_PFE_MASK |                \
                                         PORT_PCR_ODE_MASK |                \
                                         PORT_PCR_DSE_MASK |                \
                                         PORT_PCR_MUX_MASK |                \
                                         PORT_PCR_IBE_MASK |                \
                                         PORT_PCR_INV_MASK)

#define PAL_MCX_MUX(n)                  PORT_PCR_MUX(n)
#define PAL_MCX_GPIO_PCR                (PORT_PCR_SRE(1U) | PAL_MCX_MUX(0U))
#define PAL_MCX_GPIO_INPUT_PCR          (PAL_MCX_GPIO_PCR | PORT_PCR_IBE(1U))
/** @} */

/**
 * @name    MCX-specific I/O event flags
 * @{
 */
#define PAL_MCX_GPIO_IRQC_DISABLED      0U
#define PAL_MCX_GPIO_IRQC_RISING        9U
#define PAL_MCX_GPIO_IRQC_FALLING       10U
#define PAL_MCX_GPIO_IRQC_BOTH          11U
/** @} */

/**
 * @name    Alternate functions
 * @{
 * @param[in] n         alternate function selector
 */
#define PAL_MODE_ALTERNATE(n)           (PORT_PCR_SRE(1U) | PAL_MCX_MUX(n))
/** @} */

/**
 * @name    Standard I/O mode flags
 * @{
 */
#define PAL_MODE_RESET                  0U
#define PAL_MODE_UNCONNECTED            (PAL_MCX_GPIO_INPUT_PCR |           \
                                         PORT_PCR_PE(1U) | PORT_PCR_PS(1U))
#define PAL_MODE_INPUT                  PAL_MCX_GPIO_INPUT_PCR
#define PAL_MODE_INPUT_PULLUP           (PAL_MCX_GPIO_INPUT_PCR |           \
                                         PORT_PCR_PE(1U) | PORT_PCR_PS(1U))
#define PAL_MODE_INPUT_PULLDOWN         (PAL_MCX_GPIO_INPUT_PCR |           \
                                         PORT_PCR_PE(1U))
#define PAL_MODE_INPUT_ANALOG           PAL_MCX_MUX(0U)
#define PAL_MODE_OUTPUT_PUSHPULL        (PAL_MCX_MODE_OUTPUT |              \
                                         PAL_MCX_GPIO_INPUT_PCR)
#define PAL_MODE_OUTPUT_OPENDRAIN       (PAL_MCX_MODE_OUTPUT |              \
                                         PAL_MCX_GPIO_INPUT_PCR |           \
                                         PORT_PCR_ODE(1U))
/** @} */

/*===========================================================================*/
/* I/O Ports Types and constants.                                            */
/*===========================================================================*/

/**
 * @name    Port related definitions
 * @{
 */
/**
 * @brief   Width, in bits, of an I/O port.
 */
#define PAL_IOPORTS_WIDTH               32U

/**
 * @brief   Whole port mask.
 * @details This macro specifies all the valid bits into a port.
 */
#define PAL_WHOLE_PORT                  ((ioportmask_t)0xFFFFFFFFU)
/** @} */

#define PAL_MCX_IOPORTS                 6U
#define PAL_MCX_EVENT_COUNT             (PAL_MCX_IOPORTS * PAL_IOPORTS_WIDTH)

/**
 * @name    Line handling macros
 * @{
 */
/**
 * @brief   Forms a line identifier.
 * @details A port/pad pair are encoded into an @p ioline_t type. The encoding
 *          of this type is platform-dependent.
 * @note    In this driver the pad number is encoded in the lower 5 bits of
 *          the GPIO address which are guaranteed to be zero.
 */
#define PAL_LINE(port, pad)                                                 \
  ((ioline_t)((uint32_t)(port) | (uint32_t)(pad)))

/**
 * @brief   Decodes a port identifier from a line identifier.
 */
#define PAL_PORT(line)                                                      \
  ((ioportid_t)((uint32_t)(line) & 0xFFFFFFE0U))

/**
 * @brief   Decodes a pad identifier from a line identifier.
 */
#define PAL_PAD(line)                                                       \
  ((iopadid_t)((uint32_t)(line) & 0x0000001FU))

/**
 * @brief   Value identifying an invalid line.
 */
#define PAL_NOLINE                      0U
/** @} */

/**
 * @brief   Generic I/O ports static initializer.
 * @details An instance of this structure must be passed to @p palInit() at
 *          system startup time in order to initialize the digital I/O
 *          subsystem. This represents only the initial setup, specific pads
 *          or whole ports can be reprogrammed at later time.
 * @note    Implementations may extend this structure to contain more,
 *          architecture dependent, fields.
 */
typedef struct {

} PALConfig;

/**
 * @brief   Digital I/O port sized unsigned type.
 */
typedef uint32_t ioportmask_t;

/**
 * @brief   Digital I/O modes.
 */
typedef uint32_t iomode_t;

/**
 * @brief   Type of an I/O line.
 */
typedef uint32_t ioline_t;

/**
 * @brief   Type of an event mode.
 */
typedef uint32_t ioeventmode_t;

/**
 * @brief   Port Identifier.
 * @details This type can be a scalar or some kind of pointer, do not make
 *          any assumption about it, use the provided macros when populating
 *          variables of this type.
 */
typedef GPIO_Type *ioportid_t;

/**
 * @brief   Type of an pad identifier.
 */
typedef uint32_t iopadid_t;

/*===========================================================================*/
/* I/O Ports Identifiers.                                                    */
/*===========================================================================*/

#define IOPORT1                         GPIO0
#define IOPORT2                         GPIO1
#define IOPORT3                         GPIO2
#define IOPORT4                         GPIO3
#define IOPORT5                         GPIO4
#define IOPORT6                         GPIO5

/*===========================================================================*/
/* Implementation, some of the following macros could be implemented as      */
/* functions, if so please put them in pal_lld.c.                            */
/*===========================================================================*/

/**
 * @brief   Low level PAL subsystem initialization.
 *
 * @notapi
 */
#define pal_lld_init()                  _pal_lld_init()

/**
 * @brief   Reads the physical I/O port states.
 *
 * @param[in] port      port identifier
 * @return              The port bits.
 *
 * @notapi
 */
#define pal_lld_readport(port)          ((ioportmask_t)((port)->PDIR))

/**
 * @brief   Reads the output latch.
 * @details The purpose of this function is to read back the latched output
 *          value.
 *
 * @param[in] port      port identifier
 * @return              The latched logical states.
 *
 * @notapi
 */
#define pal_lld_readlatch(port)         ((ioportmask_t)((port)->PDOR))

/**
 * @brief   Writes a bits mask on a I/O port.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be written on the specified port
 *
 * @notapi
 */
#define pal_lld_writeport(port, bits)                                       \
  do {                                                                      \
    (port)->PDOR = (ioportmask_t)(bits);                                    \
  } while (false)

/**
 * @brief   Sets a bits mask on a I/O port.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be ORed on the specified port
 *
 * @notapi
 */
#define pal_lld_setport(port, bits)                                         \
  do {                                                                      \
    (port)->PSOR = (ioportmask_t)(bits);                                    \
  } while (false)

/**
 * @brief   Clears a bits mask on a I/O port.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be cleared on the specified port
 *
 * @notapi
 */
#define pal_lld_clearport(port, bits)                                       \
  do {                                                                      \
    (port)->PCOR = (ioportmask_t)(bits);                                    \
  } while (false)

/**
 * @brief   Toggles a bits mask on a I/O port.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] bits      bits to be XORed on the specified port
 *
 * @notapi
 */
#define pal_lld_toggleport(port, bits)                                      \
  do {                                                                      \
    (port)->PTOR = (ioportmask_t)(bits);                                    \
  } while (false)

/**
 * @brief   Reads a group of bits.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @return              The group logical states.
 *
 * @notapi
 */
#define pal_lld_readgroup(port, mask, offset)                               \
  ((pal_lld_readport(port) >> (offset)) & (mask))

/**
 * @brief   Writes a group of bits.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @param[in] bits      bits to be written. Values exceeding the group width
 *                      are masked.
 *
 * @notapi
 */
#define pal_lld_writegroup(port, mask, offset, bits)                        \
  do {                                                                      \
    ioportmask_t __mask = (ioportmask_t)((mask) << (offset));               \
    ioportmask_t __bits = (ioportmask_t)(((bits) & (mask)) << (offset));    \
    (port)->PDOR = ((port)->PDOR & ~__mask) | __bits;                       \
  } while (false)

/**
 * @brief   Pads group mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 * @note    Programming an unknown or unsupported mode is silently ignored.
 *
 * @param[in] port      port identifier
 * @param[in] mask      group mask
 * @param[in] offset    group bit offset within the port
 * @param[in] mode      group mode
 *
 * @notapi
 */
#define pal_lld_setgroupmode(port, mask, offset, mode)                      \
  _pal_lld_setgroupmode(port, (ioportmask_t)((mask) << (offset)), mode)

/**
 * @brief   Reads a logical state from an I/O pad.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @return              The logical state.
 * @retval PAL_LOW      low logical state.
 * @retval PAL_HIGH     high logical state.
 *
 * @notapi
 */
#define pal_lld_readpad(port, pad)                                          \
  (((port)->PDIR & PAL_PORT_BIT(pad)) != 0U ? PAL_HIGH : PAL_LOW)

/**
 * @brief   Writes a logical state on an output pad.
 * @note    This function is not meant to be invoked directly by the
 *          application  code.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @param[in] bit       logical value, the value must be @p PAL_LOW or
 *                      @p PAL_HIGH
 *
 * @notapi
 */
#define pal_lld_writepad(port, pad, bit)                                    \
  do {                                                                      \
    if ((bit) != PAL_LOW) {                                                 \
      (port)->PSOR = PAL_PORT_BIT(pad);                                     \
    }                                                                       \
    else {                                                                  \
      (port)->PCOR = PAL_PORT_BIT(pad);                                     \
    }                                                                       \
  } while (false)

/**
 * @brief   Sets a pad logical state to @p PAL_HIGH.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_setpad(port, pad)                                           \
  do {                                                                      \
    (port)->PSOR = PAL_PORT_BIT(pad);                                       \
  } while (false)

/**
 * @brief   Clears a pad logical state to @p PAL_LOW.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_clearpad(port, pad)                                         \
  do {                                                                      \
    (port)->PCOR = PAL_PORT_BIT(pad);                                       \
  } while (false)

/**
 * @brief   Toggles a pad logical state.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_togglepad(port, pad)                                        \
  do {                                                                      \
    (port)->PTOR = PAL_PORT_BIT(pad);                                       \
  } while (false)

/**
 * @brief   Pad mode setup.
 * @details This function programs a pad with the specified mode.
 * @note    The @ref PAL provides a default software implementation of this
 *          functionality, implement this function if can optimize it by using
 *          special hardware functionalities or special coding.
 * @note    Programming an unknown or unsupported mode is silently ignored.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @param[in] mode      pad mode
 *
 * @notapi
 */
#define pal_lld_setpadmode(port, pad, mode)                                 \
  _pal_lld_setgroupmode(port, PAL_PORT_BIT(pad), mode)

#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE) ||                \
    defined(__DOXYGEN__)
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
#define pal_lld_enablepadevent(port, pad, mode)                             \
  _pal_lld_enablepadevent(port, pad, mode)

/**
 * @brief   Pad event disable.
 * @details This function disables previously programmed event callbacks.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_disablepadevent(port, pad)                                  \
  _pal_lld_disablepadevent(port, pad)

/**
 * @brief   Returns a PAL event structure associated to a pad.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 *
 * @notapi
 */
#define pal_lld_get_pad_event(port, pad)                                    \
  _pal_lld_get_pad_event(port, pad)

/**
 * @brief   Returns a PAL event structure associated to a line.
 *
 * @param[in] line      line identifier
 *
 * @notapi
 */
#define pal_lld_get_line_event(line)                                        \
  _pal_lld_get_pad_event(PAL_PORT(line), PAL_PAD(line))

/**
 * @brief   Pad event enable check.
 *
 * @param[in] port      port identifier
 * @param[in] pad       pad number within the port
 * @return              Pad event status.
 * @retval false        if the pad event is disabled.
 * @retval true         if the pad event is enabled.
 *
 * @notapi
 */
#define pal_lld_ispadeventenabled(port, pad)                                \
  (((port)->ICR[pad] & GPIO_ICR_IRQC_MASK) != 0U)

extern palevent_t _pal_events[PAL_MCX_EVENT_COUNT];
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void _pal_lld_init(void);
  void _pal_lld_setgroupmode(ioportid_t port,
                             ioportmask_t mask,
                             iomode_t mode);
#if (PAL_USE_WAIT == TRUE) || (PAL_USE_CALLBACKS == TRUE) ||                \
    defined(__DOXYGEN__)
  void _pal_lld_enablepadevent(ioportid_t port,
                               iopadid_t pad,
                               ioeventmode_t mode);
  void _pal_lld_disablepadevent(ioportid_t port, iopadid_t pad);
  palevent_t *_pal_lld_get_pad_event(ioportid_t port, iopadid_t pad);
#endif
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_PAL == TRUE */

#endif /* HAL_PAL_LLD_H */

/** @} */
