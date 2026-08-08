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

# configure.ac's --enable-threaded-dns is on by default
# (-DD_DNS_THREADED, "advantage of a reentrant resolver library") --
# without it, src/dns.c's Dns_server_req() calls getaddrinfo()
# directly and synchronously on the main thread/event loop instead of
# spawning a real pthread, blocking the whole browser UI for the
# duration of every DNS lookup. NuttX's netdb resolver is reentrant
# (same property upstream's flag is named for) and CONFIG_PTHREAD is
# on by default on this board, so there's no reason to keep the
# synchronous fallback here.
CFLAGS += -DD_DNS_THREADED

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

# Same story again, for src/IO/dpi.c's dpi-plugin-executable search
# path (src/IO/Makefile.am: -DDILLO_BINDIR='"$(bindir)/"').
CFLAGS += -DDILLO_BINDIR=\"/etc/dillo/bin/\"

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

# src/: the browser itself.
# decode.c/version.c/jpeg.c/form.c/png.c/webp.c (zlib/libjpeg/iconv/
# libpng/libwebp) are NOT excluded -- see below, each right before the
# CONFIG_* it actually needs.
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

# localfile.c: new file, not part of upstream ClauCoDillo -- an
# in-process file:// loader replacing the "file" dpi plugin (which
# needs a dpid daemon this port doesn't implement). See its own
# top-of-file comment for the full rationale.
CSRCS += claucodillo/src/localfile.c

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

# png.c: needs <png.h>/<libpng/png.h> (new apps/graphics/libpng
# package, vendored for this port -- no existing NuttX package
# provided it; exports its own include path globally via its own
# Make.defs, same as libjpeg).
ifneq ($(CONFIG_LIBPNG),)
CSRCS += claucodillo/src/png.c
endif

# webp.c: needs <webp/decode.h> (new apps/graphics/libwebp package,
# vendored for this port -- no existing NuttX package provided it;
# exports its own include path globally via its own Make.defs, same
# as libjpeg/libpng). Only the decode-only file set is built, since
# that's all webp.c's own #includes ever need.
ifneq ($(CONFIG_LIBWEBP),)
CSRCS += claucodillo/src/webp.c
endif

# dns.c: real networking (getaddrinfo()-based resolver) -- compiles
# clean against NuttX's own <netdb.h>/<sys/socket.h> (one
# -Wimplicit-function-declaration warning for gai_strerror(), which
# NuttX's netdb.h doesn't declare -- harmless unless something
# actually calls into this file, which nothing does yet). The *design*
# question from docs/analysis.md's Linux-assumption audit (dpi's
# fork/exec daemon-of-daemons process model) is still open and
# deferred -- compiling this file is not the same as that being
# solved, same "compile now, defer real wiring" reasoning as
# cookies.c/actions.c/bookmark.c from the original src/ batch.
CSRCS += claucodillo/src/dns.c

# dillo.c: main() itself, the whole point of this package. Needed
# CONFIG_SCHED_WAITPID (see boards/sim/.../claucodillo_build/defconfig
# -- raw_sigchld2()'s waitpid(..., WNOHANG) needs WNOHANG, which
# NuttX's <sys/wait.h> only declares under that Kconfig option).
#
# Built as MAINSRC, not CSRCS, so it becomes a real registered NuttX
# application (same shape as examples/hello's own MAINSRC/PROGNAME/
# PRIORITY/STACKSIZE) instead of an inert unreferenced archive member.
# Application.mk automatically compiles MAINSRC with
# `-Dmain=<PROGNAME>_main` (see its `rename "main()" ... to
# "xxx_main()"` support), so dillo.c's own `int main(...)` becomes
# `dillo_main` at the object-code level with no source patch needed --
# it no longer collides with the sim architecture's own
# arch/sim/src/sim/sim_head.c `main()`.
MAINSRC = claucodillo/src/dillo.c
PROGNAME = $(CONFIG_GRAPHICS_CLAUCODILLO_PROGNAME)
PRIORITY = $(CONFIG_GRAPHICS_CLAUCODILLO_PRIORITY)
STACKSIZE = $(CONFIG_GRAPHICS_CLAUCODILLO_STACKSIZE)

# src/IO/: network I/O layer dillo.c actually calls into. TLS backend
# is tls_mbedtls.c (matches upstream's USE_MBEDTLS configuration --
# src/IO/Makefile.am's TLS_MBEDTLS variable) using apps/crypto/mbedtls,
# which brings its own include path onto CFLAGS globally via its own
# Make.defs whenever CONFIG_CRYPTO_MBEDTLS=y (board defconfig), no
# extra INCDIR needed here. ENABLE_TLS/HAVE_MBEDTLS are defined in
# config.h. tls_openssl.c is not built -- this port only ever wires up
# one TLS backend.
CSRCS += claucodillo/src/IO/about.c
CSRCS += claucodillo/src/IO/mime.c
CSRCS += claucodillo/src/IO/proto.c
CSRCS += claucodillo/src/IO/tls.c
CSRCS += claucodillo/src/IO/tls_mbedtls.c
CSRCS += claucodillo/src/IO/IO.c
CSRCS += claucodillo/src/IO/iowatch.c
CSRCS += claucodillo/src/IO/control.c
CSRCS += claucodillo/src/IO/http.c
CSRCS += claucodillo/src/IO/dpi.c

# dpip/: the wire protocol dpi.c speaks to dpi plugins over (currently
# unused at runtime -- no dpi plugin daemon exists in this port yet,
# see docs/analysis.md's Linux-assumption audit -- but dpi.c calls
# into these functions directly, so they need to actually be linkable,
# not just declared).
CSRCS += claucodillo/dpip/dpip.c

include $(APPDIR)/Application.mk
