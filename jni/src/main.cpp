#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>


#include "eventhandler.h"
#include "renderer.h"
#include "glcontext.h"
#include "gltest.h"


#if defined(__IPHONEOS__) || defined(__ANDROID__)
#include <SDL_opengles.h>
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif


static SDL_DisplayMode gDisplayMode;
static SDL_Window * gWindow;

static int RunEventLoop();

static void initGL(SDL_Window * window);
static void initRenderer(SDL_Window * window);


int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log(SDL_GetError());
        return -1;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");

    if (TTF_Init() != 0) {
        SDL_Log(TTF_GetError());
    }

    if (SDL_GetCurrentDisplayMode(0, &gDisplayMode) == 0) {
        SDL_Log("Display #%d: %dx%d", 0, gDisplayMode.w, gDisplayMode.h);
    } else {
        SDL_Log(SDL_GetError());
    }

    SDL_Window * window = SDL_CreateWindow(
            "Hello SDL",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            gDisplayMode.w,
            gDisplayMode.h,
            SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS
            );

    //initRenderer(window);
    gWindow = window;
    initGL(window);
    //RunEventLoop();

    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    return 0;
}


void initGL(SDL_Window * window)
{
    //GLContext glcontext(window);

    //glLoadIdentity();
    //glClearColor(0.7, 0.7, 0.7, 1.0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glcontext.draw();


    RunEventLoop();

    /*gGLContext = SDL_GL_CreateContext(window);

    // setup the viewport
    glViewport(0, 0, gDisplayMode.w, gDisplayMode.h);

     // Clear the color and depth buffers.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Swap our back buffer to the front
    SDL_GL_SwapWindow(window);
    glClear(GL_COLOR_BUFFER_BIT);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // This makes our buffer swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);*/
}


void initRenderer(SDL_Window * window)
{
    Renderer renderer(window);
    renderer.Clear();

    SDL_Texture * texBackground = renderer.LoadImage("zhaocaifu.jpg");
    renderer.Copy(texBackground);

    //SDL_Color color = { 255, 255, 255 };
    //SDL_Texture * texLabel = renderer.LoadLabel("TTF fonts are cool!", "FZLiBian.ttf", color, 64);
    //renderer.Copy(texLabel);

    renderer.Present();

    RunEventLoop();

    //SDL_DestroyTexture(texLabel);
    //SDL_DestroyTexture(texBackground);
}

int RunEventLoop()
{
    int done = 0;
    SDL_Event event;
    EventHandler handler;

    GLTest test(gWindow);

    while (!done) {
        test.drawFrame();
        while (SDL_PollEvent(&event)) {
            done = handler.OnEventReceived(event);
        }
    }

    return 0;
}


