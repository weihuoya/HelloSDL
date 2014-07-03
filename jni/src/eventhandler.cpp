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


static int SDL_PostEvent(SDL_EventType eventType)
{
    int posted = 0;
    if (SDL_EventState(eventType, SDL_QUERY) == SDL_ENABLE) {
        SDL_Event event;
        event.type = eventType;
        posted = (SDL_PushEvent(&event) > 0);
    }
    return (posted);
}

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
            //event.window
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
            //event.key
            OnKeyEvent(event.key);
            break;
        /*case SDL_MOUSEMOTION:
            OnMouseMotion(event.motion);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            OnMouseButton(event.button);
            break;*/
        case SDL_FINGERDOWN:
        case SDL_FINGERMOTION:
        case SDL_FINGERUP:
            if (SDL_GetNumTouchFingers(event.tfinger.touchId) == 1)
            {
                OnFingerEvent(event.tfinger);
            }
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
        SDL_PostEvent(SDL_QUIT);
    }
    return 0;
}

int EventHandler::OnMouseButton(const SDL_MouseButtonEvent& mouse)
{
    if(mouse.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Log("[mouse] x: %d, y: %d", mouse.x, mouse.y);
    }
    return 0;
}

int EventHandler::OnMouseMotion(const SDL_MouseMotionEvent& mouse)
{
    SDL_Log("[motion] x: %d, y: %d, xrel: %d, yrel: %d", mouse.x, mouse.y, mouse.xrel, mouse.yrel);
    return 0;
}

int EventHandler::OnDollarGesture(const SDL_DollarGestureEvent& gesture)
{
    SDL_Log("[dollar]");
    return 0;
}

int EventHandler::OnFingerEvent(const SDL_TouchFingerEvent& event)
{
    if(event.type == SDL_FINGERDOWN)
    {
    }
    else if(event.type == SDL_FINGERMOTION)
    {
        float dx = event.dx;
        float dy = event.dy;
        GLContext::instance()->incRotate(dx > 0 ? 2.0f : -2.0f, dy > 0 ? 2.0f : -2.0f);
    }
    else if(event.type == SDL_FINGERUP)
    {
    }

    return 0;
}

int EventHandler::OnMultiGesture(const SDL_MultiGestureEvent& gesture)
{
    const SDL_Finger *finger = NULL;
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
            GLContext::instance()->incScale(currDistance > prevDistance_ ? 0.1f : -0.1f);
        }
        else
        {
            prevDistance_ = currDistance;
        }
    }

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












