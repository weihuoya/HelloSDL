

enum INPUT_DIRECTION {
    DIRECTION_NONE,
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_HORIZONTAL,
    DIRECTION_VERTICAL,
    DIRECTION_ALL,
};


enum INPUT_TYPE {
    INPUT_START,
    INPUT_MOVE,
    INPUT_END,
    INPUT_CANCEL,
};


enum RECOGNIZE_STATE {
    STATE_POSSIBLE,
    STATE_BEGAN,
    STATE_CHANGED,
    STATE_ENDED,
    STATE_RECOGNIZED,
    STATE_CANCELLED,
    STATE_FAILED,
};



struct Input
{
    bool isFirst;
    bool isFinal;

    uint32_t type;

    uint32_t timeStamp;
    uint32_t deltaTime;
    uint32_t direction;
    uint32_t offsetDirection;

    float angle;
    float distance;
    float scale;
    float rotation;

    float velocityX;
    float velocityY;

    float deltaX;
    float deltaY;

    float centerX;
    float centerY;
};


class Hammer
{
public:
    static Hammer * instance();

    void OnTouchEvent(const SDL_TouchFingerEvent& event, const SDL_Touch * touch);
    void recognize(Input * input);

    void OnPanEvent(const Input * input);
    void OnPinchEvent(const Input * input);
    void OnSwipeEvent(const Input * input);
    void OnTapEvent(const Input * input);
    void OnDoubleTapEvent(const Input * input);
    void OnLongPressEvent(const Input * input);

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(Hammer);

    size_t prevFingers_;

    Input * firstInput_;
    Input * firstMultiple_;
    Input * lastInterval_;
};


class Recognizer
{
public:
    void recognize(Input * input);

private:
    uint32_t state_;
};
