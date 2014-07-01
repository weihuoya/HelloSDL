//
//  eventhandler.h
//  HelloSDL
//
//  Created by Moqikaka on 14-5-5.
//  Copyright (c) 2014年 Moqikaka. All rights reserved.
//

#ifndef __hellosdl__eventhandler__
#define __hellosdl__eventhandler__

#include <SDL_events.h>

class EventHandler
{
public:
    EventHandler();

    int OnEventReceived(const SDL_Event& event);


    virtual int OnWindowEvent(const SDL_WindowEvent& window);
    virtual int OnKeyEvent(const SDL_KeyboardEvent& key);
    virtual int OnFingerEvent(const SDL_TouchFingerEvent& finger);
    virtual int OnMouseButton(const SDL_MouseButtonEvent& mouse);
    virtual int OnMouseMotion(const SDL_MouseMotionEvent& mouse);
    virtual int OnDollarGesture(const SDL_DollarGestureEvent& gesture);
    virtual int OnMultiGesture(const SDL_MultiGestureEvent& gesture);

    virtual void onPause();
    virtual void onResume();
    virtual int OnQuit();

protected:
    SDL_TouchID touchId_;
    GLContext * context_;
};


#endif /* defined(__HelloSDL__eventhandler__) */
