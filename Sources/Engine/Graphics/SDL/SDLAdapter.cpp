/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include <stdio.h>
#include <assert.h>

#include "SDL.h"
#include <Engine/Base/Types.h>

ULONG DetermineDesktopWidth(void)
{
    SDL_Rect **modes = SDL_ListModes(NULL, SDL_FULLSCREEN);

    assert(modes != NULL);

    if ((modes == NULL) || (modes == (SDL_Rect **) -1))
        return(600);  // (*shrug*)

    return(modes[0]->w);
} // DetermineDesktopWidth

// end of SDLAdapter.cpp ...


