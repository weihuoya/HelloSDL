#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>


#include "eventhandler.h"
#include "glcontext.h"

#include "timer.h"


#if defined(__IPHONEOS__) || defined(__ANDROID__)
#include <SDL_opengles.h>
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif


static int RunEventLoop(SDL_Window * window);
static int testTimer();


int main(int argc, char** argv)
{
    SDL_DisplayMode displayMode;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        SDL_Log(SDL_GetError());
        return -1;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");

    if (TTF_Init() != 0) {
        SDL_Log(TTF_GetError());
    }

    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
        SDL_Log("Display #%d: %dx%d", 0, displayMode.w, displayMode.h);
    } else {
        SDL_Log(SDL_GetError());
    }

    SDL_Window * window = SDL_CreateWindow(
            "Hello SDL",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            displayMode.w,
            displayMode.h,
            SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS
            );

    //testTimer();
    RunEventLoop(window);

    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}


int RunEventLoop(SDL_Window * window)
{
    int done = 0;
    SDL_Event event;

    EventHandler * handler = EventHandler::instance();
    GLContext * context = GLContext::instance();

    context->initialize(window);

    while (!done) {
        context->drawFrame();
        while (SDL_PollEvent(&event)) {
            done = handler->OnEventReceived(event);
        }
    }

    return 0;
}


static uint32_t timerId = 0;

static void testInterval(uint32_t tick)
{
    SDL_Log("[timer] interval: %u", tick);
}

static void testTimeout(uint32_t tick)
{
    SDL_Log("[timer] timeout: %u", tick);
    Timer::instance()->remove(timerId);
}

int testTimer()
{
    Timer * timer = Timer::instance();
    timerId = timer->add(Timer::Callback(&testInterval), Timer::TIMER_INTERVAL, 100);
    timer->add(Timer::Callback(&testTimeout), Timer::TIMER_TIMEOUT, 1000);
    timer->start();
}
