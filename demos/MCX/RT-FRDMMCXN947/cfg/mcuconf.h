#ifndef MCUCONF_H
#define MCUCONF_H

#define MCXN947_MCUCONF

#define MCX_ST_IRQ_PRIORITY                 7

#define MCX_USB_USE_USB1                  TRUE
#define MCX_USB_USB1_IRQ_PRIORITY         3
#define MCX_USB_PHY_D_CAL                 0x04U
#define MCX_USB_PHY_TXCAL45DP             0x07U
#define MCX_USB_PHY_TXCAL45DM             0x07U

#define MCX_SERIAL_USE_LPUART0              FALSE
#define MCX_SERIAL_USE_LPUART1              FALSE
#define MCX_SERIAL_USE_LPUART2              FALSE
#define MCX_SERIAL_USE_LPUART3              FALSE
#define MCX_SERIAL_USE_LPUART4              TRUE
#define MCX_SERIAL_USE_LPUART5              FALSE
#define MCX_SERIAL_USE_LPUART6              FALSE
#define MCX_SERIAL_USE_LPUART7              FALSE
#define MCX_SERIAL_USE_LPUART8              FALSE
#define MCX_SERIAL_USE_LPUART9              FALSE

#endif /* MCUCONF_H */
