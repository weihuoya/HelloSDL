#include <SDL_log.h>

#include "input.h"


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


