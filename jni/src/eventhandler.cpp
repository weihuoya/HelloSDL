//
//  eventhandler.cpp
//  HelloSDL
//
//  Created by Moqikaka on 14-5-5.
//  Copyright (c) 2014年 Moqikaka. All rights reserved.
//

#include <SDL2/SDL_log.h>
#include "eventhandler.h"


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


EventHandler::EventHandler() : touchId_(0)
{
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
            onPause();
            break;
        case SDL_APP_WILLENTERFOREGROUND:
            onResume();
            break;
        case SDL_KEYUP:
        case SDL_KEYDOWN:
            //event.key
            OnKeyEvent(event.key);
            break;
        case SDL_MOUSEMOTION:
            OnMouseMotion(event.motion);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            OnMouseButton(event.button);
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
            OnQuit();
            done = 1;
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

int EventHandler::OnFingerEvent(const SDL_TouchFingerEvent& event)
{
    const SDL_Finger *finger = NULL;
    int i = 0, num = SDL_GetNumTouchFingers(event.touchId);

    for(i=0; i < num; ++i)
    {
        finger = SDL_GetTouchFinger(event.touchId, i);
    }

    if(event.type == SDL_FINGERDOWN)
    {
        SDL_Log("[finger] x: %f, y: %f, dx: %f, dy: %f", event.x, event.y, event.dx, event.dy);
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

int EventHandler::OnMultiGesture(const SDL_MultiGestureEvent& gesture)
{
    SDL_Log("[gesture] x: %f, y: %f, num: %hu", gesture.x, gesture.y, gesture.numFingers);
    return 0;
}

void EventHandler::onPause()
{
    SDL_Log("[pause]");
}

void EventHandler::onResume()
{
    SDL_Log("[resume]");
}

int EventHandler::OnQuit()
{
    SDL_Log("[quit]");
    return 0;
}












