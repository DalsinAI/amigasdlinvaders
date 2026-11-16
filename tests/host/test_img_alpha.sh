#!/bin/sh
# Builds and runs tests/host/test_img_alpha.c with the host's cc.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
${CC:-cc} -std=c99 -Wall -Werror -I"$HERE/../../amiga" -o "$T/t" "$HERE/test_img_alpha.c"
"$T/t"
