#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>


#include "eventhandler.h"
#include "glcontext.h"


#if defined(__IPHONEOS__) || defined(__ANDROID__)
#include <SDL_opengles.h>
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif


static int RunEventLoop(SDL_Window * window);


int main(int argc, char** argv)
{
    SDL_DisplayMode displayMode;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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
    EventHandler handler;
    GLContext context(window);

    context.initialize();

    while (!done) {
        context.drawFrame();
        while (SDL_PollEvent(&event)) {
            done = handler.OnEventReceived(event);
        }
    }

    return 0;
}


