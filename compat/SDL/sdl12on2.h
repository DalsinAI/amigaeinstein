/* The SDL 1.2 calls Einstein makes, on SDL 2 (GFOpenUp's SDL2.module):
 * one window whose surface is the 1.2 "screen". Einstein's own SDL/...
 * includes find these headers first (-Icompat).
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#ifndef SDL12ON2_H
#define SDL12ON2_H
#include <SDL.h>

typedef SDL_Keycode SDLKey;

#define SDL_SWSURFACE   0
#define SDL_HWSURFACE   0
#define SDL_SRCCOLORKEY SDL_TRUE
#ifndef SDL_OPENGL
#define SDL_OPENGL      0
#endif
#define SDL_FULLSCREEN  0x80000000u
/* Exposed, moved, shown: Einstein draws everything again. */
#define SDL_VIDEOEXPOSE SDL_WINDOWEVENT
#define SDL_DEFAULT_REPEAT_DELAY    500
#define SDL_DEFAULT_REPEAT_INTERVAL 30

extern SDL_Window *sdl12on2_window;

static inline SDL_Surface *SDL_SetVideoMode(int w, int h, int bpp, Uint32 flags)
{
    (void)bpp;
    if (sdl12on2_window)
        SDL_DestroyWindow(sdl12on2_window);
    sdl12on2_window = SDL_CreateWindow("Einstein", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, w, h,
            (flags & SDL_FULLSCREEN) ? SDL_WINDOW_FULLSCREEN : 0);
    return sdl12on2_window ? SDL_GetWindowSurface(sdl12on2_window) : NULL;
}

static inline void SDL_UpdateRects(SDL_Surface *s, int n, SDL_Rect *rects)
{
    (void)s;
    if (sdl12on2_window && n > 0)
        SDL_UpdateWindowSurfaceRects(sdl12on2_window, rects, n);
}

static inline SDL_Surface *SDL_DisplayFormat(SDL_Surface *s)
{
    SDL_Surface *screen = sdl12on2_window ? SDL_GetWindowSurface(sdl12on2_window) : NULL;
    return screen ? SDL_ConvertSurface(s, screen->format, 0)
                  : SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGB888, 0);
}

static inline void SDL_WarpMouse(Uint16 x, Uint16 y)
{
    if (sdl12on2_window)
        SDL_WarpMouseInWindow(sdl12on2_window, x, y);
}

static inline void SDL_WM_SetCaption(const char *title, const char *icon)
{
    (void)icon;
    if (sdl12on2_window)
        SDL_SetWindowTitle(sdl12on2_window, title);
}

static inline int SDL_EnableKeyRepeat(int delay, int interval)
{
    (void)delay; (void)interval;   /* SDL 2 always repeats */
    return 0;
}

static inline int SDL_EnableUNICODE(int enable)
{
    (void)enable;                  /* sdl12on2_char() gives the character */
    return 0;
}

/* SDL 1.2's keysym.unicode for the keys Einstein reads as text: the
 * printable ASCII key, upper case with Shift or Caps Lock; 0 otherwise. */
static inline unsigned char sdl12on2_char(const SDL_Keysym *k)
{
    SDL_Keycode s = k->sym;
    if (s < 32 || s > 126)
        return 0;
    if ((k->mod & (KMOD_SHIFT | KMOD_CAPS)) && s >= 'a' && s <= 'z')
        return (unsigned char)(s - 'a' + 'A');
    return (unsigned char)s;
}

/* SDL 1.2's modifier and lock keys (SDLK_NUMLOCK to SDLK_COMPOSE): any key
 * but these ends a "press any key" screen. */
static inline int sdl12on2_is_modifier(SDL_Keycode s)
{
    return s == SDLK_NUMLOCKCLEAR || s == SDLK_CAPSLOCK || s == SDLK_SCROLLLOCK ||
           (s >= SDLK_LCTRL && s <= SDLK_RGUI) || s == SDLK_MODE ||
           s == SDLK_HELP || s == SDLK_SYSREQ || s == SDLK_MENU ||
           s == SDLK_POWER || s == SDLK_UNDO;
}
#endif
