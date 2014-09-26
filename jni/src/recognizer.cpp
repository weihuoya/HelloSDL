#include "recognizer.h"
#include "hammer.h"



Recognizer::Recognizer() : type_(0)
{
}

Recognizer::~Recognizer()
{
}

void Recognizer::recognize(Input * input)
{
}

void Recognizer::trigger(Input * input)
{
    Hammer::instance()->trigger(type_, state_, input);
}

uint32_t Recognizer::process(Input * input)
{
    return 0;
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

TapRecognizer::~TapRecognizer()
{
}

uint32_t TapRecognizer::process(Input * input)
{
    uint32_t state = 0;
    uint32_t size = input->fingers.size();

    if(size == pointers_ && input->distance < moveThreshold_ && input->deltaTime < taptime_)
    {
        if(input->type == Input::INPUT_END)
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

            // event, timer, callback
            //Hammer::instance()->setTimeout([] (uint32_t x){}, interval_);
            //hammer->timer:cancel(timerId);
            //timerId = 0;
        }
    }
}




PanRecognizer::PanRecognizer()
{
    pointers_ = 1;
    threshold_ = 10;
    direction_ = Input::DIRECTION_ALL;

    previousX_ = 0;
    previousY_ = 0;
}

PanRecognizer::~PanRecognizer()
{
}

uint32_t PanRecognizer::process(Input * input)
{
    size_t directionX;
    size_t directionY;

    size_t x = input->deltaX - previousX_;
    size_t y = input->deltaY - previousY_;

    // 水平
    if(x > threshold_)
    {
        directionX = Input::DIRECTION_RIGHT;
    }
    else if(x < -threshold_)
    {
        directionX = Input::DIRECTION_LEFT;
    }
    else
    {
        directionX = Input::DIRECTION_NONE;
    }

    // 垂直
    if(y > threshold_)
    {
        directionY = Input::DIRECTION_UP;
    }
    else if(y < -threshold_)
    {
        directionY = Input::DIRECTION_DOWN;
    }
    else
    {
        directionY = Input::DIRECTION_NONE;
    }

    return directionX & directionY;
}




PinchRecognizer::PinchRecognizer()
{
    pointers_ = 2;
    threshold_ = 0;
}

PinchRecognizer::~PinchRecognizer()
{
}

uint32_t PinchRecognizer::process(Input * input)
{
    size_t size = input->fingers.size();
    size_t scale = abs(input->scale);

    if(size == pointers_ && scale > threshold_)
    {
    }
    else
    {
    }
}




RotateRecognizer::RotateRecognizer()
{
    pointers_ = 2;
    threshold_ = 0;
}

RotateRecognizer::~RotateRecognizer()
{
}

uint32_t RotateRecognizer::process(Input * input)
{
    size_t size = input->fingers.size();
    size_t rotation = abs(input->rotation);

    if(size == pointers_ && rotation > threshold_)
    {
    }
    else
    {
    }
}




SwipeRecognizer::SwipeRecognizer()
{
    pointers_ = 1;
    threshold_ = 10;
    velocity_ = 0.65;
    direction_ = Input::DIRECTION_HORIZONTAL | Input::DIRECTION_VERTICAL;
}

SwipeRecognizer::~SwipeRecognizer()
{
}

uint32_t SwipeRecognizer::process(Input * input)
{
    size_t size = input->fingers.size();
    size_t velocity = input->velocity;
    size_t velocityX = input->velocityX;
    size_t velocityY = input->velocityY;
}
