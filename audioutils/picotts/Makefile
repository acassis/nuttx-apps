############################################################################
# apps/audioutils/picotts/Makefile
############################################################################

include $(APPDIR)/Make.defs

PICO_DIR = $(APPDIR)/audioutils/picotts/pico

# NuttX adapter source + Pico core
CSRCS  = picotts.c

# Pico TTS core — ALL .c files including picorsrc.c
CSRCS += $(PICO_DIR)/picoacph.c
CSRCS += $(PICO_DIR)/picoapi.c
CSRCS += $(PICO_DIR)/picobase.c
CSRCS += $(PICO_DIR)/picocep.c
CSRCS += $(PICO_DIR)/picoctrl.c
CSRCS += $(PICO_DIR)/picodata.c
CSRCS += $(PICO_DIR)/picodbg.c
CSRCS += $(PICO_DIR)/picoextapi.c
CSRCS += $(PICO_DIR)/picofftsg.c
CSRCS += $(PICO_DIR)/picokdbg.c
CSRCS += $(PICO_DIR)/picokdt.c
CSRCS += $(PICO_DIR)/picokfst.c
CSRCS += $(PICO_DIR)/picoklex.c
CSRCS += $(PICO_DIR)/picoknow.c
CSRCS += $(PICO_DIR)/picokpdf.c
CSRCS += $(PICO_DIR)/picokpr.c
CSRCS += $(PICO_DIR)/picoktab.c
CSRCS += $(PICO_DIR)/picoos.c
CSRCS += $(PICO_DIR)/picopal.c
CSRCS += $(PICO_DIR)/picopam.c
CSRCS += $(PICO_DIR)/picopr.c
CSRCS += $(PICO_DIR)/picosa.c
CSRCS += $(PICO_DIR)/picosig.c
CSRCS += $(PICO_DIR)/picosig2.c
CSRCS += $(PICO_DIR)/picospho.c
CSRCS += $(PICO_DIR)/picotok.c
CSRCS += $(PICO_DIR)/picotrns.c
CSRCS += $(PICO_DIR)/picowa.c
CSRCS += $(PICO_DIR)/picorsrc.c

CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/audioutils/picotts/pico
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/audioutils/picotts/include

include $(APPDIR)/Application.mk
