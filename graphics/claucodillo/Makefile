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

# ClauCoDillo: dlib, lout, and all of dw (dw-core + dw-widgets +
# dw-fltk, dw's platform-independent pieces plus its cfltk-specific
# platform glue) so far. Same incremental shape as graphics/cfltk's
# own first pass. src/ (the browser itself) comes in follow-up
# commits.

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

# Normally set by autotools from the configured install prefix
# (dw/Makefile.am: -DDILLO_LIBDIR='"$(pkglibdir)/"'), used by
# dw/hyphenator.c to locate hyphenation pattern files at runtime
# (hyph_<lang>.dic). No NuttX filesystem-layout decision has been made
# yet for where such runtime assets live (see docs/analysis.md's
# Linux-assumption audit re: the similar ~/.dillo/TLS-CA-bundle
# questions) -- this is a placeholder so the source compiles; revisit
# once hyphenation is actually wired up and tested.
CFLAGS += -DDILLO_LIBDIR=\"/etc/dillo/\"

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

CSRCS += claucodillo/dw/alignedtablecell.c
CSRCS += claucodillo/dw/alignedtextblock.c
CSRCS += claucodillo/dw/bullet.c
CSRCS += claucodillo/dw/hyphenator.c
CSRCS += claucodillo/dw/image.c
CSRCS += claucodillo/dw/listitem.c
CSRCS += claucodillo/dw/oofawarewidget.c
CSRCS += claucodillo/dw/ooffloatsmgr.c
CSRCS += claucodillo/dw/oofposabslikemgr.c
CSRCS += claucodillo/dw/oofposabsmgr.c
CSRCS += claucodillo/dw/oofposfixedmgr.c
CSRCS += claucodillo/dw/oofpositionedmgr.c
CSRCS += claucodillo/dw/oofposrelmgr.c
CSRCS += claucodillo/dw/regardingborder.c
CSRCS += claucodillo/dw/ruler.c
CSRCS += claucodillo/dw/simpletablecell.c
CSRCS += claucodillo/dw/table.c
CSRCS += claucodillo/dw/tablecell.c
CSRCS += claucodillo/dw/textblock.c

CSRCS += claucodillo/dw/fltkflatview.c
CSRCS += claucodillo/dw/fltkimgbuf.c
CSRCS += claucodillo/dw/fltkmisc.c
CSRCS += claucodillo/dw/fltkplatform.c
CSRCS += claucodillo/dw/fltkui.c
CSRCS += claucodillo/dw/fltkviewbase.c
CSRCS += claucodillo/dw/fltkviewport.c

include $(APPDIR)/Application.mk
