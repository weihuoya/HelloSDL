#include "hammer.h"


#define COMPUTE_INTERVAL 25


Hammer * Hammer::instance()
{
    static Hammer handler;
    return &handler;
}


Hammer::Hammer() : prevFingers_(0), firstInput_(0), firstMultiple_(0), lastInterval_(0)
{
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

