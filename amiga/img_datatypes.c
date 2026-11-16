/* IMG_Load for SDLInvaders on GFOS, on the system's picture datatypes
 * (datatypes.library 43 or later: PNG, JPEG, IFF and any other picture a
 * datatype is installed for). Each picture is read as 32-bit ARGB with
 * PDTM_READPIXELARRAY into a 32-bit SDL surface. A picture whose alpha
 * bytes say anything (some neither 0 nor 255, or a mix of 0 and 255) gets
 * an alpha surface (Amask 0xFF000000), as SDL_image gives for a PNG with
 * alpha or transparency; one with no alpha is opaque (Amask 0), as SDL_image
 * gives for a JPEG. img_alpha_kind() is that decision, on its own so the
 * host test can check it.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SDLInvaders. */
#include "img_alpha.h"

#ifdef __amigaos__
#include <SDL/SDL.h>
#include "SDL_image.h"
#include <exec/types.h>
#include <intuition/classusr.h>
#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <stdlib.h>
#include <string.h>

struct Library *DataTypesBase;

static int open_datatypes(void)
{
    if (DataTypesBase == NULL)
        DataTypesBase = OpenLibrary((CONST_STRPTR)"datatypes.library", 43);
    return DataTypesBase != NULL;
}

static void close_datatypes(void)
{
    if (DataTypesBase != NULL)
    {
        CloseLibrary(DataTypesBase);
        DataTypesBase = NULL;
    }
}

SDL_Surface *IMG_Load(const char *file)
{
    Object *obj;
    struct BitMapHeader *bmh = NULL;
    SDL_Surface *s;
    ULONG w, h;
    static int registered;

    if (!open_datatypes())
    {
        SDL_SetError("IMG_Load: no datatypes.library 43");
        return NULL;
    }
    if (!registered)
    {
        atexit(close_datatypes);
        registered = 1;
    }
    obj = NewDTObject((APTR)file,
                      DTA_GroupID, GID_PICTURE,
                      PDTA_DestMode, PMODE_V43,
                      PDTA_Remap, FALSE,
                      TAG_DONE);
    if (obj == NULL)
    {
        SDL_SetError("IMG_Load: no picture datatype read %s (error %ld)", file, (long)IoErr());
        return NULL;
    }
    DoMethod(obj, DTM_PROCLAYOUT, NULL, 1);
    if (GetDTAttrs(obj, PDTA_BitMapHeader, (ULONG)&bmh, TAG_DONE) != 1 || bmh == NULL)
    {
        DisposeDTObject(obj);
        SDL_SetError("IMG_Load: %s has no size", file);
        return NULL;
    }
    w = bmh->bmh_Width;
    h = bmh->bmh_Height;
    /* big-endian ARGB: the bytes A R G B are the Uint32 0xAARRGGBB */
    s = SDL_CreateRGBSurface(SDL_SWSURFACE, (int)w, (int)h, 32,
                             0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (s == NULL)
    {
        DisposeDTObject(obj);
        return NULL;
    }
    SDL_LockSurface(s);
    DoMethod(obj, PDTM_READPIXELARRAY, (ULONG)s->pixels, PBPAFMT_ARGB,
             (ULONG)s->pitch, 0, 0, w, h);
    {
        int kind = img_alpha_kind((const unsigned char *)s->pixels, (int)w, (int)h, s->pitch);
        SDL_UnlockSurface(s);
        DisposeDTObject(obj);
        if (kind != IMG_ALPHA_USED)
        {
            /* opaque: the same pixels as a surface without alpha, and a
             * full alpha byte so a later SDL_DisplayFormat keeps colours */
            SDL_Surface *o = SDL_CreateRGBSurface(SDL_SWSURFACE, (int)w, (int)h, 32,
                                                  0x00FF0000, 0x0000FF00, 0x000000FF, 0);
            if (o != NULL)
            {
                int y;
                SDL_LockSurface(o);
                SDL_LockSurface(s);
                for (y = 0; y < (int)h; y++)
                    memcpy((Uint8 *)o->pixels + y * o->pitch,
                           (Uint8 *)s->pixels + y * s->pitch, w * 4);
                SDL_UnlockSurface(s);
                SDL_UnlockSurface(o);
                SDL_FreeSurface(s);
                s = o;
            }
        }
    }
    return s;
}
#endif
