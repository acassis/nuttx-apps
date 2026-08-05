############################################################################
# apps/graphics/claucodillo/Makefile
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

# ClauCoDillo: dlib, lout, and dw's platform-independent core
# (dw-core -- widget/layout base classes, no cfltk dependency) so far.
# Same incremental shape as graphics/cfltk's own first pass. dw-fltk
# (cfltk-specific platform glue), dw-widgets (concrete widget types),
# and src/ come in follow-up commits.

CLAUCODILLO_DIR_NAME = claucodillo

WD := ${shell echo $(CURDIR) | sed -e 's/ /\\ /g'}

CLAUCODILLO_COMMIT_HASH := 040742ada189529a4d361306e2033264ab1b3a23

CONFIG_GRAPH_CLAUCODILLO_URL ?= https://codeload.github.com/ClauCoDillo/claucodillo/zip/$(CLAUCODILLO_COMMIT_HASH)

CLAUCODILLO_TARBALL := claucodillo-$(CLAUCODILLO_COMMIT_HASH).zip

UNPACK ?= unzip -o $(if $(V),,-q)
CURL ?= curl -L $(if $(V),,-Ss)

$(CLAUCODILLO_TARBALL):
	$(ECHO_BEGIN)"Downloading: $(CLAUCODILLO_TARBALL)"
	$(Q) $(CURL) -o $(CLAUCODILLO_TARBALL) $(CONFIG_GRAPH_CLAUCODILLO_URL)
	$(ECHO_END)

$(CLAUCODILLO_DIR_NAME): $(CLAUCODILLO_TARBALL)
	$(ECHO_BEGIN)"Unpacking: $(CLAUCODILLO_TARBALL) -> $(CLAUCODILLO_DIR_NAME)"
	$(Q) $(UNPACK) $(CLAUCODILLO_TARBALL)
	$(Q) mv claucodillo-$(CLAUCODILLO_COMMIT_HASH) $(CLAUCODILLO_DIR_NAME)
	$(Q) touch $(CLAUCODILLO_DIR_NAME)
	$(ECHO_END)

# Download and unpack if no git repo found in-tree (mirrors
# graphics/cfltk's and graphics/microwindows's pattern for third-party
# sources; this workspace instead symlinks claucodillo/ to its own
# top-level clone for local dev -- see that clone's own .git).
ifeq ($(wildcard $(CLAUCODILLO_DIR_NAME)/.git),)
context:: $(CLAUCODILLO_DIR_NAME)

distclean::
	$(call DELDIR, $(CLAUCODILLO_DIR_NAME))
	$(call DELFILE, $(CLAUCODILLO_TARBALL))
endif

CFLAGS += -std=c99

CSRCS += claucodillo/dlib/dlib.c

CSRCS += claucodillo/lout/container.c
CSRCS += claucodillo/lout/identity.c
CSRCS += claucodillo/lout/misc.c
CSRCS += claucodillo/lout/object.c
CSRCS += claucodillo/lout/signal.c
CSRCS += claucodillo/lout/unicode.c

CSRCS += claucodillo/dw/findtext.c
CSRCS += claucodillo/dw/imgrenderer.c
CSRCS += claucodillo/dw/iterator.c
CSRCS += claucodillo/dw/layout.c
CSRCS += claucodillo/dw/selection.c
CSRCS += claucodillo/dw/stackingcontextmgr.c
CSRCS += claucodillo/dw/style.c
CSRCS += claucodillo/dw/tools.c
CSRCS += claucodillo/dw/types.c
CSRCS += claucodillo/dw/ui.c
CSRCS += claucodillo/dw/view.c
CSRCS += claucodillo/dw/widget.c

include $(APPDIR)/Application.mk
