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

#include "Engine/StdH.h"

#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/ErrorTable.h>
#include <Engine/Base/Translation.h>

#include <Engine/Base/FileName.h>
#include <Engine/Base/CTString.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/Console_internal.h>

#include <Engine/Graphics/Adapter.h>

INDEX con_bNoWarnings = 0;

// global handle for application window in full-screen mode
extern HWND _hwndMain;
extern BOOL _bFullScreen;


/*
 * Report error and terminate program.
 */
static BOOL _bInFatalError = FALSE;
void FatalError(const char *strFormat, ...)
{
  // disable recursion
  if (_bInFatalError) return;
  _bInFatalError = TRUE;

  // reset default windows display mode first 
  // (this is a low overhead and shouldn't allocate memory)
  CDS_ResetMode();

#ifdef PLATFORM_WIN32
  // hide fullscreen window if any
  if( _bFullScreen) {
    // must do minimize first - don't know why :(
    ShowWindow( _hwndMain, SW_MINIMIZE);
    ShowWindow( _hwndMain, SW_HIDE);
  }
#endif

  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat);
  CTString strBuffer;
  strBuffer.VPrintF(strFormat, arg);
  va_end(arg);

  if (_pConsole!=NULL) {
    // print the buffer to the console
    CPutString(TRANS("FatalError:\n"));
    CPutString(strBuffer);
    // make sure the console log was written safely
    _pConsole->CloseLog();
  }

#ifdef PLATFORM_WIN32
  // create message box with just OK button
  MessageBoxA(NULL, strBuffer, TRANS("Fatal Error"),
    MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);

  _bInFatalError = FALSE;

  extern void EnableWindowsKeys(void);
  EnableWindowsKeys();
#else
  // !!! FIXME : Use SDL2's SDL_ShowSimpleMessageBox().
  // !!! FIXME : We should really SDL_Quit() here.
  fprintf(stderr, "FATAL ERROR:\n  \"%s\"\n\n", (const char *) strBuffer);
#endif

  _bInFatalError = FALSE;
  // exit program
  exit(EXIT_FAILURE);
}

/*
 * Report warning without terminating program (stops program until user responds).
 */
void WarningMessage(const char *strFormat, ...)
{
  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat);
  CTString strBuffer;
  strBuffer.VPrintF(strFormat, arg);
  va_end(arg);

  // print it to console
  CPrintF("%s\n", (const char *) strBuffer);
  // if warnings are enabled
  if( !con_bNoWarnings) {
    // create message box
    #ifdef PLATFORM_WIN32
    MessageBoxA(NULL, (const char *) strBuffer, TRANS("Warning"), MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);
    #else  // !!! FIXME: SDL_ShowSimpleMessageBox() in SDL2.
    fprintf(stderr, "WARNING: \"%s\"\n", (const char *) strBuffer);
    #endif
  }
}

void InfoMessage(const char *strFormat, ...)
{
  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat);
  CTString strBuffer;
  strBuffer.VPrintF(strFormat, arg);
  va_end(arg);

  // print it to console
  CPrintF("%s\n", (const char *) strBuffer);

  // create message box
  #ifdef PLATFORM_WIN32
  MessageBoxA(NULL, (const char *) strBuffer, TRANS("Information"), MB_OK|MB_ICONINFORMATION|MB_SETFOREGROUND|MB_TASKMODAL);
  #else  // !!! FIXME: SDL_ShowSimpleMessageBox() in SDL2.
  fprintf(stderr, "INFO: \"%s\"\n", (const char *) strBuffer);
  #endif
}

/* Ask user for yes/no answer(stops program until user responds). */
BOOL YesNoMessage(const char *strFormat, ...)
{
  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat);
  CTString strBuffer;
  strBuffer.VPrintF(strFormat, arg);
  va_end(arg);

  // print it to console
  CPrintF("%s\n", (const char *) strBuffer);

  // create message box
  #ifdef PLATFORM_WIN32
  return MessageBoxA(NULL, strBuffer, TRANS("Question"), MB_YESNO|MB_ICONQUESTION|MB_SETFOREGROUND|MB_TASKMODAL)==IDYES;
  #else
  // !!! FIXME: SDL_messagebox
  fprintf(stderr, "QUESTION: \"%s\" [y/n] : ", (const char *) strBuffer);
  while (true)
  {
    int ch = fgetc(stdin);
    if (ch == 'y')
        return 1;
    else if (ch == 'n')
        return 0;
  }
  return 0;
  #endif
}

/*
 * Throw an exception of formatted string.
 */
void ThrowF_t(char *strFormat, ...)  // throws char *
{
  const SLONG slBufferSize = 256;
  //char strBuffer[slBufferSize+1];  // Can't throw from the stack like this!
  static char *strBuffer = NULL;

  // !!! FIXME: This could be dangerous if you call this in a catch handler...
  delete[] strBuffer;
  strBuffer = new char[slBufferSize+1];

  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat); // variable arguments start after this argument
  _vsnprintf(strBuffer, slBufferSize, strFormat, arg);
  va_end(arg);
  throw strBuffer;
}

/*
 * Get the name string for error code.
 */
 const char *ErrorName(const struct ErrorTable *pet, SLONG ulErrCode)
{
  for (INDEX i=0; i<pet->et_Count; i++) {
    if (pet->et_Errors[i].ec_Code == ulErrCode) {
      return pet->et_Errors[i].ec_Name;
    }
  }
  return TRANS("CROTEAM_UNKNOWN");
}

/*
 * Get the description string for error code.
 */
 const char *ErrorDescription(const struct ErrorTable *pet, SLONG ulErrCode)
{
  for (INDEX i=0; i<pet->et_Count; i++) {
    if (pet->et_Errors[i].ec_Code == ulErrCode) {
      return pet->et_Errors[i].ec_Description;
    }
  }
  return TRANS("Unknown error");
}

/*
 * Get the description string for windows error code.
 */
 extern const CTString GetWindowsError(DWORD dwWindowsErrorCode)
{
#ifdef PLATFORM_WIN32
  // buffer to recieve error description
  LPVOID lpMsgBuf;
  // call function that will prepare text abount given windows error code
  FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, dwWindowsErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf, 0, NULL );
  // create result CTString from prepared message
  CTString strResultMessage = (char *)lpMsgBuf;
  // Free the buffer.
  LocalFree( lpMsgBuf );
  return strResultMessage;
#else
  CTString retval = "This isn't Windows, so calling this function is probably a portability bug.";
  return(retval);
#endif
}

// must be in separate function to disable stupid optimizer
extern void Breakpoint(void)
{
#if (defined USE_PORTABLE_C)
  raise(SIGTRAP);  // This may not work everywhere. Good luck.
#elif (defined __MSVC_INLINE__)
  __asm int 0x03;
}
