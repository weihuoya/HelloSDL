



class GestureDetector
{
public:
    enum GestureType
    {
        NoGesture;
        PanGesture;
        PinchGesture;
        SwipeGesture;
        TapGesture;
        TapAndHoldGesture;
    };

    GestureDetector() : gestureType_(NoGesture) {}

    bool TapGesture(const SDL_TouchFingerEvent& event)
    {
        static int posx, posy;

        bool finished = false;
        int x = width_ * event.x;
        int y = height_* event.y;

        switch(event.type)
        {
        case SDL_FINGERDOWN:
            posx = x;
            posy = y;
            break;
        case SDL_FINGERMOTION:
        case SDL_FINGERUP:
            if(SDL_GetNumTouchFingers(event.touchId) == 1)
            {
                if(abs(x - posx) + abs(y - posy) > 40)
                {
                    finished = true;
                }
                else
                {
                    gestureType_ = TapGesture;
                }
            }
            else
            {
                finished = true;
            }
            break;
        }

        return finished;
    }

    bool TapAndHoldGesture(const SDL_TouchFingerEvent& event)
    {
        static int posx, posy, timestamp;

        bool finished = false;
        int x = width_ * event.x;
        int y = height_* event.y;
        int delta = SDL_GetTicks();

        switch(event.type)
        {
        case SDL_FINGERDOWN:
            posx = x;
            posy = y;
            timestamp = delta;
            break;
        case SDL_FINGERMOTION:
        case SDL_FINGERUP:
            if((delta - timestamp) > 500 && SDL_GetNumTouchFingers(event.touchId) == 1)
            {
                if(abs(x - posx) + abs(y - posy) > 40)
                {
                    finished = true;
                }
                else
                {
                    gestureType_ = TapGesture;
                }
            }
            else
            {
                finished = true;
            }
            break;
        }

        return finished;
    }

private:
    GestureType gestureType_;
    int width_, height_;
};
