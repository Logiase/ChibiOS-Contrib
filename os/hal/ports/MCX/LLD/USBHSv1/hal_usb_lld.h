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
 * @file    hal_usb_lld.h
 * @brief   MCX USBHS subsystem low level driver header.
 *
 * @addtogroup USB
 * @{
 */

#ifndef HAL_USB_LLD_H
#define HAL_USB_LLD_H

#if (HAL_USE_USB == TRUE) || defined(__DOXYGEN__)

#include "fsl_device_registers.h"
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_dci.h"
#include "usb_device_ehci.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief   Maximum endpoint address.
 */
#define USB_MAX_ENDPOINTS                   7

#if USB_DEVICE_CONFIG_ENDPOINTS != (USB_MAX_ENDPOINTS + 1)
#error "Mismatch: USB_DEVICE_CONFIG_ENDPOINTS != USB_MAX_ENDPOINTS + 1"
#endif

/**
 * @brief   Status stage handling method.
 */
#define USB_EP0_STATUS_STAGE                USB_EP0_STATUS_STAGE_SW

/**
 * @brief   The address is changed before the status stage.
 */
#define USB_SET_ADDRESS_MODE                USB_EARLY_SET_ADDRESS

/**
 * @brief   Method for set address acknowledge.
 */
#define USB_SET_ADDRESS_ACK_HANDLING        USB_SET_ADDRESS_ACK_SW

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    MCX configuration options
 * @{
 */
/**
 * @brief   USB driver enable switch.
 * @details If set to @p TRUE the support for USB1 is included.
 * @note    The default is @p FALSE.
 */
#if !defined(MCX_USB_USE_USB1) || defined(__DOXYGEN__)
#define MCX_USB_USE_USB1                    FALSE
#endif

/**
 * @brief   USB1 interrupt priority level setting.
 */
#if !defined(MCX_USB_USB1_IRQ_PRIORITY) || defined(__DOXYGEN__)
#define MCX_USB_USB1_IRQ_PRIORITY           3
#endif

/**
 * @brief   USBHS PHY calibration defaults.
 */
#if !defined(MCX_USB_PHY_D_CAL) || defined(__DOXYGEN__)
#define MCX_USB_PHY_D_CAL                   0x04U
#endif

#if !defined(MCX_USB_PHY_TXCAL45DP) || defined(__DOXYGEN__)
#define MCX_USB_PHY_TXCAL45DP               0x07U
#endif

#if !defined(MCX_USB_PHY_TXCAL45DM) || defined(__DOXYGEN__)
#define MCX_USB_PHY_TXCAL45DM               0x07U
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if (MCX_USB_USE_USB1 == TRUE) && !OSAL_IRQ_IS_VALID_PRIORITY(MCX_USB_USB1_IRQ_PRIORITY)
#error "Invalid IRQ priority assigned to MCX_USB_USB1_IRQ_PRIORITY"
#endif

#if (MCX_USB_USE_USB1 == TRUE) && !defined(USBHS_IRQS)
#error "USBHS_IRQS not defined"
#endif

#if (MCX_USB_USE_USB1 == FALSE)
#error "USB driver activated but no USB peripheral assigned"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Type of an IN endpoint state structure.
 */
typedef struct {
  /**
   * @brief   Requested transmit transfer size.
   */
  size_t                        txsize;
  /**
   * @brief   Transmitted bytes so far.
   */
  size_t                        txcnt;
  /**
   * @brief   Pointer to the transmission linear buffer.
   */
  const uint8_t                 *txbuf;
#if (USB_USE_WAIT == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Waiting thread.
   */
  thread_reference_t            thread;
#endif
    /* End of the mandatory fields.*/
} USBInEndpointState;

/**
 * @brief   Type of an OUT endpoint state structure.
 */
typedef struct {
  /**
   * @brief   Requested receive transfer size.
   */
  size_t                        rxsize;
  /**
   * @brief   Received bytes so far.
   */
  size_t                        rxcnt;
  /**
   * @brief   Pointer to the receive linear buffer.
   */
  uint8_t                       *rxbuf;
#if (USB_USE_WAIT == TRUE) || defined(__DOXYGEN__)
  /**
   * @brief   Waiting thread.
   */
  thread_reference_t            thread;
#endif
  /* End of the mandatory fields.*/
} USBOutEndpointState;

/**
 * @brief   Type of an USB endpoint configuration structure.
 * @note    Platform specific restrictions may apply to endpoints.
 */
typedef struct {
  /**
   * @brief   Type and mode of the endpoint.
   */
  uint32_t                      ep_mode;
  /**
   * @brief   Setup packet notification callback.
   */
  usbepcallback_t               setup_cb;
  /**
   * @brief   IN endpoint notification callback.
   */
  usbepcallback_t               in_cb;
  /**
   * @brief   OUT endpoint notification callback.
   */
  usbepcallback_t               out_cb;
  /**
   * @brief   IN endpoint maximum packet size.
   */
  uint16_t                      in_maxsize;
  /**
   * @brief   OUT endpoint maximum packet size.
   */
  uint16_t                      out_maxsize;
  /**
   * @brief   @p USBEndpointState associated to the IN endpoint.
   */
  USBInEndpointState            *in_state;
  /**
   * @brief   @p USBEndpointState associated to the OUT endpoint.
   */
  USBOutEndpointState           *out_state;
  /* End of the mandatory fields.*/
} USBEndpointConfig;

/**
 * @brief   Type of an USB driver configuration structure.
 */
typedef struct {
  /**
   * @brief   USB events callback.
   */
  usbeventcb_t                  event_cb;
  /**
   * @brief   Device GET_DESCRIPTOR request callback.
   */
  usbgetdescriptor_t            get_descriptor_cb;
  /**
   * @brief   Requests hook callback.
   */
  usbreqhandler_t               requests_hook_cb;
  /**
   * @brief   Start Of Frame callback.
   */
  usbcallback_t                 sof_cb;
  /* End of the mandatory fields.*/
} USBConfig;

/**
 * @brief   Structure representing an USB driver.
 */
struct USBDriver {
  /**
   * @brief   Driver state.
   */
  usbstate_t                    state;
  /**
   * @brief   Current configuration data.
   */
  const USBConfig               *config;
  /**
   * @brief   Bit map of the transmitting IN endpoints.
   */
  uint16_t                      transmitting;
  /**
   * @brief   Bit map of the receiving OUT endpoints.
   */
  uint16_t                      receiving;
  /**
   * @brief   Active endpoints configurations.
   */
  const USBEndpointConfig       *epc[USB_MAX_ENDPOINTS + 1];
  /**
   * @brief   Fields available to user, it can be used to associate an
   *          application-defined handler to an IN endpoint.
   */
  void                          *in_params[USB_MAX_ENDPOINTS];
  /**
   * @brief   Fields available to user, it can be used to associate an
   *          application-defined handler to an OUT endpoint.
   */
  void                          *out_params[USB_MAX_ENDPOINTS];
  /**
   * @brief   Endpoint 0 state.
   */
  usbep0state_t                 ep0state;
  /**
   * @brief   Next position in the buffer to be transferred through endpoint 0.
   */
  uint8_t                       *ep0next;
  /**
   * @brief   Number of bytes yet to be transferred through endpoint 0.
   */
  size_t                        ep0n;
  /**
   * @brief   Endpoint 0 end transaction callback.
   */
  usbcallback_t                 ep0endcb;
  /**
   * @brief   Setup packet buffer.
   */
  uint8_t                       setup[8];
  /**
   * @brief   Current USB device status.
   */
  uint16_t                      status;
  /**
   * @brief   Assigned USB address.
   */
  uint8_t                       address;
  /**
   * @brief   Current USB device configuration.
   */
  uint8_t                       configuration;
  /**
   * @brief   State of the driver when a suspend happened.
   */
  usbstate_t                    saved_state;
#if defined(USB_DRIVER_EXT_FIELDS)
  USB_DRIVER_EXT_FIELDS
#endif
  /* End of the mandatory fields.*/
  /**
   * @brief   Bitmap of initialized IN endpoints.
   */
  uint16_t                      in_enabled;
  /**
   * @brief   Bitmap of initialized OUT endpoints.
   */
  uint16_t                      out_enabled;
  /**
   * @brief   Bitmap of IN endpoints waiting for a submit retry.
   */
  uint16_t                      in_pending;
  /**
   * @brief   Bitmap of OUT endpoints waiting for a submit retry.
   */
  uint16_t                      out_pending;
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Returns the exact size of a receive transaction.
 */
#define usb_lld_get_transaction_size(usbp, ep)                              \
  ((usbp)->epc[ep]->out_state->rxcnt)

/**
 * @brief   Connects the USB device.
 */
#define usb_lld_connect_bus(usbp) do {                                      \
  (void)(usbp);                                                             \
  (void)USB_DeviceRun(mcx_usb_device_handle);                               \
} while (false)

/**
 * @brief   Disconnect the USB device.
 */
#define usb_lld_disconnect_bus(usbp) do {                                   \
  (void)(usbp);                                                             \
  (void)USB_DeviceStop(mcx_usb_device_handle);                              \
} while (false)

/**
 * @brief   Start of host wake-up procedure.
 */
#define usb_lld_wakeup_host(usbp) do {                                      \
  (void)(usbp);                                                             \
} while (false)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if (MCX_USB_USE_USB1 == TRUE) && !defined(__DOXYGEN__)
extern USBDriver USBD1;
extern usb_device_handle mcx_usb_device_handle;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  void usb_lld_init(void);
  void usb_lld_start(USBDriver *usbp);
  void usb_lld_stop(USBDriver *usbp);
  void usb_lld_reset(USBDriver *usbp);
  void usb_lld_set_address(USBDriver *usbp);
  void usb_lld_init_endpoint(USBDriver *usbp, usbep_t ep);
  void usb_lld_disable_endpoints(USBDriver *usbp);
  usbepstatus_t usb_lld_get_status_in(USBDriver *usbp, usbep_t ep);
  usbepstatus_t usb_lld_get_status_out(USBDriver *usbp, usbep_t ep);
  void usb_lld_read_setup(USBDriver *usbp, usbep_t ep, uint8_t *buf);
  void usb_lld_prepare_receive(USBDriver *usbp, usbep_t ep);
  void usb_lld_prepare_transmit(USBDriver *usbp, usbep_t ep);
  void usb_lld_start_out(USBDriver *usbp, usbep_t ep);
  void usb_lld_start_in(USBDriver *usbp, usbep_t ep);
  void usb_lld_stall_out(USBDriver *usbp, usbep_t ep);
  void usb_lld_stall_in(USBDriver *usbp, usbep_t ep);
  void usb_lld_clear_out(USBDriver *usbp, usbep_t ep);
  void usb_lld_clear_in(USBDriver *usbp, usbep_t ep);
  uint32_t usb_lld_get_frame_number(USBDriver *usbp);
#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_USB == TRUE */

#endif /* HAL_USB_LLD_H */

/** @} */
