# List of all the board related files.
BOARDSRC = $(CHIBIOS_CONTRIB)/os/hal/boards/FRDMMCXN947/board.c

# Required include directories
BOARDINC = $(CHIBIOS_CONTRIB)/os/hal/boards/FRDMMCXN947

ALLCSRC += $(BOARDSRC)
ALLINC += $(BOARDINC)
