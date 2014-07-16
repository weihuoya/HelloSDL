#include "recognizer.h"



Recognizer::Recognizer()
{
}

Recognizer::~Recognizer()
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

TapRecognizer::~TapRecognizer()
{
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




PanRecognizer::PanRecognizer()
{
}

PanRecognizer::~PanRecognizer()
{
}

uint32_t PanRecognizer::process(Input * input)
{
}




PinchRecognizer::PinchRecognizer()
{
}

PinchRecognizer::~PinchRecognizer()
{
}

uint32_t PinchRecognizer::process(Input * input)
{
}




RotateRecognizer::RotateRecognizer()
{
}

RotateRecognizer::~RotateRecognizer()
{
}

uint32_t RotateRecognizer::process(Input * input)
{
}




SwipeRecognizer::SwipeRecognizer()
{
}

SwipeRecognizer::~SwipeRecognizer()
{
}

uint32_t SwipeRecognizer::process(Input * input)
{
}
