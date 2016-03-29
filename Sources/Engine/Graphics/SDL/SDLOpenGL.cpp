/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include <Engine/Engine.h>
#include "SDL.h"

static void FailFunction_t(const char *strName) {
  ThrowF_t(TRANS("Required function %s not found."), strName);
}

static void SetFunctionPointers_t(HINSTANCE hiOGL)
{
  const char *strName;
  // get gl function pointers

  #define DLLFUNCTION(dll, output, name, inputs, params, required) \
    strName = #name;  \
    p##name = (output (__stdcall*) inputs) SDL_GL_GetProcAddress(strName); \
    if( required && p##name == NULL) FailFunction_t(strName);
  #include "Engine/Graphics/gl_functions.h"
  #undef DLLFUNCTION
}

BOOL CGfxLibrary::InitDriver_OGL(BOOL init3dfx)
{
  ASSERT( gl_hiDriver==NONE);

  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    CPrintF( TRANS("Error starting OpenGL: %s\n"), SDL_GetError());
    return(FALSE);
  }

  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  // !!! FIXME: Is it safe to add new cvars for a specific platform?
  const char *envlib = getenv("SERIOUS_GLLIBRARY");
  CTString strDriverFileName = ((envlib) ? envlib : "libGL.so.1");

  if (SDL_GL_LoadLibrary(strDriverFileName) == -1) {
    CPrintF(TRANS("Cannot load OpenGL driver '%s'"), (const char *) strDriverFileName);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return(FALSE);
  }

  // prepare functions
  SetFunctionPointers_t(gl_hiDriver);

  // done
  return TRUE;
}


void CGfxLibrary::PlatformEndDriver_OGL(void)
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


void *CGfxLibrary::OGL_GetProcAddress(const char *procname)
{
    return(SDL_GL_GetProcAddress(procname));
}


// prepare current viewport for rendering thru OpenGL
BOOL CGfxLibrary::SetCurrentViewport_OGL(CViewPort *pvp)
{
  // if must init entire opengl
  if( gl_ulFlags & GLF_INITONNEXTWINDOW)
  {
    gl_ulFlags &= ~GLF_INITONNEXTWINDOW;
    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();
    // init now
    gl_pvpActive = pvp; // remember as current viewport (must do that BEFORE InitContext)
    go_hglRC = (void *) 0x0001;  // !!! FIXME : This needs to be not null, but this is a hack.

    // init now
    InitContext_OGL();
    gl_ulFlags |=  GLF_HASACCELERATION;  // might be a lie, though...
    pvp->vp_ctDisplayChanges = gl_ctDriverChanges;
    return TRUE;
  }

  // if window was not set for this driver
  if( pvp->vp_ctDisplayChanges<gl_ctDriverChanges)
  {
    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();

    // set it
    pvp->vp_ctDisplayChanges = gl_ctDriverChanges;
  }

  // remember as current window
  gl_pvpActive = pvp;
  return TRUE;
}

// end of SDLOpenGL.cpp ...


