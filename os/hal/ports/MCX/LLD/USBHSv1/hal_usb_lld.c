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
 * @file    hal_usb_lld.c
 * @brief   MCX USBHS subsystem low level driver source.
 *
 * @addtogroup USB
 * @{
 */

#include <string.h>

#include "hal.h"

#if (HAL_USE_USB == TRUE) || defined(__DOXYGEN__)

#include "fsl_clock.h"
#include "fsl_reset.h"
#include "usb_phy.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define MCX_USB_CONTROLLER_ID           ((uint8_t)kUSB_ControllerEhci0)
#define MCX_USB_EP_MASK(ep)             ((uint16_t)((uint16_t)1U << (ep)))

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   USB1 driver identifier.
 */
#if (MCX_USB_USE_USB1 == TRUE) || defined(__DOXYGEN__)
USBDriver USBD1;
usb_device_handle mcx_usb_device_handle;
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/**
 * @brief   EP0 state.
 * @note    It is an union because IN and OUT endpoints are never used at the
 *          same time for EP0.
 */
static union {
  /**
   * @brief   IN EP0 state.
   */
  USBInEndpointState in;
  /**
   * @brief   OUT EP0 state.
   */
  USBOutEndpointState out;
} ep0_state;

/**
 * @brief   EP0 initialization structure.
 */
static const USBEndpointConfig ep0config = {
  USB_EP_MODE_TYPE_CTRL,
  _usb_ep0setup,
  _usb_ep0in,
  _usb_ep0out,
  USB_CONTROL_MAX_PACKET_SIZE,
  USB_CONTROL_MAX_PACKET_SIZE,
  &ep0_state.in,
  &ep0_state.out
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static uint8_t epaddr(usbep_t ep, uint8_t direction) {

  return (uint8_t)(ep | (uint8_t)(direction << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT));
}

static USBDriver *driver_from_handle(usb_device_handle handle) {

  (void)handle;
  return &USBD1;
}

static void mcx_usb_clock_init(void) {

  SPC0->ACTIVE_VDELAY = 0x0500;
  SPC0->ACTIVE_CFG &= ~SPC_ACTIVE_CFG_CORELDO_VDD_DS_MASK;
  SPC0->ACTIVE_CFG |= SPC_ACTIVE_CFG_DCDC_VDD_LVL(0x3) |
                      SPC_ACTIVE_CFG_CORELDO_VDD_LVL(0x3) |
                      SPC_ACTIVE_CFG_SYSLDO_VDD_DS_MASK |
                      SPC_ACTIVE_CFG_DCDC_VDD_DS(0x2);
  while (SPC0->SC & SPC_SC_BUSY_MASK)
    ;
  if (0 == (SCG0->LDOCSR & SCG_LDOCSR_LDOEN_MASK))
  {
    SCG0->TRIM_LOCK = 0x5a5a0001U;
    SCG0->LDOCSR |= SCG_LDOCSR_LDOEN_MASK;
    while (0 == (SCG0->LDOCSR & SCG_LDOCSR_VOUT_OK_MASK))
      ;
  }
  SYSCON->AHBCLKCTRLSET[2] |= SYSCON_AHBCLKCTRL2_USB_HS_MASK |
                              SYSCON_AHBCLKCTRL2_USB_HS_PHY_MASK;
  SCG0->SOSCCFG &= ~(SCG_SOSCCFG_RANGE_MASK | SCG_SOSCCFG_EREFS_MASK);
  SCG0->SOSCCFG = (1U << SCG_SOSCCFG_RANGE_SHIFT) | 
                  (1U << SCG_SOSCCFG_EREFS_SHIFT);
  SCG0->SOSCCSR |= SCG_SOSCCSR_SOSCEN_MASK;
  while (true) {
    if (SCG0->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)
      break;
  }
  SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_CLKIN_ENA_MASK |
                        SYSCON_CLOCK_CTRL_CLKIN_ENA_FM_USBH_LPT_MASK;
  CLOCK_EnableClock(kCLOCK_UsbHs);
  CLOCK_EnableClock(kCLOCK_UsbHsPhy);
  CLOCK_EnableUsbhsPhyPllClock(kCLOCK_Usbphy480M, 24000000U);
  CLOCK_EnableUsbhsClock();
}

static uint8_t mcx_usb_transfer_type(uint32_t ep_mode) {

  switch (ep_mode & USB_EP_MODE_TYPE) {
  case USB_EP_MODE_TYPE_CTRL:
    return USB_ENDPOINT_CONTROL;
  case USB_EP_MODE_TYPE_ISOC:
    return USB_ENDPOINT_ISOCHRONOUS;
  case USB_EP_MODE_TYPE_BULK:
    return USB_ENDPOINT_BULK;
  case USB_EP_MODE_TYPE_INTR:
  default:
    return USB_ENDPOINT_INTERRUPT;
  }
}

static usb_status_t mcx_usb_device_callback(usb_device_handle handle,
                                            uint32_t callback_event,
                                            void *event_param) {
  USBDriver *usbp = driver_from_handle(handle);

  (void)event_param;

  switch (callback_event) {
  case kUSB_DeviceEventBusReset:
    _usb_reset(usbp);
    break;

  case kUSB_DeviceEventSuspend:
    _usb_suspend(usbp);
    break;

  case kUSB_DeviceEventResume:
    _usb_wakeup(usbp);
    break;

#if (USB_DEVICE_CONFIG_SOF_NOTIFICATIONS > 0U)
  case kUSB_DeviceEventSOF:
    _usb_isr_invoke_sof_cb(usbp);
    break;
#endif

  default:
    break;
  }

  return kStatus_USB_Success;
}

static usb_status_t mcx_usb_endpoint_callback(usb_device_handle handle,
                                              usb_device_endpoint_callback_message_struct_t *message,
                                              void *callback_param,
                                              uint8_t direction) {
  USBDriver *usbp = driver_from_handle(handle);
  usbep_t ep = (usbep_t)((uintptr_t)callback_param & 0xFFU);
  const USBEndpointConfig *epcp;

  if ((message == NULL) || (ep > (usbep_t)USB_MAX_ENDPOINTS)) {
    return kStatus_USB_InvalidParameter;
  }

  epcp = usbp->epc[ep];
  if (epcp == NULL) {
    return kStatus_USB_InvalidRequest;
  }

  if (message->isSetup != 0U) {
    if (ep != 0U) {
      return kStatus_USB_InvalidRequest;
    }

    if ((message->buffer != NULL) && (message->length >= USB_SETUP_PACKET_SIZE)) {
      memcpy(usbp->setup, message->buffer, USB_SETUP_PACKET_SIZE);
    }
    usbp->receiving &= (uint16_t)~MCX_USB_EP_MASK(0U);
    usbp->transmitting &= (uint16_t)~MCX_USB_EP_MASK(0U);
    _usb_isr_invoke_setup_cb(usbp, 0U);
    return kStatus_USB_Success;
  }

  if (message->length == USB_CANCELLED_TRANSFER_LENGTH) {
    return kStatus_USB_TransferCancel;
  }

  if (direction == USB_OUT) {
    if (epcp->out_state != NULL) {
      epcp->out_state->rxcnt = (size_t)message->length;
    }
    _usb_isr_invoke_out_cb(usbp, ep);
  }
  else {
    if (epcp->in_state != NULL) {
      epcp->in_state->txcnt = (size_t)message->length;
    }
    _usb_isr_invoke_in_cb(usbp, ep);
  }

  return kStatus_USB_Success;
}

static usb_status_t mcx_usb_endpoint_callback_in(usb_device_handle handle,
                                                 usb_device_endpoint_callback_message_struct_t *message,
                                                 void *callback_param) {

  return mcx_usb_endpoint_callback(handle, message, callback_param, USB_IN);
}

static usb_status_t mcx_usb_endpoint_callback_out(usb_device_handle handle,
                                                  usb_device_endpoint_callback_message_struct_t *message,
                                                  void *callback_param) {

  return mcx_usb_endpoint_callback(handle, message, callback_param, USB_OUT);
}

static void mcx_usb_init_endpoint_direction(usbep_t ep,
                                            const USBEndpointConfig *epcp,
                                            uint8_t direction) {
  usb_device_endpoint_init_struct_t ep_init;
  usb_device_endpoint_callback_struct_t ep_callback;

  ep_init.zlt             = 0U;
  ep_init.transferType    = mcx_usb_transfer_type(epcp->ep_mode);
  ep_init.interval        = 0U;
  ep_init.endpointAddress = epaddr(ep, direction);
  ep_init.maxPacketSize   = (direction == USB_IN) ? epcp->in_maxsize : epcp->out_maxsize;

  ep_callback.callbackFn    = (direction == USB_IN) ? mcx_usb_endpoint_callback_in : mcx_usb_endpoint_callback_out;
  ep_callback.callbackParam = (void *)(uintptr_t)ep;
  ep_callback.isBusy        = 0U;

  (void)USB_DeviceInitEndpoint(mcx_usb_device_handle, &ep_init, &ep_callback);
}

static usbepstatus_t mcx_usb_get_status(USBDriver *usbp,
                                        usbep_t ep,
                                        uint8_t direction) {
  usb_device_endpoint_status_struct_t endpoint_status;
  uint16_t enabled_mask;

  if (ep > (usbep_t)USB_MAX_ENDPOINTS) {
    return EP_STATUS_DISABLED;
  }

  enabled_mask = (direction == USB_IN) ? usbp->in_enabled : usbp->out_enabled;
  if ((enabled_mask & MCX_USB_EP_MASK(ep)) == 0U) {
    return EP_STATUS_DISABLED;
  }

  endpoint_status.endpointAddress = epaddr(ep, direction);
  endpoint_status.endpointStatus  = 0U;
  if (USB_DeviceGetStatus(mcx_usb_device_handle,
                          kUSB_DeviceStatusEndpoint,
                          &endpoint_status) != kStatus_USB_Success) {
    return EP_STATUS_DISABLED;
  }

  if (endpoint_status.endpointStatus == (uint16_t)kUSB_DeviceEndpointStateStalled) {
    return EP_STATUS_STALLED;
  }

  return EP_STATUS_ACTIVE;
}

static usb_status_t mcx_usb_submit_out(USBDriver *usbp, usbep_t ep) {
  const USBEndpointConfig *epcp = usbp->epc[ep];
  USBOutEndpointState *osp = epcp->out_state;
  usb_status_t status;

  status = USB_DeviceRecvRequest(mcx_usb_device_handle,
                                 epaddr(ep, USB_OUT),
                                 osp->rxbuf,
                                 (uint32_t)osp->rxsize);
  if (status == kStatus_USB_Success) {
    usbp->out_pending &= (uint16_t)~MCX_USB_EP_MASK(ep);
  }
  else {
    usbp->out_pending |= MCX_USB_EP_MASK(ep);
  }

  return status;
}

static usb_status_t mcx_usb_submit_in(USBDriver *usbp, usbep_t ep) {
  const USBEndpointConfig *epcp = usbp->epc[ep];
  USBInEndpointState *isp = epcp->in_state;
  usb_status_t status;

  status = USB_DeviceSendRequest(mcx_usb_device_handle,
                                 epaddr(ep, USB_IN),
                                 (uint8_t *)(uintptr_t)isp->txbuf,
                                 (uint32_t)isp->txsize);
  if (status == kStatus_USB_Success) {
    usbp->in_pending &= (uint16_t)~MCX_USB_EP_MASK(ep);
  }
  else {
    usbp->in_pending |= MCX_USB_EP_MASK(ep);
  }

  return status;
}

static void mcx_usb_retry_pending(USBDriver *usbp) {
  usbep_t ep;

  for (ep = 0U; ep <= (usbep_t)USB_MAX_ENDPOINTS; ep++) {
    uint16_t mask = MCX_USB_EP_MASK(ep);

    if ((usbp->out_pending & mask) != 0U) {
      if (((usbp->out_enabled & mask) != 0U) &&
          ((usbp->receiving & mask) != 0U) &&
          (usbp->epc[ep] != NULL) &&
          (usbp->epc[ep]->out_state != NULL)) {
        (void)mcx_usb_submit_out(usbp, ep);
      }
      else {
        usbp->out_pending &= (uint16_t)~mask;
      }
    }

    if ((usbp->in_pending & mask) != 0U) {
      if (((usbp->in_enabled & mask) != 0U) &&
          ((usbp->transmitting & mask) != 0U) &&
          (usbp->epc[ep] != NULL) &&
          (usbp->epc[ep]->in_state != NULL)) {
        (void)mcx_usb_submit_in(usbp, ep);
      }
      else {
        usbp->in_pending &= (uint16_t)~mask;
      }
    }
  }
}

/*===========================================================================*/
/* Driver interrupt handlers and threads.                                    */
/*===========================================================================*/

#if (MCX_USB_USE_USB1 == TRUE) || defined(__DOXYGEN__)
OSAL_IRQ_HANDLER(Vector14C) {

  OSAL_IRQ_PROLOGUE();
  USB_DeviceEhciIsrFunction(mcx_usb_device_handle);
  mcx_usb_retry_pending(&USBD1);
  OSAL_IRQ_EPILOGUE();
}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level USB driver initialization.
 *
 * @notapi
 */
void usb_lld_init(void) {

#if MCX_USB_USE_USB1 == TRUE
  usb_phy_config_struct_t phy_config = {
    MCX_USB_PHY_D_CAL,
    MCX_USB_PHY_TXCAL45DP,
    MCX_USB_PHY_TXCAL45DM
  };

  usbObjectInit(&USBD1);
  USBD1.in_enabled   = 0U;
  USBD1.out_enabled  = 0U;
  USBD1.in_pending   = 0U;
  USBD1.out_pending  = 0U;

  mcx_usb_clock_init();
  (void)USB_EhciPhyInit(MCX_USB_CONTROLLER_ID, 24000000U, &phy_config);
  (void)USB_DeviceInit(MCX_USB_CONTROLLER_ID,
                       mcx_usb_device_callback,
                       &mcx_usb_device_handle);
#endif
}

/**
 * @brief   Configures and activates the USB peripheral.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @notapi
 */
void usb_lld_start(USBDriver *usbp) {

#if MCX_USB_USE_USB1 == TRUE
  if ((usbp == &USBD1) && (usbp->state == USB_STOP)) {
    (void)USB_DeviceRun(mcx_usb_device_handle);
    nvicEnableVector(USB1_HS_IRQn, MCX_USB_USB1_IRQ_PRIORITY);
  }
#else
  (void)usbp;
#endif
}

/**
 * @brief   Deactivates the USB peripheral.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @notapi
 */
void usb_lld_stop(USBDriver *usbp) {

#if MCX_USB_USE_USB1 == TRUE
  if ((usbp == &USBD1) && (usbp->state == USB_READY)) {
    nvicDisableVector(USB1_HS_IRQn);
    (void)USB_DeviceStop(mcx_usb_device_handle);
    usbp->in_enabled   = 0U;
    usbp->out_enabled  = 0U;
    usbp->in_pending   = 0U;
    usbp->out_pending  = 0U;
  }
#else
  (void)usbp;
#endif
}

/**
 * @brief   USB low level reset routine.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @notapi
 */
void usb_lld_reset(USBDriver *usbp) {

  usbp->in_enabled   = 0U;
  usbp->out_enabled  = 0U;
  usbp->in_pending   = 0U;
  usbp->out_pending  = 0U;
  usbp->epc[0] = &ep0config;
  usb_lld_init_endpoint(usbp, 0U);
}

/**
 * @brief   Sets the USB address.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @notapi
 */
void usb_lld_set_address(USBDriver *usbp) {
  uint8_t address;
  uint8_t state;

  address = usbp->address;
  if (USB_DeviceSetStatus(mcx_usb_device_handle,
                          kUSB_DeviceStatusAddress,
                          &address) == kStatus_USB_Success) {
    state = (uint8_t)kUSB_DeviceStateAddress;
    (void)USB_DeviceSetStatus(mcx_usb_device_handle,
                              kUSB_DeviceStatusDeviceState,
                              &state);
  }
}

/**
 * @brief   Enables an endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @notapi
 */
void usb_lld_init_endpoint(USBDriver *usbp, usbep_t ep) {
  const USBEndpointConfig *epcp;

  if (ep > (usbep_t)USB_MAX_ENDPOINTS) {
    return;
  }

  epcp = usbp->epc[ep];
  if (epcp == NULL) {
    return;
  }

  if (epcp->in_state != NULL) {
    mcx_usb_init_endpoint_direction(ep, epcp, USB_IN);
    usbp->in_enabled |= MCX_USB_EP_MASK(ep);
  }

  if (epcp->out_state != NULL) {
    mcx_usb_init_endpoint_direction(ep, epcp, USB_OUT);
    usbp->out_enabled |= MCX_USB_EP_MASK(ep);
  }
}

/**
 * @brief   Disables all the active endpoints except the endpoint zero.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 *
 * @notapi
 */
void usb_lld_disable_endpoints(USBDriver *usbp) {
  usbep_t ep;

  for (ep = 1U; ep <= (usbep_t)USB_MAX_ENDPOINTS; ep++) {
    if ((usbp->in_enabled & MCX_USB_EP_MASK(ep)) != 0U) {
      (void)USB_DeviceCancel(mcx_usb_device_handle, epaddr(ep, USB_IN));
      (void)USB_DeviceDeinitEndpoint(mcx_usb_device_handle, epaddr(ep, USB_IN));
      usbp->in_enabled &= (uint16_t)~MCX_USB_EP_MASK(ep);
      usbp->in_pending &= (uint16_t)~MCX_USB_EP_MASK(ep);
    }
    if ((usbp->out_enabled & MCX_USB_EP_MASK(ep)) != 0U) {
      (void)USB_DeviceCancel(mcx_usb_device_handle, epaddr(ep, USB_OUT));
      (void)USB_DeviceDeinitEndpoint(mcx_usb_device_handle, epaddr(ep, USB_OUT));
      usbp->out_enabled &= (uint16_t)~MCX_USB_EP_MASK(ep);
      usbp->out_pending &= (uint16_t)~MCX_USB_EP_MASK(ep);
    }
  }
}

/**
 * @brief   Returns the status of an OUT endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 * @return              The endpoint status.
 *
 * @notapi
 */
usbepstatus_t usb_lld_get_status_out(USBDriver *usbp, usbep_t ep) {

  return mcx_usb_get_status(usbp, ep, USB_OUT);
}

/**
 * @brief   Returns the status of an IN endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 * @return              The endpoint status.
 *
 * @notapi
 */
usbepstatus_t usb_lld_get_status_in(USBDriver *usbp, usbep_t ep) {

  return mcx_usb_get_status(usbp, ep, USB_IN);
}

/**
 * @brief   Reads a setup packet from the dedicated packet buffer.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 * @param[out] buf      buffer where to copy the packet data
 *
 * @notapi
 */
void usb_lld_read_setup(USBDriver *usbp, usbep_t ep, uint8_t *buf) {

  (void)ep;
  if ((buf != NULL) && (buf != usbp->setup)) {
    memcpy(buf, usbp->setup, USB_SETUP_PACKET_SIZE);
  }
}

/**
 * @brief   Prepares for a receive operation.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @notapi
 */
void usb_lld_prepare_receive(USBDriver *usbp, usbep_t ep) {

  (void)usbp;
  (void)ep;
}

/**
 * @brief   Prepares for a transmit operation.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @notapi
 */
void usb_lld_prepare_transmit(USBDriver *usbp, usbep_t ep) {

  (void)usbp;
  (void)ep;
}

/**
 * @brief   Starts a receive operation on an OUT endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @notapi
 */
void usb_lld_start_out(USBDriver *usbp, usbep_t ep) {

  if ((ep <= (usbep_t)USB_MAX_ENDPOINTS) &&
      (usbp->epc[ep] != NULL) &&
      (usbp->epc[ep]->out_state != NULL)) {
    (void)mcx_usb_submit_out(usbp, ep);
  }
}

/**
 * @brief   Starts a transmit operation on an IN endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @notapi
 */
void usb_lld_start_in(USBDriver *usbp, usbep_t ep) {

  if ((ep <= (usbep_t)USB_MAX_ENDPOINTS) &&
      (usbp->epc[ep] != NULL) &&
      (usbp->epc[ep]->in_state != NULL)) {
    (void)mcx_usb_submit_in(usbp, ep);
  }
}

/**
 * @brief   Brings an OUT endpoint in the stalled state.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @notapi
 */
void usb_lld_stall_out(USBDriver *usbp, usbep_t ep) {

  (void)usbp;
  (void)USB_DeviceStallEndpoint(mcx_usb_device_handle, epaddr(ep, USB_OUT));
}

/**
 * @brief   Brings an IN endpoint in the stalled state.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @notapi
 */
void usb_lld_stall_in(USBDriver *usbp, usbep_t ep) {

  (void)usbp;
  (void)USB_DeviceStallEndpoint(mcx_usb_device_handle, epaddr(ep, USB_IN));
}

/**
 * @brief   Brings an OUT endpoint in the active state.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @notapi
 */
void usb_lld_clear_out(USBDriver *usbp, usbep_t ep) {

  (void)usbp;
  (void)USB_DeviceUnstallEndpoint(mcx_usb_device_handle, epaddr(ep, USB_OUT));
}

/**
 * @brief   Brings an IN endpoint in the active state.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 *
 * @notapi
 */
void usb_lld_clear_in(USBDriver *usbp, usbep_t ep) {

  (void)usbp;
  (void)USB_DeviceUnstallEndpoint(mcx_usb_device_handle, epaddr(ep, USB_IN));
}

/**
 * @brief   Returns the current frame number.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @return              The current frame number.
 *
 * @notapi
 */
uint32_t usb_lld_get_frame_number(USBDriver *usbp) {
  uint16_t frame = 0U;

  (void)usbp;
  (void)USB_DeviceGetStatus(mcx_usb_device_handle,
                            kUSB_DeviceStatusSynchFrame,
                            &frame);
  return (uint32_t)frame;
}

#endif /* HAL_USE_USB == TRUE */

/** @} */

