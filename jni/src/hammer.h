#ifndef __hellosdl__hammer__
#define __hellosdl__hammer__

#include <functional>
#include <memory>
#include <vector>
#include <utility>

#include "gltool.h"
#include "input.h"

#include "timer.h"


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

    static Hammer * instance();

    void OnTouchEvent(const SDL_TouchFingerEvent& event, const SDL_Touch * touch);
    void recognize(const Input * input);

    Uint32 setTimeout(Timer::Callback& callback, Uint32 timeout);
    void clearTimeout(Uint32 slotId);

    void trigger(uint32_t type, uint32_t state, const Input * input);

    void OnPanEvent(const Input * input);
    void OnPinchEvent(const Input * input);
    void OnSwipeEvent(const Input * input);
    void OnTapEvent(const Input * input);
    void OnDoubleTapEvent(const Input * input);
    void OnLongPressEvent(const Input * input);

    void trigger(Uint32 event, Uint32 state, const Input * input);

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(Hammer);

    Timer timer_;

    std::shared_ptr<Input> firstInput_;
    std::shared_ptr<Input> firstMultiple_;
    std::shared_ptr<Input> lastInterval_;

    size_t prevFingers_;
};


#endif /* defined(__HelloSDL__hammer__) */
