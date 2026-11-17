/* Host test for img_alpha_kind(): which pictures IMG_Load gives an alpha
 * surface, as SDL_image does.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SDLInvaders. */
#include <stdio.h>
#include <string.h>
#include "img_alpha.h"

static int fails;
static void check(const char *what, int got, int want)
{
    printf("%s %s\n", got == want ? "pass" : "FAIL", what);
    if (got != want)
        fails++;
}

/* w x h ARGB pixels, pitch padded by 8 bytes, every alpha byte a */
static void fill(unsigned char *p, int w, int h, int pitch, unsigned char a)
{
    int x, y;
    memset(p, 0x5A, (size_t)pitch * h); /* the padding must never count */
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
        {
            unsigned char *px = p + y * pitch + x * 4;
            px[0] = a; px[1] = 10; px[2] = 20; px[3] = 30;
        }
}

int main(void)
{
    enum { W = 5, H = 4, PITCH = W * 4 + 8 };
    unsigned char buf[PITCH * H];

    fill(buf, W, H, PITCH, 255);
    check("all 255: opaque (a JPEG, or a PNG without alpha)", img_alpha_kind(buf, W, H, PITCH), IMG_ALPHA_NONE);
    fill(buf, W, H, PITCH, 0);
    check("all 0: no alpha said (a datatype that leaves alpha 0)", img_alpha_kind(buf, W, H, PITCH) == IMG_ALPHA_USED, 0);
    fill(buf, W, H, PITCH, 255);
    buf[PITCH * 2 + 4 * 3] = 0;
    check("255 with one 0: transparency used", img_alpha_kind(buf, W, H, PITCH), IMG_ALPHA_USED);
    fill(buf, W, H, PITCH, 255);
    buf[PITCH * (H - 1) + 4 * (W - 1)] = 128;
    check("one 128 in the last pixel: alpha used", img_alpha_kind(buf, W, H, PITCH), IMG_ALPHA_USED);
    fill(buf, W, H, PITCH, 0);
    buf[0] = 77;
    check("one 77 in the first pixel: alpha used", img_alpha_kind(buf, W, H, PITCH), IMG_ALPHA_USED);
    check("empty picture: not used", img_alpha_kind(buf, 0, 0, PITCH) == IMG_ALPHA_USED, 0);
    printf("%s: %d failed\n", fails ? "FAIL" : "PASS", fails);
    return fails != 0;
}
