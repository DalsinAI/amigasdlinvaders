#!/bin/sh
# The release archive and the GFOpenUp part, for Cradle's stove_build
# (target package):
#  1. upstream SDLInvaders 0.7.6 and libmikmod 3.3.16, fetched as the
#     tarballs package/sdlinvaders.lock names and checked against its
#     sha256 sums (neither is kept in this repository), with amiga/patches
#     applied to SDLInvaders;
#  2. the game built by amiga/Makefile.amigaos;
#  3. the drawer, its icon and the Installer script beside it packed as
#     NAME-VERSION-68040.lha (tools/pack_dist.py), with its .sha256 and a
#     short .readme;
#  4. OUT/Parts/SDLInvaders, a GFOpenUp part: its Part file and the drawer's
#     files for SYS:Games/SDLInvaders, installed with
#       GFOpenUpTool INSTALL Parts/SDLInvaders
#     It needs the GFOpenGPU part, whose SDL2.module draws the game (through
#     sdl12-compat, linked in).
# Nothing is published from here.
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SDLInvaders.
set -eu

HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/.." && pwd)
. "$HERE/package.env"
. "$HERE/sdlinvaders.lock"
OUT=${OUT:-"$ROOT/build-amiga-package"}
mkdir -p "$OUT"
OUT=$(CDPATH= cd -- "$OUT" && pwd)
STOVE=${STOVE:-"$HOME/AmiCradle/stoves/os32-gcc16-open6"}
# outside the checkout: Cradle cleans it (git clean -ffdx) before every job
WORK=${WORK:-"$HOME/.cache/amigasdlinvaders"}
mkdir -p "$WORK"
cd "$ROOT"

# fetch URL SHA256 FILE: download once, always check the sum
fetch() {
    if [ ! -f "$WORK/$3" ] || ! echo "$2  $WORK/$3" | sha256sum -c - >/dev/null 2>&1; then
        curl -fsSL -o "$WORK/$3.part" "$1"
        mv "$WORK/$3.part" "$WORK/$3"
    fi
    echo "$2  $WORK/$3" | sha256sum -c - >/dev/null || { echo "sha256 of $3 is wrong" >&2; exit 1; }
}

# 1. upstream, unpacked and patched afresh every time
fetch "$SDLINVADERS_URL" "$SDLINVADERS_SHA256" SDLInvaders.tar.bz2
fetch "$LIBMIKMOD_URL" "$LIBMIKMOD_SHA256" libmikmod.tar.gz
SI="$WORK/SDLInvaders"; MM="$WORK/libmikmod"
rm -rf "$SI" "$MM" "$WORK/libmikmod-3.3.16"
tar -xjf "$WORK/SDLInvaders.tar.bz2" -C "$WORK"
tar -xzf "$WORK/libmikmod.tar.gz" -C "$WORK"
mv "$WORK/libmikmod-3.3.16" "$MM"
for p in "$ROOT"/amiga/patches/*.patch; do
    (cd "$SI" && patch -p1 -s < "$p")
done

# 2. the game (a stove's compiler is in its prefix/, or STOVE is the prefix)
if [ -x "$STOVE/prefix/bin/m68k-amigaos-gcc" ]; then PREFIX="$STOVE/prefix"; else PREFIX="$STOVE"; fi
make -f amiga/Makefile.amigaos SI="$SI" MM="$MM" STOVE="$STOVE" PREFIX="$PREFIX" -j4 $TARGET > "$OUT/build-output.txt" 2>&1 || { tail -n 60 "$OUT/build-output.txt"; exit 1; }

# 3. the archive
python3 "$ROOT/tools/pack_dist.py" --dist "$DIST" --name "$NAME" --out "$OUT"

# 4. the GFOpenUp part
sh "$ROOT/package/write-part.sh" "$DIST/$NAME" "$OUT/Parts/$NAME" "$NAME" "$PORT_VERSION" \
    "SDLInvaders: Space Invaders with a night city, rockets and MOD music"
echo "part: $(grep -c '^copy' "$OUT/Parts/$NAME/Part") files, $(du -sk "$OUT/Parts/$NAME" | cut -f1) KB"
ls -l "$DIST/$NAME/$NAME"
