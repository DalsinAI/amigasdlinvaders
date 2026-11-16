/* Whether a 32-bit ARGB picture (A first in each pixel's four bytes) uses
 * its alpha: IMG_ALPHA_USED when some alpha byte is neither 0 nor 255, or
 * when both 0 and 255 occur; IMG_ALPHA_NONE when every alpha byte is the
 * same 0 or 255 (a datatype that has no alpha for the picture fills them
 * all with one value). Copyright (c) 2026 AmiCradle Team. GPL 2 or later,
 * like SDLInvaders. */
#ifndef AMIGA_IMG_ALPHA_H
#define AMIGA_IMG_ALPHA_H
#include <string.h>
#include <stdlib.h>

enum { IMG_ALPHA_NONE = 0, IMG_ALPHA_USED = 1 };

static inline int img_alpha_kind(const unsigned char *pixels, int w, int h, int pitch)
{
    int seen0 = 0, seen255 = 0, x, y;
    for (y = 0; y < h; y++)
    {
        const unsigned char *p = pixels + (size_t)y * (size_t)pitch;
        for (x = 0; x < w; x++, p += 4)
        {
            if (*p == 0)
                seen0 = 1;
            else if (*p == 255)
                seen255 = 1;
            else
                return IMG_ALPHA_USED;
            if (seen0 && seen255)
                return IMG_ALPHA_USED;
        }
    }
    return IMG_ALPHA_NONE;
}
#endif
