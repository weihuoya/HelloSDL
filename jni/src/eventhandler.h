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
#include "gltool.h"
//#include <sigc++/sigc++.h>

class EventHandler
{
public:
    static EventHandler * instance();
    int OnEventReceived(const SDL_Event& event);

    int OnWindowEvent(const SDL_WindowEvent& window);
    int OnKeyEvent(const SDL_KeyboardEvent& key);
    int OnFingerEvent(const SDL_TouchFingerEvent& finger);
    int OnMouseButton(const SDL_MouseButtonEvent& mouse);
    int OnMouseMotion(const SDL_MouseMotionEvent& mouse);
    int OnDollarGesture(const SDL_DollarGestureEvent& gesture);
    int OnMultiGesture(const SDL_MultiGestureEvent& gesture);

    void OnPause();
    void OnResume();
    int OnQuit();

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(EventHandler);


    SDL_TouchID touchId_;
    float prevDistance_;
};


#endif /* defined(__HelloSDL__eventhandler__) */
