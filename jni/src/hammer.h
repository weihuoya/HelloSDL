#ifndef __hellosdl__hammer__
#define __hellosdl__hammer__

#include <functional>
#include <memory>
#include <vector>
#include <utility>

#include "gltool.h"
#include "input.h"




class Hammer
{
public:
    enum HAMMER_EVENT {
        EVENT_NONE,
        EVENT_PAN,
        EVENT_PINCH,
        EVENT_SWIPE,
        EVENT_TAP,
        EVENT_DOUBLE_TAP,
        EVENT_LONG_PRESS,
    };

    typedef std::function<void ()> TimeCallback;

    static Hammer * instance();

    void OnTouchEvent(const SDL_TouchFingerEvent& event, const SDL_Touch * touch);
    void recognize(const Input * input);

    Uint32 setTimeout(TimerCallback& callback, Uint32 timeout);
    void clearTimeout(Uint32 slotId);
    static Uint32 SDLCALL sTimeHandler(Uint32 interval, void *param);

    void OnPanEvent(const Input * input);
    void OnPinchEvent(const Input * input);
    void OnSwipeEvent(const Input * input);
    void OnTapEvent(const Input * input);
    void OnDoubleTapEvent(const Input * input);
    void OnLongPressEvent(const Input * input);

    void trigger(Uint32 event, Uint32 state, const Input * input);

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(Hammer);

    struct TimeSlot {
        TimeSlot(TimeCallback& callback, Uint32 timeout, Uint32 slotId) : callback(callback), ticktock(0), timeout(timeout), slotId(slotId) {}

        TimeCallback callback;
        Uint32 ticktock;
        Uint32 timeout;
        Uint32 slotId;
    };
    typedef std::vector<TimeSlot> TIMEHANDLERS;

    TIMEHANDLERS timehandlers_;
    SDL_TimerID timerId_;

    std::shared_ptr<Input> firstInput_;
    std::shared_ptr<Input> firstMultiple_;
    std::shared_ptr<Input> lastInterval_;

    size_t prevFingers_;
};


#endif /* defined(__HelloSDL__hammer__) */
