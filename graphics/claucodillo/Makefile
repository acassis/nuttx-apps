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

# Same story as DILLO_LIBDIR above, for src/paths.c's system-wide
# config-file search path (src/Makefile.am:
# -DDILLO_SYSCONF='"$(sysconfdir)/"', where configure.ac sets
# sysconfdir=${sysconfdir}/${PACKAGE} -- i.e. normally .../etc/dillo/
# under whatever prefix was configured). Same placeholder, same "no
# NuttX filesystem-layout decision made yet" caveat.
CFLAGS += -DDILLO_SYSCONF=\"/etc/dillo/\"

# Same story again, for src/ui.c's "user_help.html" doc path
# (src/Makefile.am: -DDILLO_DOCDIR='"$(docdir)/"').
CFLAGS += -DDILLO_DOCDIR=\"/etc/dillo/doc/\"

# apps/system/zlib provides <zlib.h>, but (unlike apps/graphics/libjpeg)
# doesn't export its include path globally via its own Make.defs -- add
# it here instead, conditionally, matching what CONFIG_LIB_ZLIB actually
# enables.
ifneq ($(CONFIG_LIB_ZLIB),)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/system/zlib/zlib
endif

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

# src/: the browser itself. Excluded for now (image/compression
# external libs not yet available on NuttX, or genuinely part of the
# deferred dpi/networking milestone -- see docs/analysis.md's
# Linux-assumption audit and its "defer dpi" decision):
#   webp.c, png.c    (need libwebp/libpng -- no NuttX package yet)
#   dns.c            (real networking, deferred with dpi)
#   dillo.c          (main(): needs the rest of src/ first)
# decode.c/version.c (zlib, now apps/system/zlib) and jpeg.c/form.c
# (libjpeg/iconv, both already available) are NOT excluded -- see
# below, each right before the CONFIG_* it actually needs.
CSRCS += claucodillo/src/paths.c
CSRCS += claucodillo/src/tipwin.c
CSRCS += claucodillo/src/ui.c
CSRCS += claucodillo/src/uicmd.c
CSRCS += claucodillo/src/bw.c
CSRCS += claucodillo/src/cookies.c
CSRCS += claucodillo/src/actions.c
CSRCS += claucodillo/src/hsts.c
CSRCS += claucodillo/src/auth.c
CSRCS += claucodillo/src/md5.c
CSRCS += claucodillo/src/digest.c
CSRCS += claucodillo/src/colors.c
CSRCS += claucodillo/src/misc.c
CSRCS += claucodillo/src/history.c
CSRCS += claucodillo/src/prefs.c
CSRCS += claucodillo/src/prefsparser.c
CSRCS += claucodillo/src/keys.c
CSRCS += claucodillo/src/url.c
CSRCS += claucodillo/src/bitvec.c
CSRCS += claucodillo/src/klist.c
CSRCS += claucodillo/src/chain.c
CSRCS += claucodillo/src/utf8.c
CSRCS += claucodillo/src/timeout.c
CSRCS += claucodillo/src/dialog.c
CSRCS += claucodillo/src/web.c
CSRCS += claucodillo/src/nav.c
CSRCS += claucodillo/src/cache.c
CSRCS += claucodillo/src/dicache.c
CSRCS += claucodillo/src/capi.c
CSRCS += claucodillo/src/domain.c
CSRCS += claucodillo/src/css.c
CSRCS += claucodillo/src/cssparser.c
CSRCS += claucodillo/src/styleengine.c
CSRCS += claucodillo/src/plain.c
CSRCS += claucodillo/src/html.c
CSRCS += claucodillo/src/table.c
CSRCS += claucodillo/src/bookmark.c
CSRCS += claucodillo/src/gif.c
CSRCS += claucodillo/src/svg.c
CSRCS += claucodillo/src/imgbuf.c
CSRCS += claucodillo/src/image.c
CSRCS += claucodillo/src/menu.c
CSRCS += claucodillo/src/dpiapi.c
CSRCS += claucodillo/src/findbar.c
CSRCS += claucodillo/src/xembed.c

# form.c: needs <iconv.h>, built into NuttX's own libc (not a missing
# external library the way it first looked -- CONFIG_LIBC_LOCALE gates
# the real implementation; the header/prototypes resolve either way).
CSRCS += claucodillo/src/form.c

# decode.c/version.c: need <zlib.h> (apps/system/zlib, see CFLAGS
# above). version.c also references a_Jpeg_version()/a_Png_version()/
# a_Webp_version() via djpeg.h/dpng.h/dwebp.h, but those are pure
# declaration headers (no jpeglib.h/png.h/webp/decode.h pulled in at
# this level) -- compiles fine even before jpeg.c/png.c/webp.c
# themselves are all in place, same "compiling only needs headers, not
# a real link yet" reasoning as the rest of this package so far.
ifneq ($(CONFIG_LIB_ZLIB),)
CSRCS += claucodillo/src/decode.c
CSRCS += claucodillo/src/version.c
endif

# jpeg.c: needs <jpeglib.h> (apps/graphics/libjpeg, already exports its
# own include path globally via its own Make.defs).
ifneq ($(CONFIG_LIBJPEG),)
CSRCS += claucodillo/src/jpeg.c
endif

include $(APPDIR)/Application.mk
