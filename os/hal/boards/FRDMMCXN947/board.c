#include "hal.h"

static void mcx_gpio_init(void) {

  CLOCK_EnableClock(kCLOCK_Port0);
  CLOCK_EnableClock(kCLOCK_Port1);
  CLOCK_EnableClock(kCLOCK_Port2);
  CLOCK_EnableClock(kCLOCK_Port3);
  CLOCK_EnableClock(kCLOCK_Port4);

  palSetLine(LINE_LED_RED);
  palSetLine(LINE_LED_GREEN);
  palSetLine(LINE_LED_BLUE);

  palSetLineMode(LINE_LED_RED, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_LED_GREEN, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_LED_BLUE, PAL_MODE_OUTPUT_PUSHPULL);

  palSetLineMode(LINE_BUTTON_SW2, PAL_MODE_INPUT_PULLUP);
  palSetLineMode(LINE_BUTTON_SW3, PAL_MODE_INPUT_PULLUP);

  palSetLineMode(LINE_LPUART4_RX, BOARD_LPUART4_MODE);
  palSetLineMode(LINE_LPUART4_TX, BOARD_LPUART4_MODE);
}

void __early_init(void) {
}

/**
 * @brief   Board-specific initialization code.
 * @note    You can add your board-specific code here.
 */
void boardInit(void) {

  mcx_gpio_init();
}
