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
 * @file    hal_lld.c
 * @brief   PLATFORM HAL subsystem low level driver source.
 *
 * @addtogroup HAL
 * @{
 */

#include "hal.h"

#include "fsl_clock.h"
#include "fsl_spc.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/**
 * @brief   CMSIS system core clock variable.
 * @note    It is declared in CMSIS header.
 */
uint32_t SystemCoreClock;

#if defined(HAL_LLD_USE_CLOCK_MANAGEMENT) && !defined(__DOXYGEN__)
/**
 * @brief   Post-reset configuration, must be implemented.
 */
const halclkcfg_t hal_clkcfg_reset = {
  .dummy = 0U
};

/**
 * @brief   Configuration from mcuconf.h, must be implemented.
 */
const halclkcfg_t hal_clkcfg_default = {
  .dummy = 0U
};
#endif

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static void BOARD_BootClockPLL150M(void) {
  CLOCK_EnableClock(kCLOCK_Scg);
  CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);

  spc_active_mode_dcdc_option_t dcdcOpt = {
    .DCDCVoltage = kSPC_DCDC_OverdriveVoltage,
    .DCDCDriveStrength = kSPC_DCDC_NormalDriveStrength,
  };
  SPC_SetActiveModeDCDCRegulatorConfig(SPC0, &dcdcOpt);
  spc_active_mode_core_ldo_option_t ldoOpt = {
    .CoreLDOVoltage = kSPC_CoreLDO_OverDriveVoltage,
    .CoreLDODriveStrength = kSPC_CoreLDO_NormalDriveStrength,
  };
  SPC_SetActiveModeCoreLDORegulatorConfig(SPC0, &ldoOpt);

  FMU0->FCTRL = (FMU0->FCTRL & ~((uint32_t)FMU_FCTRL_RWSC_MASK)) | (FMU_FCTRL_RWSC(0x3U));

  spc_sram_voltage_config_t sramCfg = {
    .operateVoltage = kSPC_sramOperateAt1P2V,
    .requestVoltageUpdate = true,
  };
  SPC_SetSRAMOperateVoltage(SPC0, &sramCfg);

  CLOCK_SetupFROHFClocking(48000000U);
  const pll_setup_t pll0Setup = {
    .pllctrl = SCG_APLLCTRL_SOURCE(1U) | SCG_APLLCTRL_SELI(27U) | SCG_APLLCTRL_SELP(13U),
    .pllndiv = SCG_APLLNDIV_NDIV(8U),
    .pllpdiv = SCG_APLLPDIV_PDIV(1U),
    .pllmdiv = SCG_APLLMDIV_MDIV(50U),
    .pllRate = 150000000U,
  };
  CLOCK_SetPLL0Freq(&pll0Setup);
  CLOCK_SetPll0MonitorMode(kSCG_Pll0MonitorDisable);

  CLOCK_AttachClk(kPLL0_to_MAIN_CLK);

  CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);

  SystemCoreClock = 150000000U;
} 

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level HAL driver initialization.
 *
 * @notapi
 */
void hal_lld_init(void) {
  BOARD_BootClockPLL150M();
}

#if defined(HAL_LLD_USE_CLOCK_MANAGEMENT) || defined(__DOXYGEN__)
/**
 * @brief   Switches to a different clock configuration
 *
 * @param[in] ccp       pointer to clock a @p halclkcfg_t structure
 * @return              The clock switch result.
 * @retval false        if the clock switch succeeded
 * @retval true         if the clock switch failed
 *
 * @notapi
 */
bool hal_lld_clock_switch_mode(const halclkcfg_t *ccp) {

  (void)ccp;

  return false;
}

/**
 * @brief   Returns the frequency of a clock point in Hz.
 *
 * @param[in] clkpt     clock point to be returned
 * @return              The clock point frequency in Hz or zero if the
 *                      frequency is unknown.
 *
 * @notapi
 */
halfreq_t hal_lld_get_clock_point(halclkpt_t clkpt) {

  (void)clkpt;

  return 0U;
}
#endif /* defined(HAL_LLD_USE_CLOCK_MANAGEMENT) */

/** @} */
