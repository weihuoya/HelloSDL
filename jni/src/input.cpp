#include <SDL_log.h>

#include "input.h"


#define COMPUTE_INTERVAL 25



Input::Input() :
    //numFingers(0),
    deltaTime(0), direction(DIRECTION_NONE),
    velocity(0), velocityX(0), velocityY(0),
    isFirst(false), isFinal(false)
{
}

Input::Input(const Input& rhs)
{
    fingers = rhs.fingers;
    //numFingers = rhs.numFingers;

    type = rhs.type;
    timeStamp = rhs.timeStamp;

    angle = rhs.angle;
    distance = rhs.distance;
    scale = rhs.scale;
    rotation = rhs.rotation;

    deltaTime = rhs.deltaTime;
    direction = rhs.direction;
    velocityX = rhs.velocityX;
    velocityY = rhs.velocityY;

    offsetDirection = rhs.offsetDirection;
    deltaX = rhs.deltaX;
    deltaY = rhs.deltaY;

    centerX = rhs.centerX;
    centerY = rhs.centerY;

    isFirst = rhs.isFirst;
    isFinal = rhs.isFinal;
}

Input::~Input()
{
}

void Input::swap(Input& rhs)
{
    std::swap(fingers, rhs.fingers);
    //std::swap(numFingers, rhs.numFingers);

    std::swap(type, rhs.type);
    std::swap(timeStamp, rhs.timeStamp);

    std::swap(angle, rhs.angle);
    std::swap(distance, rhs.distance);
    std::swap(scale, rhs.scale);
    std::swap(rotation, rhs.rotation);

    std::swap(deltaTime, rhs.deltaTime);
    std::swap(direction, rhs.direction);
    std::swap(velocityX, rhs.velocityX);
    std::swap(velocityY, rhs.velocityY);

    std::swap(offsetDirection, rhs.offsetDirection);
    std::swap(deltaX, rhs.deltaX);
    std::swap(deltaY, rhs.deltaY);

    std::swap(centerX, rhs.centerX);
    std::swap(centerY, rhs.centerY);

    std::swap(isFirst, rhs.isFirst);
    std::swap(isFinal, rhs.isFinal);
}

void Input::setFingers(const SDL_Touch * touch)
{
    SDL_Finger * p, * f;
    int i = 0;
    int numFingers = touch->num_fingers;
    int size = fingers.size();

    for(i = 0; i < numFingers; ++i)
    {
        if(i < size)
        {
            p = fingers.at(i).get();
        }
        else
        {
            p = new SDL_Finger;
            fingers.emplace_back(p);
        }

        f = touch->fingers[i];
        memcpy(p, f, sizeof(SDL_Finger));
    }

    fingers.resize(numFingers);
}

void Input::dump() const
{
    size_t k = 0;
    char buff[2048];
    const char s_true[] = "true";
    const char s_false[] = "false";


    k += sprintf(buff+k, "\ntype: ");
    if(type == INPUT_START)
    {
        k += sprintf(buff+k, "start");
    }
    else if(type == INPUT_MOVE)
    {
        k += sprintf(buff+k, "move");
    }
    else
    {
        k += sprintf(buff+k, "end");
    }

    k += sprintf(buff+k, "\npointers: %u", fingers.size());
    k += sprintf(buff+k, "\ntimestamp: %u", timeStamp);
    k += sprintf(buff+k, "\nangle: %.02f", angle);
    k += sprintf(buff+k, "\ndistance: %.02f", distance);
    k += sprintf(buff+k, "\nscale: %.02f", scale);
    k += sprintf(buff+k, "\nrotation: %.02f", rotation);

    k += sprintf(buff+k, "\ndirection: ");
    if(direction == DIRECTION_UP)
    {
        k += sprintf(buff+k, "up");
    }
    else if(direction == DIRECTION_DOWN)
    {
        k += sprintf(buff+k, "down");
    }
    else if(direction == DIRECTION_LEFT)
    {
        k += sprintf(buff+k, "left");
    }
    else if(direction == DIRECTION_RIGHT)
    {
        k += sprintf(buff+k, "right");
    }

    k += sprintf(buff+k, "\nvelocityX: %.02f", velocityX);
    k += sprintf(buff+k, "\nvelocityY: %.02f", velocityY);

    k += sprintf(buff+k, "\nfirst: %s", isFirst ? s_true : s_false);
    k += sprintf(buff+k, "\nfinal: %s", isFinal ? s_true : s_false);


    SDL_Log("[input]\n%s", buff);
}



Hammer * Hammer::instance()
{
    static Hammer handler;
    return &handler;
}

Hammer::Hammer() : prevFingers_(0)
{
}


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
    uint32_t direction = DIRECTION_NONE;

    if(x == y)
    {
        direction = DIRECTION_NONE;
    }
    else if(abs(x) >= abs(y))
    {
        direction = x > 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
    }
    else
    {
        direction = y > 0 ? DIRECTION_UP : DIRECTION_DOWN;
    }

    return direction;
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
        eventType = INPUT_START;
    }
    else if(event.type == SDL_FINGERMOTION)
    {
        eventType = INPUT_MOVE;
    }
    else //if(event.type == SDL_FINGERUP)
    {
        isFinal = (numFingers == 0);
        eventType = INPUT_END;
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

