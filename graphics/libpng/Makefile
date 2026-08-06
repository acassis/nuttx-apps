############################################################################
# apps/graphics/libpng/Makefile
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

# libpng has no autotools/CMake run here to generate pnglibconf.h the
# normal way -- upstream ships a ready-made scripts/pnglibconf.h.prebuilt
# for exactly this case (any build system without its own configure
# step), copied into place below. Same shape as apps/graphics/libjpeg's
# own pre-generated-config-header pattern (that one via a patch file
# instead, since libjpeg's jconfig.h has no prebuilt equivalent
# upstream ships directly).

LIBPNG_TARBALL := libpng-$(CONFIG_LIBPNG_VER).tar.gz

libpng.tar.gz:
	$(Q) curl -L https://github.com/pnggroup/libpng/archive/refs/tags/v$(CONFIG_LIBPNG_VER).tar.gz -o libpng.tar.gz
	$(Q) tar xf libpng.tar.gz
	$(Q) mv libpng-$(CONFIG_LIBPNG_VER) libpng
	$(Q) cp libpng/scripts/pnglibconf.h.prebuilt libpng/pnglibconf.h

# Download and unpack tarball if no git repo found
ifeq ($(wildcard libpng/.git),)
context:: libpng.tar.gz

distclean::
	$(call DELDIR, libpng)
	$(call DELFILE, libpng.tar.gz)
endif

# libpng's own png.h #includes <zlib.h> directly -- needs zlib's
# include path too (apps/system/zlib doesn't export it globally, see
# graphics/claucodillo's own Makefile for the same situation).
ifneq ($(CONFIG_LIB_ZLIB),)
CFLAGS += ${INCDIR_PREFIX}$(APPDIR)/system/zlib/zlib
endif

CFLAGS += -Wno-shadow -Wno-strict-prototypes -Wno-undef -Wno-array-bounds

# Read/decode side only -- see this package's Kconfig help text for
# why the write-side files aren't built.
CSRCS += libpng/png.c
CSRCS += libpng/pngerror.c
CSRCS += libpng/pngget.c
CSRCS += libpng/pngmem.c
CSRCS += libpng/pngpread.c
CSRCS += libpng/pngread.c
CSRCS += libpng/pngrio.c
CSRCS += libpng/pngrtran.c
CSRCS += libpng/pngrutil.c
CSRCS += libpng/pngset.c
CSRCS += libpng/pngtrans.c

MODULE = $(CONFIG_LIBPNG)

include $(APPDIR)/Application.mk
