# List of the ChibiOS generic MCXN947 startup and CMSIS files.
STARTUPSRC = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/crt1.c

STARTUPASM = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/crt0_v8m-ml.S \
             $(CHIBIOS_CONTRIB)/os/common/startup/ARMCMx/compilers/GCC/vectors_mcx.S

STARTUPINC = $(CHIBIOS)/os/common/portability/GCC \
             $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC \
             $(CHIBIOS_CONTRIB)/os/common/startup/ARMCMx/devices/MCXN947 \
             $(CHIBIOS)/os/common/ext/ARM/CMSIS/Core/Include \
             $(CHIBIOS_CONTRIB)/ext/mcux-devices-mcx/MCXN/MCXN947 \
             $(CHIBIOS_CONTRIB)/ext/mcux-devices-mcx/MCXN/periph

STARTUPLD = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/ld
STARTUPLD_CONTRIB = $(CHIBIOS_CONTRIB)/os/common/startup/ARMCMx/compilers/GCC/ld

ALLXASMSRC += $(STARTUPASM)
ALLCSRC    += $(STARTUPSRC)
ALLINC     += $(STARTUPINC)
