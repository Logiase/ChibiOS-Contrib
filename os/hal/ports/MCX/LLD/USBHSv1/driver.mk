HAL_USB_SRC = $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/USBHSv1/hal_usb_lld.c \
              $(CHIBIOS_CONTRIB)/ext/mcuxsdk-middleware-usb/device/usb_device_dci.c \
              $(CHIBIOS_CONTRIB)/ext/mcuxsdk-middleware-usb/device/usb_device_ehci.c \
              $(CHIBIOS_CONTRIB)/ext/mcuxsdk-middleware-usb/phy/usb_phy.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/USBHSv1/components/osa/fsl_os_abstraction_bm.c \
              $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/USBHSv1/components/lists/fsl_component_generic_list.c

ifeq ($(USE_SMART_BUILD),yes)
ifneq ($(findstring HAL_USE_USB TRUE,$(HALCONF)),)
PLATFORMSRC_CONTRIB += $(HAL_USB_SRC)
endif
else
PLATFORMSRC_CONTRIB += $(HAL_USB_SRC)
endif

PLATFORMINC_CONTRIB += $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/USBHSv1 \
                       $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/USBHSv1/components/osa \
                       $(CHIBIOS_CONTRIB)/os/hal/ports/MCX/LLD/USBHSv1/components/lists \
                       $(CHIBIOS_CONTRIB)/ext/mcuxsdk-middleware-usb/config/device/ehci \
                       $(CHIBIOS_CONTRIB)/ext/mcuxsdk-middleware-usb/include \
                       $(CHIBIOS_CONTRIB)/ext/mcuxsdk-middleware-usb/device \
                       $(CHIBIOS_CONTRIB)/ext/mcuxsdk-middleware-usb/phy
