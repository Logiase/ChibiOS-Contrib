#include "hal.h"

/**
 * @brief   Board-specific initialization code.
 * @note    You can add your board-specific code here.
 */
void boardInit(void) {
  palSetLine(LINE_LED_RED);
  palSetLine(LINE_LED_GREEN);
  palSetLine(LINE_LED_BLUE);

  palSetLineMode(LINE_LED_RED, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_LED_GREEN, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_LED_BLUE, PAL_MODE_OUTPUT_PUSHPULL);

  palSetLineMode(LINE_BUTTON_SW2, PAL_MODE_INPUT_PULLUP);
  palSetLineMode(LINE_BUTTON_SW3, PAL_MODE_INPUT_PULLUP);
}
