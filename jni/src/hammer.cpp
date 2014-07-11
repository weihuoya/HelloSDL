#include "hammer.h"
#include <SDL_timer.h>


#define COMPUTE_INTERVAL 25


Hammer * Hammer::instance()
{
    static Hammer handler;
    return &handler;
}


Hammer::Hammer() : prevFingers_(0), firstInput_(0), firstMultiple_(0), lastInterval_(0)
{
}


Uint32 SDLCALL Hammer::sTimeHandler(Uint32 interval, void *param)
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
        size_t count = 0, timeout = 0;
        TIMEHANDLERS::iterator step, iter;
        TIMEHANDLERS::iterator last = timehandlers.end();

        for(step = iter = timehandlers.begin(); iter != last; ++iter)
        {
            timeout = (*iter).second - interval;
            if(timeout > 0)
            {
                (*iter).second = timeout;
                *step++ = *iter;
                ++count;
            }
            else
            {
                (*iter).first();
            }
        }
        timehandlers.resize(count);
    }
}


void Hammer::setTimeout(TimerCallback callback, Uint32 interval)
{
    //timehandlers_.push_back(std::make_pair(callback, interval));
    timehandlers_.emplace_back(callback, interval);

    if(timerId_ == 0)
    {
        timerId_ = SDL_AddTimer(50, Hammer::sTimeHandler, this);
    }
}


void Hammer::OnTouchEvent(const SDL_TouchFingerEvent& event, const SDL_Touch * touch)
{
    const SDL_Finger *finger = NULL;
    float deltaX, deltaY;
    int i = 0, numFingers = touch->num_fingers;
    bool isFirst = prevFingers_ == 0 && event.type == SDL_FINGERDOWN;
    bool isFinal = numFingers == 0 && event.type == SDL_FINGERUP;

    // compute input
    input->isFirst = isFirst;
    input->isFinal = isFinal;
    input->eventType = event.type;

    if(!firstInput_) firstInput_ = input;

    if(numFingers == 1)
    {
        finger = touch->fingers[0];
        input->centerX = finger->x;
        input->centerY = finger->y;

        firstMultiple_ = NULL;
    }
    else if(numFingers > 1)
    {
        float x = 0, y = 0;
        for (i = 0; i < numFingers; ++i)
        {
            finger = touch->fingers[i];
            x += finger->x;
            y += finger->y;
        }
        input->centerX = x / numFingers;
        input->centerY = y / numFingers;

        if(!firstMultiple_) firstMultiple_ = input;
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
        firstDistance = sqrt(deltaX * deltaX + deltaY * deltaY);

        deltaX = input->fingers[0]->x - input->fingers[1]->x;
        deltaY = input->fingers[0]->y - input->fingers[1]->y;
        currDistance = sqrt(deltaX * deltaX + deltaY * deltaY);

        input->scale = currDistance / firstDistance;

        //rotate
        float firstRotate;
        float currRotate;

        deltaX = firstMultiple_->fingers[0]->x - firstMultiple_->fingers[1]->x;
        deltaY = firstMultiple_->fingers[0]->y - firstMultiple_->fingers[1]->y;
        firstRotate = atan2(deltaY, deltaX) * 180 / M_PI;

        deltaX = input->fingers[0]->x - input->fingers[1]->x;
        deltaY = input->fingers[0]->y - input->fingers[1]->y;
        currRotate = atan2(deltaY, deltaX) * 180 / M_PI;

        input->rotation = currRotate - firstRotate;


        input->deltaX = deltaX = input->centerX - firstMultiple_->centerX;
        input->deltaY = deltaY = input->centerY - firstMultiple_->centerY;
    }
    else
    {
        input->deltaX = deltaX = input->centerX - firstInput_->centerX;
        input->deltaY = deltaY = input->centerY - firstInput_->centerY;
    }

    input->angle = atan2(deltaY, deltaX) * 180 / M_PI;
    input->distance = sqrt(deltaX * deltaX + deltaY * deltaY);

    if(deltaX == deltaY)
    {
        input->offsetDirection = DIRECTION_NONE;
    }
    else if(abs(deltaX) >= abs(deltaY))
    {
        input->offsetDirection = deltaX > 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
    }
    else
    {
        input->offsetDirection = deltaY > 0 ? DIRECTION_UP : DIRECTION_DOWN;
    }


    // compute interval
    if(!lastInterval_) lastInterval_ = input;

    float deltaTime = input->timeStamp - lastInterval_->timeStamp,
    float velocity, velocityX, velocityY;
    uint32_t direction;

    if (deltaTime > COMPUTE_INTERVAL || lastInterval_->velocity < 0)
    {
        deltaX = lastInterval_->deltaX - input->deltaX;
        deltaY = lastInterval_->deltaY - input->deltaY;

        velocityX = Math.abs(deltaX / deltaTime);
        velocityY = Math.abs(deltaY / deltaTime);
        velocity = std::max(velocityX, velocityY);

        if(deltaX == deltaY)
        {
            direction = DIRECTION_NONE;
        }
        else if(abs(deltaX) >= abs(deltaY))
        {
            direction = deltaX > 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
        }
        else
        {
            direction = deltaY > 0 ? DIRECTION_UP : DIRECTION_DOWN;
        }
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


    this->recognize(input);

    prevFingers_ = numFingers;
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




Recognizer::Recognizer()
{
}


void Recognizer::recognize(Input * input)
{
}


uint32_t Recognizer::process(Input * input)
{
    return STATE_FAILED;
}


TapRecognizer::TapRecognizer()
{
    prevTimeStamp_ = 0;
    prevCenterX_ = 0;
    prevCenterY_ = 0;
    tapCount_ = 0;

    pointers_ = 1;
    taps_ = 1;
    interval_ = 300;
    taptime_ = 250;
    moveThreshold_ = 10;
    offsetThreshold_ = 10;
}


uint32_t TapRecognizer::process(Input * input)
{
    if(input->numFingers == pointers_ && input->distance < moveThreshold_ && input->deltaTime < taptime_)
    {
        if(input->type == INPUT_END)
        {
            bool validInterval = prevTimeStamp_ > 0 ? (input->timeStamp - prevTimeStamp_ < interval_) : true;

            float deltaX = input->centerX - prevCenterX_;
            float deltaY = input->centerY - prevCenterY_;
            float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

            if(validInterval && distance < offsetThreshold_)
            {
                tapCount_ = tapCount_ + 1;
            }
            else
            {
                tapCount_ = 1;
            }
        }
    }
}
