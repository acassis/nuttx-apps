############################################################################
# apps/graphics/libwebp/Makefile
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

# libwebp needs no generated config.h at all for a plain portable-C
# build like this one (unlike libpng/libjpeg) -- its sources only pull
# in src/webp/config.h when HAVE_CONFIG_H is defined, which nothing
# here defines, so they fall back to their own built-in defaults.

LIBWEBP_TARBALL := libwebp-$(CONFIG_LIBWEBP_VER).tar.gz

libwebp.tar.gz:
	$(Q) curl -L https://github.com/webmproject/libwebp/archive/refs/tags/v$(CONFIG_LIBWEBP_VER).tar.gz -o libwebp.tar.gz
	$(Q) tar xf libwebp.tar.gz
	$(Q) mv libwebp-$(CONFIG_LIBWEBP_VER) libwebp
	$(Q) patch -p1 -d libwebp < \
		0001-thread_utils-avoid-OK-enumerator-clash-with-nuttx-sys-types.patch

# Download and unpack tarball if no git repo found
ifeq ($(wildcard libwebp/.git),)
context:: libwebp.tar.gz

distclean::
	$(call DELDIR, libwebp)
	$(call DELFILE, libwebp.tar.gz)
endif

CFLAGS += -Wno-shadow -Wno-strict-prototypes -Wno-undef -Wno-array-bounds

# Decode-only file set -- mirrors upstream's own "libwebpdecoder"
# targets (src/dec/Makefile.am's libwebpdecode_la_SOURCES, and the
# BUILD_LIBWEBPDECODER branches of src/dsp/Makefile.am and
# src/utils/Makefile.am), minus the SIMD-accelerated variants (see
# this package's Kconfig help text for why those are skipped).

# src/dec: the actual VP8/VP8L bitstream + incremental decoders.
CSRCS += libwebp/src/dec/alpha_dec.c
CSRCS += libwebp/src/dec/buffer_dec.c
CSRCS += libwebp/src/dec/frame_dec.c
CSRCS += libwebp/src/dec/idec_dec.c
CSRCS += libwebp/src/dec/io_dec.c
CSRCS += libwebp/src/dec/quant_dec.c
CSRCS += libwebp/src/dec/tree_dec.c
CSRCS += libwebp/src/dec/vp8_dec.c
CSRCS += libwebp/src/dec/vp8l_dec.c
CSRCS += libwebp/src/dec/webp_dec.c

# src/dsp: portable-C pixel/colorspace/rescaling routines the decoder
# needs (encoder-only routines like cost.c/ssim.c are not built).
CSRCS += libwebp/src/dsp/alpha_processing.c
CSRCS += libwebp/src/dsp/cpu.c
CSRCS += libwebp/src/dsp/dec.c
CSRCS += libwebp/src/dsp/dec_clip_tables.c
CSRCS += libwebp/src/dsp/filters.c
CSRCS += libwebp/src/dsp/lossless.c
CSRCS += libwebp/src/dsp/rescaler.c
CSRCS += libwebp/src/dsp/upsampling.c
CSRCS += libwebp/src/dsp/yuv.c

# src/utils: shared helpers the decoder needs (encoder-only helpers
# like bit_writer_utils.c are not built).
CSRCS += libwebp/src/utils/bit_reader_utils.c
CSRCS += libwebp/src/utils/color_cache_utils.c
CSRCS += libwebp/src/utils/filters_utils.c
CSRCS += libwebp/src/utils/huffman_utils.c
CSRCS += libwebp/src/utils/palette.c
CSRCS += libwebp/src/utils/quant_levels_dec_utils.c
CSRCS += libwebp/src/utils/rescaler_utils.c
CSRCS += libwebp/src/utils/random_utils.c
CSRCS += libwebp/src/utils/thread_utils.c
CSRCS += libwebp/src/utils/utils.c

MODULE = $(CONFIG_LIBWEBP)

include $(APPDIR)/Application.mk
