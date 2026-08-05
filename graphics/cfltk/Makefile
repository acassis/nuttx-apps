############################################################################
# apps/graphics/cfltk/Makefile
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(APPDIR)/Make.defs

# cFLTK: platform-independent core only (see Kconfig). No windowing
# backend is provided yet -- src/backend/nx/ does not exist. Anything
# that actually links a window will need it; this package just proves
# the core builds under the NuttX toolchain, same milestone shape as
# `graphics/microwindows` before `examples/microwindows` existed.

CFLTK_DIR_NAME = cfltk

WD := ${shell echo $(CURDIR) | sed -e 's/ /\\ /g'}

CFLTK_COMMIT_HASH := fad7b0cf128331f5c474634773a8a0f0139d736a

CONFIG_GRAPH_CFLTK_URL ?= https://codeload.github.com/cFLTK/cfltk/zip/$(CFLTK_COMMIT_HASH)

CFLTK_TARBALL := cfltk-$(CFLTK_COMMIT_HASH).zip

UNPACK ?= unzip -o $(if $(V),,-q)
CURL ?= curl -L $(if $(V),,-Ss)

$(CFLTK_TARBALL):
	$(ECHO_BEGIN)"Downloading: $(CFLTK_TARBALL)"
	$(Q) $(CURL) -o $(CFLTK_TARBALL) $(CONFIG_GRAPH_CFLTK_URL)
	$(ECHO_END)

$(CFLTK_DIR_NAME): $(CFLTK_TARBALL)
	$(ECHO_BEGIN)"Unpacking: $(CFLTK_TARBALL) -> $(CFLTK_DIR_NAME)"
	$(Q) $(UNPACK) $(CFLTK_TARBALL)
	$(Q) mv cfltk-$(CFLTK_COMMIT_HASH) $(CFLTK_DIR_NAME)
	$(Q) touch $(CFLTK_DIR_NAME)
	$(ECHO_END)

# Download and unpack if no git repo found in-tree (mirrors
# graphics/microwindows's pattern for third-party sources).
ifeq ($(wildcard $(CFLTK_DIR_NAME)/.git),)
context:: $(CFLTK_DIR_NAME)

distclean::
	$(call DELDIR, $(CFLTK_DIR_NAME))
	$(call DELFILE, $(CFLTK_TARBALL))
endif

CFLAGS += -std=c99
CFLAGS += -Wno-unused-parameter -Wno-missing-field-initializers
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/graphics/cfltk/cfltk/include

# core

CSRCS += cfltk/src/core/Fl.c
CSRCS += cfltk/src/core/fl_filename.c
CSRCS += cfltk/src/core/Fl_Group.c
CSRCS += cfltk/src/core/Fl_Preferences.c
CSRCS += cfltk/src/core/Fl_Tooltip.c
CSRCS += cfltk/src/core/fl_utf8.c
CSRCS += cfltk/src/core/Fl_Widget.c

# dialogs

CSRCS += cfltk/src/dialogs/fl_ask.c
CSRCS += cfltk/src/dialogs/fl_file_chooser.c
CSRCS += cfltk/src/dialogs/Fl_Help_Dialog.c

# draw

CSRCS += cfltk/src/draw/fl_colormap.c
CSRCS += cfltk/src/draw/fl_draw.c
CSRCS += cfltk/src/draw/fl_symbols.c

# image (PNG/JPEG need external codec libraries -- see Kconfig)

CSRCS += cfltk/src/image/Fl_Bitmap.c
CSRCS += cfltk/src/image/Fl_BMP_Image.c
CSRCS += cfltk/src/image/Fl_GIF_Image.c
CSRCS += cfltk/src/image/Fl_Image.c
CSRCS += cfltk/src/image/Fl_Pixmap.c
CSRCS += cfltk/src/image/Fl_RGB_Image.c
CSRCS += cfltk/src/image/Fl_Shared_Image.c
CSRCS += cfltk/src/image/Fl_XBM_Image.c
CSRCS += cfltk/src/image/Fl_XPM_Image.c

ifeq ($(CONFIG_GRAPHICS_CFLTK_PNG),y)
CSRCS += cfltk/src/image/Fl_PNG_Image.c
endif

ifeq ($(CONFIG_GRAPHICS_CFLTK_JPEG),y)
CSRCS += cfltk/src/image/Fl_JPEG_Image.c
endif

# menu

CSRCS += cfltk/src/menu/Fl_Choice.c
CSRCS += cfltk/src/menu/Fl_Menu_Bar.c
CSRCS += cfltk/src/menu/Fl_Menu_Button.c
CSRCS += cfltk/src/menu/Fl_Menu_.c
CSRCS += cfltk/src/menu/Fl_Menu_Item.c
CSRCS += cfltk/src/menu/fl_menu_popup.c

# text

CSRCS += cfltk/src/text/Fl_Text_Buffer.c
CSRCS += cfltk/src/text/Fl_Text_Display.c
CSRCS += cfltk/src/text/Fl_Text_Editor.c

# valuators

CSRCS += cfltk/src/valuators/Fl_Adjuster.c
CSRCS += cfltk/src/valuators/Fl_Counter.c
CSRCS += cfltk/src/valuators/Fl_Dial.c
CSRCS += cfltk/src/valuators/Fl_Fill_Dial.c
CSRCS += cfltk/src/valuators/Fl_Fill_Slider.c
CSRCS += cfltk/src/valuators/Fl_Hor_Fill_Slider.c
CSRCS += cfltk/src/valuators/Fl_Hor_Nice_Slider.c
CSRCS += cfltk/src/valuators/Fl_Hor_Slider.c
CSRCS += cfltk/src/valuators/Fl_Hor_Value_Slider.c
CSRCS += cfltk/src/valuators/Fl_Line_Dial.c
CSRCS += cfltk/src/valuators/Fl_Nice_Slider.c
CSRCS += cfltk/src/valuators/Fl_Roller.c
CSRCS += cfltk/src/valuators/Fl_Scrollbar.c
CSRCS += cfltk/src/valuators/Fl_Simple_Counter.c
CSRCS += cfltk/src/valuators/Fl_Slider.c
CSRCS += cfltk/src/valuators/Fl_Valuator.c
CSRCS += cfltk/src/valuators/Fl_Value_Input.c
CSRCS += cfltk/src/valuators/Fl_Value_Output.c
CSRCS += cfltk/src/valuators/Fl_Value_Slider.c

# widgets

CSRCS += cfltk/src/widgets/Fl_Box.c
CSRCS += cfltk/src/widgets/Fl_Browser.c
CSRCS += cfltk/src/widgets/Fl_Browser_.c
CSRCS += cfltk/src/widgets/Fl_Button.c
CSRCS += cfltk/src/widgets/Fl_Check_Browser.c
CSRCS += cfltk/src/widgets/Fl_Check_Button.c
CSRCS += cfltk/src/widgets/Fl_Clock.c
CSRCS += cfltk/src/widgets/Fl_Color_Chooser.c
CSRCS += cfltk/src/widgets/Fl_Double_Window.c
CSRCS += cfltk/src/widgets/Fl_File_Browser.c
CSRCS += cfltk/src/widgets/Fl_File_Input.c
CSRCS += cfltk/src/widgets/Fl_Float_Input.c
CSRCS += cfltk/src/widgets/Fl_Help_View.c
CSRCS += cfltk/src/widgets/Fl_Hold_Browser.c
CSRCS += cfltk/src/widgets/Fl_Input.c
CSRCS += cfltk/src/widgets/Fl_Input_Choice.c
CSRCS += cfltk/src/widgets/Fl_Int_Input.c
CSRCS += cfltk/src/widgets/Fl_Light_Button.c
CSRCS += cfltk/src/widgets/Fl_Menu_Window.c
CSRCS += cfltk/src/widgets/Fl_Multi_Browser.c
CSRCS += cfltk/src/widgets/Fl_Multiline_Input.c
CSRCS += cfltk/src/widgets/Fl_Multiline_Output.c
CSRCS += cfltk/src/widgets/Fl_Output.c
CSRCS += cfltk/src/widgets/Fl_Pack.c
CSRCS += cfltk/src/widgets/Fl_Progress.c
CSRCS += cfltk/src/widgets/Fl_Radio_Button.c
CSRCS += cfltk/src/widgets/Fl_Repeat_Button.c
CSRCS += cfltk/src/widgets/Fl_Return_Button.c
CSRCS += cfltk/src/widgets/Fl_Round_Button.c
CSRCS += cfltk/src/widgets/Fl_Scroll.c
CSRCS += cfltk/src/widgets/Fl_Secret_Input.c
CSRCS += cfltk/src/widgets/Fl_Select_Browser.c
CSRCS += cfltk/src/widgets/Fl_Single_Window.c
CSRCS += cfltk/src/widgets/Fl_Spinner.c
CSRCS += cfltk/src/widgets/Fl_Tabs.c
CSRCS += cfltk/src/widgets/Fl_Tile.c
CSRCS += cfltk/src/widgets/Fl_Toggle_Button.c
CSRCS += cfltk/src/widgets/Fl_Window.c
CSRCS += cfltk/src/widgets/Fl_Wizard.c

# backend (see Kconfig -- exactly one must be selected for anything to
# link; the core above builds fine with none selected, same milestone
# shape as graphics/microwindows before examples/microwindows existed)

ifeq ($(CONFIG_GRAPHICS_CFLTK_BACKEND_NX),y)
CSRCS += cfltk/src/backend/nx/fl_nx_window.c
CSRCS += cfltk/src/backend/nx/fl_nx_event.c
CSRCS += cfltk/src/backend/nx/fl_nx_driver.c
endif

include $(APPDIR)/Application.mk
