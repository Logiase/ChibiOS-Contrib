#ifndef BOARD_H
#define BOARD_H

/*
 * Setup for the NXP FRDM-MCXN947 board.
 */

/*
 * Board identifier.
 */
#define BOARD_FRDMMCXN947
#define BOARD_NAME                  "NXP FRDM-MCXN947"

/*
 * Board LEDs.
 *
 * The RGB LED signals are active low on the FRDM-MCXN947. The pin assignments
 * match the MCUXpresso SDK pin_mux data for the board.
 */
#define BOARD_LED_ACTIVE_LOW    TRUE

#define BOARD_LED_RED_PORT      GPIO0
#define BOARD_LED_RED_PAD       10U
#define BOARD_LED_RED_MASK      (1UL << BOARD_LED_RED_PAD)
#define LINE_LED_RED            PAL_LINE(BOARD_LED_RED_PORT, BOARD_LED_RED_PAD)

#define BOARD_LED_GREEN_PORT    GPIO0
#define BOARD_LED_GREEN_PAD     27U
#define BOARD_LED_GREEN_MASK    (1UL << BOARD_LED_GREEN_PAD)
#define LINE_LED_GREEN          PAL_LINE(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PAD)

#define BOARD_LED_BLUE_PORT    GPIO1
#define BOARD_LED_BLUE_PAD     2U
#define BOARD_LED_BLUE_MASK    (1UL << BOARD_LED_BLUE_PAD)
#define LINE_LED_BLUE          PAL_LINE(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PAD)

/*
 * Board buttons.
 */
#define BOARD_SW2_PORT         GPIO0
#define BOARD_SW2_PAD          23U
#define BOARD_SW2_MASK         (1UL << BOARD_SW2_PAD)
#define LINE_BUTTON_SW2        PAL_LINE(BOARD_SW2_PORT, BOARD_SW2_PAD)

#define BOARD_SW3_PORT         GPIO0
#define BOARD_SW3_PAD          6U
#define BOARD_SW3_MASK         (1UL << BOARD_SW3_PAD)
#define LINE_BUTTON_SW3        PAL_LINE(BOARD_SW3_PORT, BOARD_SW3_PAD)

/*
 * Serial Port.
 */
#define BOARD_LPUART4_RX_PORT      GPIO1
#define BOARD_LPUART4_RX_PAD       8U
#define BOARD_LPUART4_RX_MASK      (1UL << BOARD_LPUART4_RX_PAD)
#define LINE_LPUART4_RX            PAL_LINE(BOARD_LPUART4_RX_PORT, BOARD_LPUART4_RX_PAD)

#define BOARD_LPUART4_TX_PORT      GPIO1
#define BOARD_LPUART4_TX_PAD       9U
#define BOARD_LPUART4_TX_MASK      (1UL << BOARD_LPUART4_TX_PAD)
#define LINE_LPUART4_TX            PAL_LINE(BOARD_LPUART4_TX_PORT, BOARD_LPUART4_TX_PAD)

#define BOARD_LPUART4_MODE         (PAL_MODE_ALTERNATE(2U) | \
                                    PORT_PCR_IBE(1U) |      \
                                    PORT_PCR_PE(1U) |       \
                                    PORT_PCR_PS(1U))

#if !defined(_FROM_ASM_)

#include "fsl_device_registers.h"

#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* BOARD_H */
