SDLInvaders for GFOS
====================

A port of SDLInvaders 0.7.6 by Niels Weber and its contributors, a
Space Invaders game over a night city with rockets, seek rockets, bombs
and MOD music, from https://sourceforge.net/projects/sdlinvaders/ (GPL 2
or later), for GFOS: AmigaOS 3.2 with GFOpenUp, on a 68040 or 68060 with
an FPU, on GFOpenUp's SDL 2 through sdl12-compat.

Status: built in Cradle's os32-gcc16-open6 stove; lab test pending.


What you need
-------------

- GFOS: AmigaOS 3.2 with GFOpenUp 0.8.26 or later, which installs
  LIBS:GFOpenGPU/SDL2.module, and the PNG and JPEG picture datatypes
  (AmigaOS 3.2 has both).
- A 68040 or 68060 with an FPU (not an LC040 or EC040), an RTG screen
  with room for an 800 x 600 window (or the AmiCradle lab), and about
  6 MB of free memory.


How to play
-----------

Install it one of three ways:

- GFOpenUp: `GFOpenUpTool INSTALL Parts/SDLInvaders` puts it in
  SYS:Games/SDLInvaders.
- Run Install-SDLInvaders from the archive (it offers Work:Games).
- Or copy the SDLInvaders drawer where you want it.

Then double-click SDLInvaders. The keys are in the drawer's Controls
file. The high scores are kept in the drawer's User/highscores; an
update by Install-SDLInvaders or GFOpenUp leaves User/ alone.


Building
--------

    sh package/build-amiga.sh        (STOVE=/path/to/os32-gcc16-open6)

or, in Cradle, stove_build with the os32-gcc16-open6 stove and target
package. It fetches SDLInvaders 0.7.6 and libmikmod 3.3.16 as the
tarballs package/sdlinvaders.lock names, checks their sha256 sums,
applies amiga/patches, builds with amiga/Makefile.amigaos and packs
SDLInvaders-VERSION-68040.lha and the GFOpenUp part Parts/SDLInvaders.
Neither upstream is kept in this repository.

Host tests:

- `sh tests/host/test_patch.sh` applies the patch to upstream and
  preprocesses main.c for AmigaOS and Linux: the Amiga build keeps its
  scores in User/highscores, copes with a missing table, reads no
  arguments on a Workbench start and calls amiga_setup first; the Linux
  build stays as upstream.
- `sh tests/host/test_img_alpha.sh` checks which pictures IMG_Load gives
  an alpha surface (img_alpha_kind), as SDL_image does.


What was changed
----------------

- amiga/patches/0001-amigaos.patch (main.c): the high scores in
  User/highscores (seeded from upstream's data/highscores on the first
  start), a missing or unwritable table no longer crashes, a Workbench
  start (argc 0) reads no arguments, and amiga_setup() runs first.
- amiga/amiga_start.c: 128 KB stack, $VER, the game moves to its own
  drawer (PROGDIR:) so data/ is found from any Shell, and back at exit;
  User/ is made on the first start.
- amiga/img_datatypes.c: IMG_Load on the system's picture datatypes
  (PNG, JPEG, IFF, ...), read as 32-bit ARGB; a picture whose alpha says
  something gets an alpha surface, others an opaque one (amiga/img_alpha.h).
  No libpng or libjpeg is built in.
- amiga/sdl_mixer_lite.c: the SDL_mixer 1.2 calls the game makes, on SDL
  1.2's audio: WAV sounds and the ProTracker music through libmikmod's
  MOD loader and player (fetched at build time, only those sources).
