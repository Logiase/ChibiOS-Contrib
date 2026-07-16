PLATFORMSRC_CONTRIB := $(CHIBIOS)/os/hal/ports/common/ARMCMx/nvic.c \
					   $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/MCXN947/hal_lld.c \
					   $(CHIBIOS_CONTRIB)/ext/mcux-devices-mcx/MCXN/MCXN947/drivers/fsl_clock.c \
					   $(CHIBIOS_CONTRIB)/ext/mcux-devices-mcx/MCXN/MCXN947/drivers/fsl_edma_soc.c \
					   $(CHIBIOS_CONTRIB)/ext/mcux-devices-mcx/MCXN/MCXN947/drivers/fsl_reset.c \
					   $(CHIBIOS_CONTRIB)/ext/mcuxsdk-core/drivers/common/fsl_common_arm.c \
					   $(CHIBIOS_CONTRIB)/ext/mcuxsdk-core/drivers/mcx_spc/fsl_spc.c

PLATFORMINC_CONTRIB := $(CHIBIOS)/os/hal/ports/common/ARMCMx \
					   $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/MCXN947 \
					   $(CHIBIOS_CONTRIB)/ext/mcux-devices-mcx/MCXN/MCXN947 \
					   $(CHIBIOS_CONTRIB)/ext/mcux-devices-mcx/MCXN/MCXN947/drivers \
					   $(CHIBIOS_CONTRIB)/ext/mcuxsdk-core/drivers/common \
					   $(CHIBIOS_CONTRIB)/ext/mcuxsdk-core/drivers/mcx_spc
					   

ifeq ($(USE_SMART_BUILD),yes)
  ifeq ($(CONFDIR),)
    CONFDIR = .
  endif

  HALCONF := $(strip $(shell cat $(CONFDIR)/halconf.h $(CONFDIR)/halconf_community.h | egrep -e "\#define"))
endif

DDEFS  += -include mcx_device.h
DADEFS += -include mcx_device.h

# Drivers compatible with the platform.
include $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/SYSTICKv1/driver.mk
include $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/GPIOv1/driver.mk
include $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/LPUARTv1/driver.mk
include $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/USBHSv1/driver.mk

ifneq ($(MCX_NEED_LPFLEXCOMM),)
  PLATFORMSRC_CONTRIB += $(CHIBIOS_CONTRIB)/ext/mcuxsdk-core/drivers/lpflexcomm/fsl_lpflexcomm.c
  PLATFORMINC_CONTRIB += $(CHIBIOS_CONTRIB)/ext/mcuxsdk-core/drivers/lpflexcomm
endif

# Shared variables.
ALLCSRC += $(PLATFORMSRC_CONTRIB)
ALLINC  += $(PLATFORMINC_CONTRIB)
