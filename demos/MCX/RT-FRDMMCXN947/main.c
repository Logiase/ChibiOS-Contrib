#include "ch.h"
#include "hal.h"

static volatile uint32_t heartbeat;

typedef struct {
  ioline_t line;
  bool led_on;
} ButtonWaitConfig;

static ButtonWaitConfig sw2_wait = {
  .line = LINE_BUTTON_SW2,
  .led_on = true,
};

static ButtonWaitConfig sw3_wait = {
  .line = LINE_BUTTON_SW3,
  .led_on = false,
};

static THD_WORKING_AREA(waSw2Thread, 128);
static THD_WORKING_AREA(waSw3Thread, 128);

static THD_FUNCTION(ButtonWaitThread, arg) {
  const ButtonWaitConfig *config = (const ButtonWaitConfig *)arg;

  while (true) {
    if (palWaitLineTimeout(config->line, TIME_INFINITE) == MSG_OK) {
      if (config->led_on) {
        palClearLine(LINE_LED_RED);
      } else {
        palSetLine(LINE_LED_RED);
      }
    }
  }
}

int main(void) {

  halInit();
  chSysInit();

  palEnableLineEvent(LINE_BUTTON_SW2, PAL_EVENT_MODE_RISING_EDGE);
  palEnableLineEvent(LINE_BUTTON_SW3, PAL_EVENT_MODE_FALLING_EDGE);

  chThdCreateStatic(waSw2Thread, sizeof(waSw2Thread), NORMALPRIO, ButtonWaitThread, (void *)&sw2_wait);
  chThdCreateStatic(waSw3Thread, sizeof(waSw3Thread), NORMALPRIO, ButtonWaitThread, (void *)&sw3_wait);

  while (true) {
    heartbeat++;
    chThdSleepMilliseconds(500);
  }
}
