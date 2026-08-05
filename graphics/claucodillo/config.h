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

#endif /* APPS_GRAPHICS_CLAUCODILLO_CONFIG_H */
