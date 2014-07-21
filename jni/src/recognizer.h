#ifndef __hellosdl__recognizer__
#define __hellosdl__recognizer__

#include "input.h"


enum RECOGNIZE_STATE {
    STATE_POSSIBLE,
    STATE_BEGAN,
    STATE_CHANGED,
    STATE_ENDED,
    STATE_RECOGNIZED,
    STATE_CANCELLED,
    STATE_FAILED,
};


class Recognizer
{
public:
    virtual ~Recognizer();
    void recognize(Input * input);

protected:
    Recognizer();
    virtual uint32_t process(Input * input);

    uint32_t state_;
};


class TapRecognizer : public Recognizer
{
public:
    TapRecognizer();
    virtual ~TapRecognizer();

protected:
    virtual uint32_t process(Input * input);

    // previous
    float prevTimeStamp_;
    float prevCenterX_;
    float prevCenterY_;
    size_t tapCount_;

    // config
    size_t pointers_;
    size_t taps_;
    size_t interval_;
    size_t taptime_;
    size_t moveThreshold_;
    size_t offsetThreshold_;
};


class PanRecognizer : public Recognizer
{
public:
    PanRecognizer();
    virtual ~PanRecognizer();

protected:
    virtual uint32_t process(Input * input);

    // config
    size_t pointers_;
    size_t threshold_;
    size_t direction_;
};


class PinchRecognizer : public Recognizer
{
public:
    PinchRecognizer();
    virtual ~PinchRecognizer();

protected:
    virtual uint32_t process(Input * input);

    // config
    size_t pointers_;
    size_t threshold_;
    size_t direction_;
};


class RotateRecognizer : public Recognizer
{
public:
    RotateRecognizer();
    virtual ~RotateRecognizer();

protected:
    virtual uint32_t process(Input * input);

    // config
    size_t pointers_;
    size_t threshold_;
    size_t direction_;
};


class SwipeRecognizer : public Recognizer
{
public:
    SwipeRecognizer();
    virtual ~SwipeRecognizer();

protected:
    virtual uint32_t process(Input * input);

    // config
    size_t pointers_;
    size_t threshold_;
    size_t direction_;
};


#endif /* defined(__HelloSDL__recognizer__) */
