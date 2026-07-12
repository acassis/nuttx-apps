include $(APPDIR)/Make.defs

PROGNAME = picotts
PRIORITY = $(CONFIG_PICOTTS_PRIORITY)
STACKSIZE = $(CONFIG_PICOTTS_STACK_SIZE)
MODULE = $(CONFIG_EXAMPLES_PICOTTS)

MAINSRC = picotts_cmd.c

CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/audioutils/picotts/include

include $(APPDIR)/Application.mk
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/audioutils/picotts/pico
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/audioutils/picotts/include
CFLAGS += -I$(APPDIR)/audioutils/picotts/pico
CFLAGS += -I$(APPDIR)/audioutils/picotts/include
