#include <SDL_timer.h>
#include <SDL_events.h>

#include "hammer.h"


#define COMPUTE_INTERVAL 25

static inline float getDistance(float x, float y)
{
    return sqrt(x * x + y * y);
}

static inline float getRotate(float x, float y)
{
    return atan2(y, x) * 180 / M_PI;
}

static inline uint32_t getDirection(float x, float y)
{
    uint32_t direction = Input::DIRECTION_NONE;

    if(x == y)
    {
        direction = Input::DIRECTION_NONE;
    }
    else if(abs(x) >= abs(y))
    {
        direction = x > 0 ? Input::DIRECTION_LEFT : Input::DIRECTION_RIGHT;
    }
    else
    {
        direction = y > 0 ? Input::DIRECTION_UP : Input::DIRECTION_DOWN;
    }

    return direction;
}




Hammer * Hammer::instance()
{
    static Hammer handler;
    return &handler;
}

Hammer::Hammer() : prevFingers_(0)
{
}


void Hammer::OnTouchEvent(const SDL_TouchFingerEvent& event, const SDL_Touch * touch)
{
    const SDL_Finger *finger = NULL;
    float deltaX, deltaY;
    int i = 0, numFingers = touch->num_fingers;
    bool isFirst = false;
    bool isFinal = false;
    uint32_t eventType;

    std::shared_ptr<Input> input(new Input);

    if(event.type == SDL_FINGERDOWN)
    {
        isFirst = (prevFingers_ == 0);
        eventType = Input::INPUT_START;
    }
    else if(event.type == SDL_FINGERMOTION)
    {
        eventType = Input::INPUT_MOVE;
    }
    else //if(event.type == SDL_FINGERUP)
    {
        isFinal = (numFingers == 0);
        eventType = Input::INPUT_END;
    }
    //else
    //{
    //    isFinal = (numFingers == 0);
    //    eventType = INPUT_CANCEL;
    //}

    // compute input
    input->isFirst = isFirst;
    input->isFinal = isFinal;
    input->type = eventType;

    input->setFingers(touch);

    if(!firstInput_)
    {
        firstInput_ = input;
        //needRelease = false;
    }

    if(numFingers == 1)
    {
        finger = input->fingers[0].get();
        input->centerX = finger->x;
        input->centerY = finger->y;

        firstMultiple_.reset();
        /*if(firstMultiple_)
        {
            delete firstMultiple_;
            firstMultiple_ = NULL;
        }*/
    }
    else if(numFingers > 1)
    {
        float x = 0, y = 0;
        for (i = 0; i < numFingers; ++i)
        {
            finger = input->fingers[i].get();
            x += finger->x;
            y += finger->y;
        }
        input->centerX = x / numFingers;
        input->centerY = y / numFingers;

        if(!firstMultiple_)
        {
            //needRelease = false;
            firstMultiple_ = input;
        }
    }

    input->timeStamp = event.timestamp;
    input->deltaTime = input->timeStamp - firstInput_->timeStamp;

    if(firstMultiple_)
    {
        //scale
        float firstDistance;
        float currDistance;

        deltaX = firstMultiple_->fingers[0]->x - firstMultiple_->fingers[1]->x;
        deltaY = firstMultiple_->fingers[0]->y - firstMultiple_->fingers[1]->y;
        //firstDistance = sqrt(deltaX * deltaX + deltaY * deltaY);
        firstDistance = getDistance(deltaX, deltaY);

        deltaX = input->fingers[0]->x - input->fingers[1]->x;
        deltaY = input->fingers[0]->y - input->fingers[1]->y;
        //currDistance = sqrt(deltaX * deltaX + deltaY * deltaY);
        currDistance = getDistance(deltaX, deltaY);

        input->scale = currDistance / firstDistance;

        //rotate
        float firstRotate;
        float currRotate;

        deltaX = firstMultiple_->fingers[0]->x - firstMultiple_->fingers[1]->x;
        deltaY = firstMultiple_->fingers[0]->y - firstMultiple_->fingers[1]->y;
        //firstRotate = atan2(deltaY, deltaX) * 180 / M_PI;
        firstRotate = getRotate(deltaX, deltaY);

        deltaX = input->fingers[0]->x - input->fingers[1]->x;
        deltaY = input->fingers[0]->y - input->fingers[1]->y;
        //currRotate = atan2(deltaY, deltaX) * 180 / M_PI;
        currRotate = getRotate(deltaX, deltaY);

        input->rotation = currRotate - firstRotate;


        input->deltaX = deltaX = input->centerX - firstMultiple_->centerX;
        input->deltaY = deltaY = input->centerY - firstMultiple_->centerY;
    }
    else
    {
        input->deltaX = deltaX = input->centerX - firstInput_->centerX;
        input->deltaY = deltaY = input->centerY - firstInput_->centerY;
    }

    input->angle = getRotate(deltaX, deltaY);
    input->distance = getDistance(deltaX, deltaY);
    input->offsetDirection = getDirection(deltaX, deltaY);


    // compute interval
    if(!lastInterval_)
    {
        lastInterval_ = input;
        //needRelease = false;
    }

    float deltaTime = input->timeStamp - lastInterval_->timeStamp;
    float velocity, velocityX, velocityY;
    uint32_t direction;

    if (deltaTime > COMPUTE_INTERVAL || lastInterval_->velocity < 0)
    {
        deltaX = lastInterval_->deltaX - input->deltaX;
        deltaY = lastInterval_->deltaY - input->deltaY;

        velocityX = abs(deltaX / deltaTime);
        velocityY = abs(deltaY / deltaTime);
        velocity = std::max(velocityX, velocityY);

        direction = getDirection(deltaX, deltaY);
    }
    else
    {
        // use latest velocity info if it doesn't overtake a minimum period
        velocity = lastInterval_->velocity;
        velocityX = lastInterval_->velocityX;
        velocityY = lastInterval_->velocityY;
        direction = lastInterval_->direction;
    }

    input->velocity = velocity;
    input->velocityX = velocityX;
    input->velocityY = velocityY;
    input->direction = direction;


    this->recognize(input.get());

    prevFingers_ = numFingers;

    if(isFinal)
    {
        firstInput_.reset();
        firstMultiple_.reset();
        lastInterval_.reset();
    }
}


void Hammer::recognize(const Input * input)
{
    input->dump();
}


Uint32 SDLCALL Hammer::sTimeHandler(Uint32 delta, void *param)
{
    Hammer * hammer = static_cast<Hammer*>(param);
    TIMEHANDLERS& timehandlers = hammer->timehandlers_;

    if(timehandlers.empty())
    {
        SDL_RemoveTimer(hammer->timerId_);
        hammer->timerId_ = 0;
    }
    else
    {
        size_t count = 0;
        TIMEHANDLERS::iterator step, iter;
        TIMEHANDLERS::iterator last = timehandlers.end();

        for(step = iter = timehandlers.begin(); iter != last; ++iter)
        {
            iter->ticktock = iter->ticktock + delta
            if(iter->ticktock < iter->timeout)
            {
                *step++ = *iter;
                ++count;
            }
            else
            {
                iter->callback();
            }
        }
        timehandlers.resize(count);
    }
}


Uint32 Hammer::setTimeout(TimerCallback callback, Uint32 timeout)
{
    static Uint32 sSlotId = 0;

    timehandlers_.emplace_back(callback, timeout, ++sSlotId);

    if(timerId_ == 0)
    {
        timerId_ = SDL_AddTimer(50, Hammer::sTimeHandler, this);
    }

    return sSlotId;
}


void Hammer::clearTimeout(Uint32 slotId)
{
    std::remove_if(timehandlers_.begin(), timehandlers_.end(), [slotId](const TimeSlot& slot) { return slot.slotId == slotId; });
}


void Hammer::OnPanEvent(const Input * input)
{
}

void Hammer::OnPinchEvent(const Input * input)
{
}

void Hammer::OnSwipeEvent(const Input * input)
{
}

void Hammer::OnTapEvent(const Input * input)
{
}

void Hammer::OnDoubleTapEvent(const Input * input)
{
}

void Hammer::OnLongPressEvent(const Input * input)
{
}


