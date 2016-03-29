/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "SDL.h"
#include "SDL_thread.h"

#include <Engine/Engine.h>

ULONG ThreadLocalGetCurrentTID(void)
{
    return(SDL_ThreadID());
} // ThreadLocalGetCurrentTID

// end of SDLThreadLocalStorage.cpp ...


