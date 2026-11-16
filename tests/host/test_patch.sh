#!/bin/sh
# Applies amiga/patches to upstream SDLInvaders (fetched and checked as
# package/sdlinvaders.lock says, or SI=/an/unpacked/tree) and preprocesses
# main.c for AmigaOS and for Linux against stub SDL headers: the Amiga
# build keeps its high scores in User/, guards a missing table, makes a
# Workbench start (argc 0) read no arguments, opens a window rather than
# a screen by default and calls amiga_setup before
# anything else; the Linux build stays as upstream.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/../.." && pwd)
. "$ROOT/package/sdlinvaders.lock"
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
if [ -z "${SI:-}" ]; then
    curl -fsSL -o "$T/si.tbz" "$SDLINVADERS_URL"
    echo "$SDLINVADERS_SHA256  $T/si.tbz" | sha256sum -c - >/dev/null
    tar -xjf "$T/si.tbz" -C "$T"
    SI="$T/SDLInvaders"
else
    cp -r "$SI" "$T/SDLInvaders"; SI="$T/SDLInvaders"
fi
for p in "$ROOT"/amiga/patches/*.patch; do (cd "$SI" && patch -p1 -s < "$p"); done
CPP="${CC:-cc} -E -P -I$HERE/stub -I$HERE/stub/SDL -I$ROOT/amiga -I$SI"
$CPP -D__amigaos__ "$SI/main.c" > "$T/amiga.i"
$CPP "$SI/main.c" > "$T/linux.i"
fails=0
check() { if eval "$2"; then echo "pass $1"; else echo "FAIL $1"; fails=$((fails+1)); fi; }
check "Amiga: scores in User/highscores, twice" '[ "$(grep -c "\"User/highscores\"" "$T/amiga.i")" = 2 ]'
check "Amiga: no data/highscores" '! grep -q "data/highscores" "$T/amiga.i"'
check "Amiga: save_scores returns when it cannot write" 'grep -A12 "void save_scores" "$T/amiga.i" | grep -q "return;"'
check "Amiga: a missing table is shown empty, not read" 'grep -A30 "void show_highscore" "$T/amiga.i" | grep -q "strcpy(highscore\[i\].name, \"-\")" && [ "$(grep -c "if (file!=" "$T/amiga.i")" = 1 ]'
check "Amiga: Workbench argv" 'grep -q "wb_argv\[\] = { \"SDLInvaders\"" "$T/amiga.i"'
check "Amiga: amiga_setup before ReadCommandLine" 'awk "/int main/{m=1} m&&/amiga_setup\(\)/{s=NR} m&&/ReadCommandLine\(argv\)/{r=NR} END{exit !(s&&r&&s<r)}" "$T/amiga.i"'
check "Amiga: a window by default" 'grep -q "^int fullscreen=0;" "$T/amiga.i"'
check "Linux: data/highscores, twice" '[ "$(grep -c "\"data/highscores\"" "$T/linux.i")" = 2 ]'
check "Linux: no amiga_setup, no wb_argv" '! grep -q "amiga_setup\|wb_argv\|User/highscores" "$T/linux.i"'
check "Linux: full screen by default, as upstream" 'grep -q "^int fullscreen=1;" "$T/linux.i"'
echo "$( [ $fails = 0 ] && echo PASS || echo FAIL ): $fails failed"
[ $fails = 0 ]
