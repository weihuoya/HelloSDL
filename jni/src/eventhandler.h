//
//  eventhandler.h
//  HelloSDL
//


#ifndef __hellosdl__eventhandler__
#define __hellosdl__eventhandler__

#include <SDL_events.h>
#include "gltool.h"
//#include <sigc++/sigc++.h>


#include <vector>
#include <utility>

typedef void (SDLCALL * TimerCallback) ();
typedef std::vector< std::pair<TimerCallback, Uint32> > TIMERHANDLERS;


class EventHandler
{
public:
    static EventHandler * instance();
    int OnEventReceived(const SDL_Event& event);

    int OnWindowEvent(const SDL_WindowEvent& window);

    int OnKeyEvent(const SDL_KeyboardEvent& key);
    int OnFingerEvent(const SDL_TouchFingerEvent& finger);
    int OnMouseEvent(const SDL_Event& event);

    int OnDollarGesture(const SDL_DollarGestureEvent& gesture);
    int OnMultiGesture(const SDL_MultiGestureEvent& gesture);

    void OnPause();
    void OnResume();
    int OnQuit();

    void setTimeout(TimerCallback callback, Uint32 interval);
    static Uint32 SDLCALL sTimerHandler(Uint32 interval, void *param);

    SDL_TimerID timerId_;
    TIMERHANDLERS timehandlers_;

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(EventHandler);


    SDL_TouchID touchId_;
    float prevDistance_;
};


#endif /* defined(__HelloSDL__eventhandler__) */
