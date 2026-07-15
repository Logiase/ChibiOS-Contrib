MCX_NEED_LPFLEXCOMM_LPUARTv1 :=

ifeq ($(USE_SMART_BUILD),yes)
  ifneq ($(findstring HAL_USE_SERIAL TRUE,$(HALCONF)),)
    PLATFORMSRC_CONTRIB += $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/LPUARTv1/hal_serial_lld.c
    MCX_NEED_LPFLEXCOMM_LPUARTv1 := yes
  endif
  ifneq ($(findstring HAL_USE_UART TRUE,$(HALCONF)),)
    PLATFORMSRC_CONTRIB += $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/LPUARTv1/hal_uart_lld.c
	MCX_NEED_LPFLEXCOMM_LPUARTv1 := yes
  endif
else
  PLATFORMSRC_CONTRIB += $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/LPUARTv1/hal_serial_lld.c 
  MCX_NEED_LPFLEXCOMM_LPUARTv1 := yes
endif

ifneq ($(MCX_NEED_LPFLEXCOMM_LPUARTv1),)
  MCX_NEED_LPFLEXCOMM := yes
  PLATFORMSRC_CONTRIB += $(CHIBIOS_CONTRIB)/ext/mcuxsdk-core/drivers/lpflexcomm/lpuart/fsl_lpuart.c
endif

PLATFORMINC_CONTRIB += $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/LPUARTv1 \
					   $(CHIBIOS_CONTRIB)/ext/mcuxsdk-core/drivers/lpflexcomm/lpuart
