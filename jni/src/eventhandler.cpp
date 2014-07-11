//
//  eventhandler.cpp
//  HelloSDL
//
//  Created by Moqikaka on 14-5-5.
//  Copyright (c) 2014年 Moqikaka. All rights reserved.
//

#include <SDL_touch.h>
#include <SDL_log.h>

#include "eventhandler.h"
#include "glcontext.h"

#include "SDL_events_c.h"
#include "SDL_touch_c.h"


EventHandler::EventHandler() : touchId_(0), prevDistance_(0.0f)
{
}


EventHandler * EventHandler::instance()
{
    static EventHandler handler;
    return &handler;
}


int EventHandler::OnEventReceived(const SDL_Event& event)
{
    int done = 0;

    switch(event.type) {
        case SDL_WINDOWEVENT:
            OnWindowEvent(event.window);
            break;
        case SDL_APP_WILLENTERBACKGROUND:
            OnPause();
            break;
        case SDL_APP_WILLENTERFOREGROUND:
            OnResume();
            break;
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            OnKeyEvent(event.key);
            break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            OnMouseEvent(event);
            break;
        case SDL_FINGERDOWN:
        case SDL_FINGERMOTION:
        case SDL_FINGERUP:
            OnFingerEvent(event.tfinger);
            break;
        case SDL_MULTIGESTURE:
            OnMultiGesture(event.mgesture);
            break;
        case SDL_DOLLARGESTURE:
            OnDollarGesture(event.dgesture);
            break;
        case SDL_QUIT:
            done = OnQuit();
            break;
        default:
            break;
    }

    return done;
}


int EventHandler::OnWindowEvent(const SDL_WindowEvent& window)
{
    switch (window.event) {
        case SDL_WINDOWEVENT_ENTER:
            SDL_Log("mouse enter");
            break;
        case SDL_WINDOWEVENT_LEAVE:
            SDL_Log("mouse leave");
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            SDL_Log("window gain focus");
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            SDL_Log("window lost focus");
            break;
        default:
            break;
    }
    return 0;
}


int EventHandler::OnKeyEvent(const SDL_KeyboardEvent& key)
{
    if(key.type == SDL_KEYDOWN)
    {
        SDL_Log("[key] sym: %d", key.keysym.sym);
    }

    if (key.keysym.sym == 1073742094)
    {
        SDL_SendQuit();
    }
    return 0;
}


int EventHandler::OnDollarGesture(const SDL_DollarGestureEvent& gesture)
{
    SDL_Log("[dollar]");
    return 0;
}


int EventHandler::OnMouseEvent(const SDL_Event& event)
{
    /*const SDL_MouseButtonEvent& button = event.button;
    const SDL_MouseMotionEvent& motion = event.motion;

    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Log("[mouse] down");
    }
    else if(event.type == SDL_MOUSEMOTION)
    {
        SDL_Log("[mouse] motion");
    }
    else if(event.type == SDL_MOUSEBUTTONUP)
    {
        SDL_Log("[mouse] up");
    }*/

    return 0;
}


int EventHandler::OnFingerEvent(const SDL_TouchFingerEvent& event)
{
    Uint32 timestamp = event.timestamp;
    SDL_TouchID touchId = event.touchId;
    SDL_FingerID fingerId = event.fingerId;

    int i = 0;
    const SDL_Finger *finger = NULL;
    const SDL_Touch * touch = SDL_GetTouch(touchId);
    if(touch)
    {
        for(i = 0; i < touch->num_fingers; ++i)
        {
            finger = touch->fingers[i];
        }
        SDL_Log("[finger] (%lld, %d)", touch->id, touch->num_fingers, touch->fingers);
    }

    /*if(event.type == SDL_FINGERDOWN)
    {
        SDL_Log("[finger] (%lld, %lld, %u) down", touchId, fingerId, timestamp);
    }
    else if(event.type == SDL_FINGERMOTION)
    {
        SDL_Log("[finger] (%lld, %lld, %u) motion", touchId, fingerId, timestamp);
    }
    else if(event.type == SDL_FINGERUP)
    {
        SDL_Log("[finger] (%lld, %lld, %u) up", touchId, fingerId, timestamp);
    }*/

    return 0;
}


int EventHandler::OnMultiGesture(const SDL_MultiGestureEvent& gesture)
{
    /*const SDL_Finger *finger = NULL;
    SDL_TouchID touchId = gesture.touchId;
    int i = 0, numFingers = SDL_GetNumTouchFingers(touchId);

    if (touchId_ != touchId)
    {
        touchId_ = touchId;
        prevDistance_ = 0.0f;
    }

    if (numFingers == 2)
    {
        const SDL_Finger * finger0 = SDL_GetTouchFinger(gesture.touchId, 0);
        const SDL_Finger * finger1 = SDL_GetTouchFinger(gesture.touchId, 1);

        float dx = (finger0->x - finger1->x) * 16;
        float dy = (finger0->y - finger1->y) * 16;
        float currDistance = sqrt(dx*dx+dy*dy);

        if (prevDistance_ > 0)
        {
            float delta = currDistance - prevDistance_;
            if(delta < 0.1)
            {
                GLContext::instance()->incTranslate(0, 0);
            }
            else
            {
                GLContext::instance()->incScale(1.0);
            }
            prevDistance_ = currDistance;
        }
        else
        {
            prevDistance_ = currDistance;
        }
    }*/

    return 0;
}


void EventHandler::OnPause()
{
    SDL_Log("[pause]");
}


void EventHandler::OnResume()
{
    SDL_Log("[resume]");
}


int EventHandler::OnQuit()
{
    SDL_Log("[quit]");
    return 1;
}






