/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "SeriousSam/StdH.h"
#include "MainWindow.h"
#include "resource.h"

// !!! FIXME : Make a clean abstraction, remove these #ifdefs.
#ifdef PLATFORM_UNIX
#include "SDL.h"
#endif

BOOL _bWindowChanging = FALSE;    // ignores window messages while this is set
HWND _hwndMain = NULL;

static char achWindowTitle[256]; // current window title

// for window reposition function
static PIX _pixLastSizeI, _pixLastSizeJ;


#ifdef PLATFORM_WIN32
static HBITMAP _hbmSplash = NULL;
static BITMAP  _bmSplash;

// window procedure active while window changes are occuring
LRESULT WindowProc_WindowChanging( HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{
    switch( message ) {
    case WM_PAINT: {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps); 
      EndPaint(hWnd, &ps); 
 
      return 0;
                   } break;
    case WM_ERASEBKGND: {

      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps); 
      RECT rect;
      GetClientRect(hWnd, &rect); 
      FillRect(ps.hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
      HDC hdcMem = CreateCompatibleDC(ps.hdc); 
      SelectObject(hdcMem, _hbmSplash); 
      BitBlt(ps.hdc, 0, 0, _bmSplash.bmWidth, _bmSplash.bmHeight, hdcMem, 0, 0, SRCCOPY); 
//      StretchBlt(ps.hdc, 0, 0, rect.right, rect.bottom,
//        hdcMem, 0,0, _bmSplash.bmWidth, _bmSplash.bmHeight, SRCCOPY); 

      EndPaint(hWnd, &ps); 

      return 1; 
                   } break;
    case WM_CLOSE:
      return 0;
      break;
    }
    return DefWindowProcA(hWnd, message, wParam, lParam);
}

// window procedure active normally
LRESULT WindowProc_Normal( HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{
  switch( message ) {

  // system commands
  case WM_SYSCOMMAND: {
    switch( wParam & ~0x0F) {
    // window resizing messages
    case SC_MINIMIZE:
    case SC_RESTORE:
    case SC_MAXIMIZE:
      // relay to application
  	  PostMessage(NULL, message, wParam & ~0x0F, lParam);
      // do not allow automatic resizing
      return 0;
      break;
    // prevent screen saver and monitor power down
    case SC_SCREENSAVE:
    case SC_MONITORPOWER:
      return 0;
    }
                      } break;
  // when close box is clicked
  case WM_CLOSE:
    // relay to application
  	PostMessage(NULL, message, wParam, lParam);
    // do not pass to default wndproc
    return 0;

  // some standard focus loose/gain messages
  case WM_LBUTTONUP:
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_CANCELMODE:
  case WM_KILLFOCUS:
  case WM_ACTIVATEAPP:
    // relay to application
  	PostMessage(NULL, message, wParam, lParam);
    // pass to default wndproc
    break;
  }

  // if we get to here, we pass the message to default procedure
  return DefWindowProcA(hWnd, message, wParam, lParam);
}

// main window procedure
LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{
  // dispatch to proper window procedure
  if(_bWindowChanging) {
    return WindowProc_WindowChanging(hWnd, message, wParam, lParam);
  } else {
    return WindowProc_Normal(hWnd, message, wParam, lParam);
  }
}
#endif

// init/end main window management
void MainWindow_Init(void)
{
#ifdef PLATFORM_WIN32
  // register the window class
  WNDCLASSEXA wc;
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = _hInstance;
  wc.hIcon = LoadIconA( _hInstance, (LPCSTR)IDR_MAINFRAME );
  wc.hCursor = NULL;
  wc.hbrBackground = NULL;
  wc.lpszMenuName = APPLICATION_NAME;
  wc.lpszClassName = APPLICATION_NAME;
  wc.hIconSm = NULL;
  if (0 == RegisterClassExA(&wc)) {
    DWORD dwError = GetLastError();
    CTString strErrorMessage(TRANS("Cannot open main window!"));
    CTString strError;
    strError.PrintF("%s Error %d", strErrorMessage, dwError);
    FatalError(strError);
  }

  // load bitmaps
  _hbmSplash = LoadBitmapA(_hInstance, (char*)IDB_SPLASH);
  ASSERT(_hbmSplash!=NULL);
  GetObject(_hbmSplash, sizeof(BITMAP), (LPSTR) &_bmSplash); 
  // here was loading and setting of no-windows-mouse-cursor

#else
  STUBBED("load window icon");
#endif
}


void MainWindow_End(void)
{
#ifdef PLATFORM_WIN32
  DeleteObject(_hbmSplash);
#else
  STUBBED("");
#endif
}


// close the main application window
void CloseMainWindow(void)
{
#ifdef PLATFORM_WIN32
  // if window exists
  if( _hwndMain!=NULL) {
    // destroy it
    DestroyWindow(_hwndMain);
    _hwndMain = NULL;
  }
#else
  _hwndMain = NULL;
#endif
}

#ifdef PLATFORM_UNIX
static void CreateSDLWindow(PIX pixSizeI, PIX pixSizeJ,
                            const char *title, const char *icon,
                            Uint32 flags)
{
  flags |= SDL_OPENGL;

  SDL_WM_SetCaption(title, icon);

  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  // ShowCursor(0) and input grabbing need to be done here to prevent a
  //  Voodoo 3 bug. Was this ever fixed?
  SDL_ShowCursor(0);
  SDL_WM_GrabInput(SDL_GRAB_ON);

  int color_depth[] = {32, 24, 16, 15, -1};
  int i;

  if (_pGfx->gl_iCurrentDepth != 0)
  {
    color_depth[0] = _pGfx->gl_iCurrentDepth;
    color_depth[1] = -1;
  } // if

  for (i = 0; color_depth[i] > 0; i++)
  {
    int bits = (color_depth[i] >= 24) ? 8 : 5;
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, bits);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, bits);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, bits);
    if (SDL_SetVideoMode(pixSizeI, pixSizeJ, color_depth[i], flags) != NULL)
      break;
    CPrintF("SDL_SetVideoMode() failed at %d-bit color! Reason: %s\n", color_depth[i], SDL_GetError());
  } // for

  SDL_WM_GrabInput(SDL_GRAB_OFF);

  if (color_depth[i] > 0)
    _pGfx->gl_iCurrentDepth = color_depth[i];
  else
  {
    SDL_Quit();
    FatalError("Failed to create GL context.\n");
  } // else

  _hwndMain = (void *) 0x0001;
  SE_UpdateWindowHandle( _hwndMain);
} // CreateSDLWindow
#endif


void ResetMainWindowNormal(void)
{
#ifdef PLATFORM_WIN32
  ShowWindow( _hwndMain, SW_HIDE);
  // add edges and title bar to window size so client area would have size that we requested
  RECT rWindow, rClient;
  GetClientRect( _hwndMain, &rClient);
  GetWindowRect( _hwndMain, &rWindow);
  const PIX pixWidth  = _pixLastSizeI + (rWindow.right-rWindow.left) - (rClient.right-rClient.left);
  const PIX pixHeight = _pixLastSizeJ + (rWindow.bottom-rWindow.top) - (rClient.bottom-rClient.top);
  const PIX pixPosX   = (::GetSystemMetrics(SM_CXSCREEN) - pixWidth ) /2;
  const PIX pixPosY   = (::GetSystemMetrics(SM_CYSCREEN) - pixHeight) /2;
  // set new window size and show it
  SetWindowPos( _hwndMain, NULL, pixPosX,pixPosY, pixWidth,pixHeight, SWP_NOZORDER);
  ShowWindow(   _hwndMain, SW_SHOW);
#endif
}


// open the main application window for windowed mode
void OpenMainWindowNormal( PIX pixSizeI, PIX pixSizeJ)
{
#ifdef PLATFORM_WIN32
  ASSERT(_hwndMain==NULL);

  // create a window, invisible initially
  _hwndMain = CreateWindowExA(
	  WS_EX_APPWINDOW,
	  APPLICATION_NAME,
	  "",   // title
    WS_OVERLAPPED|WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU,
	  10,10,
	  100,100,  // window size
	  NULL,
	  NULL,
	  _hInstance,
	  NULL);
  // didn't make it?
  if( _hwndMain==NULL) FatalError(TRANS("Cannot open main window!"));
  SE_UpdateWindowHandle( _hwndMain);

  // set window title
  sprintf( achWindowTitle, TRANS("Serious Sam (Window %dx%d)"), pixSizeI, pixSizeJ);
  SetWindowTextA( _hwndMain, achWindowTitle);
  _pixLastSizeI = pixSizeI;
  _pixLastSizeJ = pixSizeJ;
  ResetMainWindowNormal();

#else
  _pixLastSizeI = pixSizeI;
  _pixLastSizeJ = pixSizeJ;
  // set window title
  sprintf( achWindowTitle, TRANS("Serious Sam (Window %dx%d)"), pixSizeI, pixSizeJ);
  CreateSDLWindow(pixSizeI, pixSizeJ, achWindowTitle, "ssam", 0);
#endif
}


// open the main application window for fullscreen mode
void OpenMainWindowFullScreen( PIX pixSizeI, PIX pixSizeJ)
{
#ifdef PLATFORM_WIN32
  ASSERT( _hwndMain==NULL);
  // create a window, invisible initially
  _hwndMain = CreateWindowExA(
    WS_EX_TOPMOST | WS_EX_APPWINDOW,
    APPLICATION_NAME,
    "",   // title
    WS_POPUP,
    0,0,
    pixSizeI, pixSizeJ,  // window size
    NULL,
    NULL,
    _hInstance,
    NULL);
  // didn't make it?
  if( _hwndMain==NULL) FatalError(TRANS("Cannot open main window!"));
  SE_UpdateWindowHandle( _hwndMain);

  // set window title and show it
  sprintf( achWindowTitle, TRANS("Serious Sam (FullScreen %dx%d)"), pixSizeI, pixSizeJ);
  SetWindowTextA( _hwndMain, achWindowTitle);
  ShowWindow(    _hwndMain, SW_SHOWNORMAL);

#else
  // set window title
  sprintf( achWindowTitle, TRANS("Serious Sam (FullScreen %dx%d)"), pixSizeI, pixSizeJ);
  CreateSDLWindow(pixSizeI, pixSizeJ, achWindowTitle, "ssam", SDL_FULLSCREEN);
#endif
}


// open the main application window invisible
void OpenMainWindowInvisible(void)
{
#ifdef PLATFORM_WIN32
  ASSERT(_hwndMain==NULL);
  // create a window, invisible initially
  _hwndMain = CreateWindowExA(
	  WS_EX_APPWINDOW,
	  APPLICATION_NAME,
	  "",   // title
    WS_POPUP,
	  0,0,
	  10, 10,  // window size
	  NULL,
	  NULL,
	  _hInstance,
	  NULL);
  // didn't make it?
  if( _hwndMain==NULL) {
    DWORD dwError = GetLastError();
    CTString strErrorMessage(TRANS("Cannot open main window!"));
    CTString strError;
    strError.PrintF("%s Error %d", strErrorMessage, dwError);
    FatalError(strError);
  }
  SE_UpdateWindowHandle( _hwndMain);

  // set window title
  sprintf( achWindowTitle, "Serious Sam");
  SetWindowTextA( _hwndMain, achWindowTitle);

#else

  STUBBED("Need SDL invisible window or something");

#endif
}

