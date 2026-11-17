/* SDLInvaders for GFOS: the SDL_image 1.2 calls the game makes. GFOpenUp's
 * SDL 1.2 (the kit's sdl12-compat) comes without an SDL_image 1.2, so
 * amiga/img_datatypes.c reads the pictures with the system's own picture
 * datatypes (PNG and JPEG). Copyright (c) 2026 AmiCradle Team. GPL 2 or
 * later, like SDLInvaders. */
#ifndef AMIGA_SDL_IMAGE_H
#define AMIGA_SDL_IMAGE_H
#include <SDL/SDL.h>
#ifdef __cplusplus
extern "C" {
#endif
SDL_Surface *IMG_Load(const char *file);
#define IMG_GetError SDL_GetError
#define IMG_SetError SDL_SetError
#ifdef __cplusplus
}
#endif
#endif
