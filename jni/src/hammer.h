

struct Pointer
{
};


struct InputData
{
    uint32_t timeStamp;
    std::vector<struct{float clientX; float clientY;}> pointers;

    float centerX;
    float centerY;

    float deltaX;
    float deltaY;
};


class Input
{
public:
    enum DIRECTION {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT,
    };

    void inputHandler(manager, eventType, input);
    void computeInputData(manager, input);
    void computeIntervalInputData(manager, input);

    bool isFirst;
    bool isFinal;

    uint32_t eventType;

    std::vector<Pointer> pointers;
    std::vector<Pointer> changedPointers;

    uint32_t timeStamp;
    uint32_t deltaTime;
    float deltaX;
    float deltaY;
    float centerX;
    float centerY;
    float angle;
    float distance;
    uint32_t offsetDirection;

    float scale;
    float rotation;
};


class Manager
{
public:

    InputData * simpleCloneInputData(input);

    void recognize(input);

    InputData * firstInput;
    InputData * firstMultiple;
    InputData * lastInterval;
};
