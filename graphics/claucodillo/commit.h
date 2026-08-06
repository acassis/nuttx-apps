/*
 * apps/graphics/claucodillo/commit.h
 *
 * Hand-maintained NuttX substitute for the commit.h that
 * src/Makefile.am normally generates at build time (via `git
 * describe --dirty` if GIT_AVAILABLE, or an empty file otherwise --
 * see configure.ac). This port doesn't run that step. Deliberately
 * empty: src/version.c's use of GIT_COMMIT is already `#ifdef`-
 * guarded, so omitting the define entirely is the correct "no git
 * info available" case, not a stand-in needing a real value.
 */
