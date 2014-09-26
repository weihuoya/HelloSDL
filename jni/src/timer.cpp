#include "timer.h"


static uint32_t idCounter = 0;
static inline uint32_t uniqueId()
{
    return ++idCounter;
}


Timer::Timer() : timerId_(0)
{
}


Timer::~Timer()
{
    stop();
}


Timer * Timer::instance()
{
    static Timer timer;
    return &timer;
}


uint32_t Timer::add(const Callback& callback, TIMER_TYPE type, int delay)
{
    uint32_t id = uniqueId();
    slots_.emplace_back(type, id, (uint32_t)delay, callback);
    return id;
}


void Timer::remove(uint32_t id)
{
    std::vector<TimerSlot>::iterator iter, last = slots_.end();

    for(iter = slots_.begin(); iter != last; ++iter)
    {
        if(iter->id == id)
        {
            slots_.erase(iter);
            break;
        }
    }
}


void Timer::start()
{
    if(timerId_ == 0)
    {
        timerId_ = SDL_AddTimer(50, Timer::sTimeHandler, this);
    }
}


void Timer::stop()
{
    if(timerId_ > 0)
    {
        SDL_RemoveTimer(timerId_);
        timerId_ = 0;
    }
}


uint32_t SDLCALL Timer::sTimeHandler(uint32_t interval, void *param)
{
    Timer * timer = static_cast<Timer*>(param);
    std::vector<TimerSlot>& slots = timer->slots_;

    if(slots.empty())
    {
        timer->stop();
    }
    else
    {
        bool running = false;
        uint32_t count = 0, timeout = 0;
        std::vector<TimerSlot>::iterator step, iter;
        std::vector<TimerSlot>::iterator last = slots.end();

        for(step = iter = slots.begin(); iter != last; ++iter)
        {
            running = true;
            timeout = iter->tick + interval;

            if(timeout > iter->delay)
            {
                iter->callback(timeout);
                if(iter->type == Timer::TIMER_INTERVAL)
                {
                    iter->tick = 0;
                }
                else if(iter->type == Timer::TIMER_TIMEOUT)
                {
                    running = false;
                }
            }
            else
            {
                iter->tick = timeout;
            }

            if(running)
            {
                *step++ = *iter;
                ++count;
            }
        }

        slots.resize(count);
    }

    return 0;
}
