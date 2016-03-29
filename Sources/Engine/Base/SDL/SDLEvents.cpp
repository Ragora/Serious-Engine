/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include <stdlib.h>
#include <assert.h>

#include <Engine/Base/Types.h>
#include <Engine/Base/SDL/SDLEvents.h>
#include "SDL.h"

static Uint16 next_message_char = 0;


// This keeps the input subsystem in sync with everything else, by
//  making sure all SDL events tunnel through one function.
extern int SE_SDL_InputEventPoll(SDL_Event *event);


BOOL PeekMessage(MSG *msg, void *hwnd, UINT wMsgFilterMin,
                UINT wMsgFilterMax, UINT wRemoveMsg)
{
    assert(msg != NULL);
    assert(wRemoveMsg == PM_REMOVE);
    assert(wMsgFilterMin == 0);
    assert(wMsgFilterMax == 0);

    if (next_message_char != 0)
    {
        msg->message = WM_CHAR;
        msg->wParam = next_message_char;
        msg->unicode = next_message_char;
        next_message_char = 0;
        return(TRUE);
    } // if

    SDL_Event sdlevent;
    if (!SE_SDL_InputEventPoll(&sdlevent))
        return(FALSE);

    assert(sdlevent.type != SDL_NOEVENT);

    msg->message = sdlevent.type;

    switch (sdlevent.type)
    {
        case SDL_MOUSEMOTION:
            msg->lParam = (
                            ((sdlevent.motion.y << 16) & 0xFFFF0000) |
                            ((sdlevent.motion.x      ) & 0x0000FFFF)
                          );
            break;

        case SDL_KEYDOWN:
            if (sdlevent.key.keysym.mod & KMOD_ALT)
                msg->message = WM_SYSKEYDOWN;
            // deliberate fall through...
        case SDL_KEYUP:
            msg->unicode = sdlevent.key.keysym.unicode;
            msg->wParam = sdlevent.key.keysym.sym;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (sdlevent.button.button == 2)  // right button...
            {
                msg->message = WM_RBUTTONDOWN;
            } // if

            else if (sdlevent.button.button == 4)  // wheel up
            {
                msg->message = WM_MOUSEWHEEL;
                msg->wParam = 120 << 16;
            } // else if

            else if (sdlevent.button.button == 5)  // wheel down
            {
                msg->message = WM_MOUSEWHEEL;
                msg->wParam = -120 << 16;
            } // else if
            break;

        case SDL_MOUSEBUTTONUP:
            if (sdlevent.button.button == 2)  // right button...
            {
                msg->message = WM_RBUTTONUP;
            } // if
            break;
    } // switch

    return(TRUE);
} // PeekMessage


void TranslateMessage(MSG *msg)
{
    if (msg->message == WM_KEYDOWN)
    {
        if (msg->unicode != 0)
            next_message_char = msg->unicode;
    } // if
} // TranslateMessage


void DispatchMessage(MSG *msg)
{
    // do nothing.
} // DispatchMessage


SHORT GetKeyState(int vk)
{
    SHORT retval = 0;

    switch (vk)
    {
        case VK_LBUTTON:
            if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK)
                retval = 0x8000;
            break;

        case VK_RBUTTON:
            if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK)
                retval = 0x8000;
            break;

        case VK_MBUTTON:
            if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MMASK)
                retval = 0x8000;
            break;

        default:
            if (SDL_GetKeyState(NULL)[vk])
                retval = 0x8000;
            break;
    } // switch

    return(retval);
} // GetKeyState


SHORT GetAsyncKeyState(int vk)
{
    return(GetKeyState(vk));
} // GetAsyncKeyState


BOOL GetCursorPos(LPPOINT lpPoint)
{
    assert(lpPoint != NULL);

    int x, y;
    SDL_GetMouseState(&x, &y);
    lpPoint->x = x;
    lpPoint->y = y;
    return(TRUE);
} // GetCursorPos


BOOL ScreenToClient(void *hWnd, LPPOINT lpPoint)
{
    // do nothing. SDL returns points in client coordinates already.
    return(1);  // success.  :)
} // ScreenToClient


int ShowCursor(BOOL yes)
{
    static int count = 0;
    count += (yes) ? 1 : -1;
    SDL_ShowCursor((count >= 0) ? SDL_ENABLE : SDL_DISABLE);
    return(count);
} // ShowCursor

// end of SDLEvents.cpp ...


