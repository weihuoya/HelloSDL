#ifndef __hellosdl__timer__
#define __hellosdl__timer__

#include <SDL.h>
#include <functional>
#include <vector>
#include <cstdint>


class Timer
{
public:
    typedef std::function<void(uint32_t)> Callback;

    enum TIMER_TYPE {
        TIMER_INTERVAL,
        TIMER_TIMEOUT,
    };

    struct TimerSlot
    {
        TimerSlot() {}

        TimerSlot(TIMER_TYPE type, uint32_t id, uint32_t delay, const Callback& callback)
            : type(type), id(id), tick(0), delay(delay), callback(callback)
        {
        }

        TimerSlot(const TimerSlot& rhs)
            : type(rhs.type), id(rhs.id), tick(rhs.tick), delay(rhs.delay), callback(rhs.callback)
        {
        }

        TIMER_TYPE type;
        uint32_t id;
        uint32_t tick;
        uint32_t delay;
        Callback callback;
    };

    Timer();
    ~Timer();

    uint32_t add(const Callback& callback, TIMER_TYPE type, int delay);
    void remove(uint32_t id);

    void start();
    void stop();

    static Timer * instance();
    static uint32_t SDLCALL sTimeHandler(uint32_t interval, void *param);

private:
    int timerId_;
    std::vector<TimerSlot> slots_;
};


#endif /* defined(__HelloSDL__timer__) */
