#include "recognizer.h"
#include "hammer.h"



////////////////////////////////////////////////////////////////////////////////
// base
////////////////////////////////////////////////////////////////////////////////
Recognizer::Recognizer(RECOGNIZER_TYPE type) : type_(type), state_(Recognizer::STATE_POSSIBLE)
{
}

Recognizer::~Recognizer()
{
}

void Recognizer::recognize(const Input * input)
{
    if(state_ == Recognizer::STATE_POSSIBLE || state_ == Recognizer::STATE_BEGAN || state_ == STATE_CHANGED)
    {
        this->process(input);
    }
}

void Recognizer::trigger()
{
    Hammer::instance()->trigger(type_);
}

uint32_t Recognizer::process(const Input * input)
{
    return 0;
}

void Recognizer::reset()
{
    state_ = Recognizer::STATE_POSSIBLE;
}



////////////////////////////////////////////////////////////////////////////////
// tap
////////////////////////////////////////////////////////////////////////////////
TapRecognizer::TapRecognizer() : Recognizer(Recognizer::TYPE_TAP)
{
    timer_ = 0;

    prevTimeStamp_ = -1;
    prevCenterX_ = -1;
    prevCenterY_ = -1;
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

uint32_t TapRecognizer::process(const Input * input)
{
    uint32_t state = 0;
    uint32_t size = input->fingers.size();

    if(size == pointers_ && input->distance < moveThreshold_ && input->deltaTime < taptime_)
    {
        if(input->type == Input::INPUT_START && tapCount_ == 0)
        {
            Timer::Callback callback = [this] (uint32_t x) -> void {
                        this->state_ = Recognizer::STATE_FAILED;
                    };
            timer_ = Hammer::instance()->setTimeout(callback, interval_);
        }
        else if(input->type == Input::INPUT_END)
        {
            float distance = 0;
            bool validInterval = prevTimeStamp_ > 0 ? (input->timeStamp - prevTimeStamp_ < interval_) : true;

            if(prevCenterX_ > 0)
            {
                float deltaX = input->centerX - prevCenterX_;
                float deltaY = input->centerY - prevCenterY_;
                distance = sqrt(deltaX * deltaX + deltaY * deltaY);
            }
            else
            {
                prevCenterX_ = input->centerX;
                prevCenterY_ = input->centerY;
            }

            if(validInterval && distance < offsetThreshold_)
            {
                Timer::Callback callback = [this] (uint32_t x) -> void {
                            this->state_ = Recognizer::STATE_RECOGNIZED;
                            this->trigger();
                        };
                timer_ = Hammer::instance()->setTimeout(callback, interval_);

                tapCount_ = tapCount_ + 1;
            }
            else
            {
                tapCount_ = 0;
            }
        }
    }
    else
    {
        state_ = Recognizer::STATE_FAILED;
    }
}


void TapRecognizer::reset()
{
    state_ = Recognizer::STATE_POSSIBLE;

    Hammer::instance()->clearTimeout(timer_);
    timer_ = 0;

    prevTimeStamp_ = -1;
    prevCenterX_ = -1;
    prevCenterY_ = -1;
    tapCount_ = 0;
}


////////////////////////////////////////////////////////////////////////////////
// pan
////////////////////////////////////////////////////////////////////////////////
PanRecognizer::PanRecognizer() : Recognizer(Recognizer::TYPE_PAN)
{
    pointers_ = 1;
    threshold_ = 10;
    direction_ = Input::DIRECTION_ALL;

    previousX_ = -1;
    previousY_ = -1;
}

PanRecognizer::~PanRecognizer()
{
}

uint32_t PanRecognizer::process(const Input * input)
{
    size_t size = input->fingers.size();

    if(size == pointers_)
    {
        float x;
        float y;

        size_t directionX;
        size_t directionY;

        if(previousX_ > 0)
        {
            x = input->deltaX - previousX_;
            y = input->deltaY - previousY_;
        }
        else
        {
            x = input->deltaX;
            y = input->deltaY;
        }

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

        //
        if(directionX != Input::DIRECTION_NONE || directionY != Input::DIRECTION_NONE)
        {
            state_ = Recognizer::STATE_RECOGNIZED;
            trigger();
        }
    }
    else
    {
        state_ = Recognizer::STATE_FAILED;
    }
}


void PanRecognizer::reset()
{
    state_ = Recognizer::STATE_POSSIBLE;

    previousX_ = -1;
    previousY_ = -1;
}




////////////////////////////////////////////////////////////////////////////////
// pinch
////////////////////////////////////////////////////////////////////////////////
PinchRecognizer::PinchRecognizer() : Recognizer(Recognizer::TYPE_PINCH)
{
    pointers_ = 2;
    threshold_ = 0;
}

PinchRecognizer::~PinchRecognizer()
{
}

uint32_t PinchRecognizer::process(const Input * input)
{
    size_t size = input->fingers.size();

    if(size == pointers_)
    {
        float scale = abs(input->scale);
        if(scale > threshold_)
        {
            state_ = Recognizer::STATE_RECOGNIZED;
            trigger();
        }
        else if(input->type == Input::INPUT_END)
        {
            state_ = Recognizer::STATE_FAILED;
        }
    }
    else
    {
        state_ = Recognizer::STATE_FAILED;
    }
}

void PinchRecognizer::reset()
{
    state_ = Recognizer::STATE_POSSIBLE;
}




////////////////////////////////////////////////////////////////////////////////
// rotate
////////////////////////////////////////////////////////////////////////////////
RotateRecognizer::RotateRecognizer() : Recognizer(Recognizer::TYPE_ROTATE)
{
    pointers_ = 2;
    threshold_ = 0;
}

RotateRecognizer::~RotateRecognizer()
{
}

uint32_t RotateRecognizer::process(const Input * input)
{
    size_t size = input->fingers.size();
    size_t rotation = abs(input->rotation);

    if(size == pointers_)
    {
        if(rotation > threshold_)
        {
            state_ = Recognizer::STATE_RECOGNIZED;
            trigger();
        }
        else if(input->type == Input::INPUT_END)
        {
            state_ = Recognizer::STATE_FAILED;
        }
    }
    else
    {
        state_ = Recognizer::STATE_FAILED;
    }
}

void RotateRecognizer::reset()
{
    state_ = Recognizer::STATE_POSSIBLE;
}


////////////////////////////////////////////////////////////////////////////////
// swipe
////////////////////////////////////////////////////////////////////////////////
SwipeRecognizer::SwipeRecognizer() : Recognizer(Recognizer::TYPE_SWIPE)
{
    pointers_ = 1;
    threshold_ = 0.65;
    direction_ = Input::DIRECTION_HORIZONTAL | Input::DIRECTION_VERTICAL;
}

SwipeRecognizer::~SwipeRecognizer()
{
}

uint32_t SwipeRecognizer::process(const Input * input)
{
    size_t size = input->fingers.size();

    if(size == pointers_)
    {
        float velocity;

        if(direction_ == Input::DIRECTION_HORIZONTAL)
        {
            velocity = input->velocityX;
        }
        else if(direction_ == Input::DIRECTION_VERTICAL)
        {
            velocity = input->velocityY;
        }
        else
        {
            velocity = input->velocity;
        }

        if(velocity > threshold_ && input->type == Input::INPUT_END)
        {
            state_ = Recognizer::STATE_RECOGNIZED;
            trigger();
        }
    }
    else
    {
        state_ = Recognizer::STATE_FAILED;
    }

}

void SwipeRecognizer::reset()
{
    state_ = Recognizer::STATE_POSSIBLE;
}

