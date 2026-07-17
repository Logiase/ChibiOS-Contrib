#include "ch.h"
#include "hal.h"
#include "usbcfg.h"

#if defined (__TEST_RT)
#include "rt_test_root.h"
#endif
#if defined (__TEST_OSLIB)
#include "oslib_test_root.h"
#endif

/* Testing in progress.*/
static bool testing = false;

semaphore_t scls;

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 256);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palToggleLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(testing ? 100 : 500);
  }
}

/*
 * USB CDC ACM echo thread.
 */
static THD_WORKING_AREA(waUsbEchoThread, 256);
static THD_FUNCTION(UsbEchoThread, arg) {
  uint8_t buffer[64];

  (void)arg;
  chRegSetThreadName("usb_echo");

  while (true) {
    if (SDU1.config->usbp->state != USB_ACTIVE) {
      chThdSleepMilliseconds(50);
      continue;
    }

    size_t n = chnReadTimeout(&SDU1, buffer, sizeof buffer, TIME_MS2I(100));
    if (n > 0U) {
      (void)chnWriteTimeout(&SDU1, buffer, n, TIME_MS2I(100));
    }
  }
}
/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the Serial or SIO driver using the default configuration.
   */
  sdStart(&SD4, NULL);
  /*
   * Activates the USB CDC ACM device on the board USBHS connector.
   */
  chSemObjectInit(&scls, 0);
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO + 10, Thread1, NULL);
  chThdCreateStatic(waUsbEchoThread, sizeof(waUsbEchoThread), NORMALPRIO + 1,
                    UsbEchoThread, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
#if defined (__TEST_RT) || defined (__TEST_OSLIB)
    if (palReadLine(LINE_BUTTON_SW2) == PAL_LOW) {
      testing = true;
#if defined (__TEST_RT)
      test_execute((BaseSequentialStream *)&SD4, &rt_test_suite);
#endif
#if defined (__TEST_OSLIB)
      test_execute((BaseSequentialStream *)&SD4, &oslib_test_suite);
#endif
      testing = false;
    }
#endif
    chThdSleepMilliseconds(500);
  }
}
