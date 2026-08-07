/*
 * apps/graphics/claucodillo/config.h
 *
 * Hand-maintained NuttX substitute for ClauCoDillo's normal
 * autotools-generated config.h (produced by ./configure from
 * configure.ac's AC_DEFINE/AC_CHECK_HEADERS/feature-toggle calls,
 * which this NuttX port doesn't run). Kept here, outside the
 * claucodillo/ source tree itself (added to the include path in
 * Make.defs), rather than inside it, since that tree is either a
 * pinned-commit download or (for local dev) a symlink to this
 * workspace's own claucodillo/ clone -- either way, not somewhere to
 * drop a NuttX-specific generated file.
 *
 * Starts minimal (only what dlib/ actually needs) and is meant to
 * grow alongside whatever piece of ClauCoDillo's build is being
 * ported next -- see docs/analysis.md in the ClauCoDillo_NuttX
 * project for current status. Not a general "define everything
 * configure.ac might set" stand-in.
 */
#ifndef APPS_GRAPHICS_CLAUCODILLO_CONFIG_H
#define APPS_GRAPHICS_CLAUCODILLO_CONFIG_H

/* dlib/d_size.h: NuttX's libc provides a real C99 <stdint.h>. */
#define HAVE_STDINT_H 1

/* lout/object.c's pointer_hashValue(): autotools sets this via
 * AC_CHECK_SIZEOF([void *]). Only the sim (x86_64 host) target exists
 * so far -- revisit this value before building for a 32-bit NuttX
 * target. */
#define SIZEOF_VOID_P 8

/* lout/misc.c's PRGNAME: normally auto-defined by autoconf/automake
 * from configure.ac's AC_INIT([dillo], [3.3.0-rc1]) -- kept in sync
 * with that line by hand here since this port doesn't run autotools. */
#define PACKAGE "dillo"
#define VERSION "3.3.0-rc1"

/* src/decode.c's char-encoding conversion: configure.ac probes
 * whether the local iconv() takes `const char **` (older libiconv) or
 * `char **` (newer) for its second argument, via AC_DEFINE([inbuf_t],
 * ...). NuttX's own <iconv.h> (libs/libc/locale/lib_iconv.c) declares
 * `size_t iconv(iconv_t, char **in, ...)` -- the newer/plain-char
 * convention. */
#define inbuf_t char

/* src/IO/mime.c's a_Mime_init() only registers a MIME-type-to-viewer
 * mapping for an image format if its own ENABLE_* macro is defined --
 * configure.ac sets these based on which decode libraries ./configure
 * actually found (see AC_DEFINE([ENABLE_PNG], ...) etc there). This
 * NuttX port never ran configure, so none of these were ever defined,
 * even though the decode libraries themselves (libpng/libjpeg/libwebp,
 * see this workspace's apps/graphics/{libpng,libjpeg,libwebp}) are
 * fully compiled and linked in. The decode *code* being present was
 * never the gap -- confirmed live via a full instrumented trace from
 * HTTP response down to a_Mime_get_viewer("image/png") returning NULL
 * (the *only* place this silently fails: no error, no crash, the
 * image's already-fully-downloaded data just gets handed to
 * Cache_null_client and discarded). GIF (src/gif.c) and SVG
 * (src/svg.c, via the bundled nanosvg.h/nanosvgrast.h header-only
 * library) need no external library at all, so are safe to enable
 * unconditionally alongside the three that do. */
#define ENABLE_PNG 1
#define ENABLE_JPEG 1
#define ENABLE_WEBP 1
#define ENABLE_GIF 1
#define ENABLE_SVG 1

#endif /* APPS_GRAPHICS_CLAUCODILLO_CONFIG_H */
