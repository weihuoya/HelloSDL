#ifndef __hellosdl__recognizer__
#define __hellosdl__recognizer__

#include "input.h"




class Recognizer
{
public:
    enum RECOGNIZER_TYPE {
        TYPE_TAP,
        TYPE_PAN,
        TYPE_PINCH,
        TYPE_ROTATE,
        TYPE_SWIPE,
    };

    enum RECOGNIZE_STATE {
        STATE_POSSIBLE,
        STATE_BEGAN,
        STATE_CHANGED,
        STATE_RECOGNIZED,
        STATE_CANCELLED,
        STATE_FAILED,
    };

    virtual ~Recognizer();

    void recognize(const Input * input);
    void trigger();

    virtual uint32_t process(const Input * input);
    virtual void reset();

protected:
    Recognizer(RECOGNIZER_TYPE type);

    uint32_t type_;
    uint32_t state_;
};


class TapRecognizer : public Recognizer
{
public:
    TapRecognizer();
    virtual ~TapRecognizer();

protected:
    virtual uint32_t process(const Input * input);
    virtual void reset();

    uint32_t timer_;

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
    virtual uint32_t process(const Input * input);
    virtual void reset();

    // config
    size_t pointers_;
    size_t threshold_;
    size_t direction_;
    //previous
    float previousX_;
    float previousY_;
};


class PinchRecognizer : public Recognizer
{
public:
    PinchRecognizer();
    virtual ~PinchRecognizer();

protected:
    virtual uint32_t process(const Input * input);
    virtual void reset();

    // config
    size_t pointers_;
    float threshold_;
};


class RotateRecognizer : public Recognizer
{
public:
    RotateRecognizer();
    virtual ~RotateRecognizer();

protected:
    virtual uint32_t process(const Input * input);
    virtual void reset();

    // config
    size_t pointers_;
    float threshold_;
};


class SwipeRecognizer : public Recognizer
{
public:
    SwipeRecognizer();
    virtual ~SwipeRecognizer();

protected:
    virtual uint32_t process(const Input * input);
    virtual void reset();

    // config
    size_t pointers_;
    float threshold_;
    size_t direction_;
};


#endif /* defined(__HelloSDL__recognizer__) */
