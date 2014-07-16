#ifndef __hellosdl__hammer__
#define __hellosdl__hammer__


#include <memory>
#include <vector>
#include <utility>


typedef void (SDLCALL * TimeCallback) ();


class Hammer
{
public:
    static Hammer * instance();

    void OnTouchEvent(const SDL_TouchFingerEvent& event, const SDL_Touch * touch);
    void recognize(const Input * input);

    void setTimeout(TimerCallback callback, Uint32 interval);
    static Uint32 SDLCALL sTimeHandler(Uint32 interval, void *param);

    void OnPanEvent(const Input * input);
    void OnPinchEvent(const Input * input);
    void OnSwipeEvent(const Input * input);
    void OnTapEvent(const Input * input);
    void OnDoubleTapEvent(const Input * input);
    void OnLongPressEvent(const Input * input);

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(Hammer);

    typedef std::vector< std::pair<TimeCallback, Uint32> > TIMEHANDLERS;
    TIMEHANDLERS timehandlers_;
    SDL_TimerID timerId_;

    std::shared_ptr<Input> firstInput_;
    std::shared_ptr<Input> firstMultiple_;
    std::shared_ptr<Input> lastInterval_;

    size_t prevFingers_;
};


#endif /* defined(__HelloSDL__hammer__) */
