#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_thread.h>

#include "renderer.h"


static int LoadFontThread(void *ptr);


Renderer::Renderer(SDL_Window * window, int index, Uint32 flags)
{
    renderer_ = SDL_CreateRenderer(window, index, flags);
}


Renderer::~Renderer()
{
    std::vector<SDL_Texture*>::const_iterator iter, end = textures_.end();

    for(iter = textures_.begin(); iter != end; ++iter)
    {
        SDL_DestroyTexture(*iter);
    }

    SDL_DestroyRenderer(renderer_);
}

int Renderer::Copy(SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
    SDL_Rect from, to;

    if (srcrect) {
        from.x = srcrect->x;
        from.y = srcrect->y;
        from.w = srcrect->w;
        from.h = srcrect->h;
    } else {
        from.x = 0;
        from.y = 0;
        SDL_QueryTexture(texture, NULL, NULL, &from.w, &from.h);
    }

    if (dstrect) {
        to.x = srcrect->x;
        to.y = srcrect->y;
        to.w = srcrect->w;
        to.h = srcrect->h;
    } else {
        to.x = 0;
        to.y = 0;
        to.w = from.w;
        to.h = from.h;
    }

    return SDL_RenderCopy(renderer_, texture, &from, &to);
}

void Renderer::Present()
{
    SDL_RenderPresent(renderer_);
}

int Renderer::Clear()
{
    return SDL_RenderClear(renderer_);
}

SDL_Texture* Renderer::LoadImage(const char* filename)
{
    SDL_Texture * texture = NULL;

    texture = IMG_LoadTexture(renderer_, filename);
    if (texture == NULL) {
        SDL_Log(IMG_GetError());
    }

    if (texture) {
        textures_.push_back(texture);
    }

    return texture;
}


SDL_Texture* Renderer::LoadBMP(const char* filename)
{
    SDL_Texture * texture = NULL;
    SDL_Surface * loadedImage = SDL_LoadBMP(filename);

    if (loadedImage) {
        texture = SDL_CreateTextureFromSurface(renderer_, loadedImage);
        SDL_FreeSurface(loadedImage);
    }
    else
    {
        SDL_Log(SDL_GetError());
    }

    if (texture) {
        textures_.push_back(texture);
    }

    return texture;
}

SDL_Texture* Renderer::LoadLabel(
        const char * message,
        const char * fontname,
        const SDL_Color& fontcolor,
        int fontsize)
{
    SDL_Texture * texture = NULL;
    TTF_Font * font = TTF_OpenFont(fontname, fontsize);

    /*if (font) {
        SDL_Surface * surface = TTF_RenderText_Blended(font, message, fontcolor);
        texture = SDL_CreateTextureFromSurface(renderer_, surface);

        SDL_FreeSurface(surface);
        TTF_CloseFont(font);
    } else {
        SDL_Log(TTF_GetError());
    }*/

    SDL_Thread *thread = SDL_CreateThread(LoadFontThread, "LoadFontThread", (void *)renderer_);
    if (thread) {
        SDL_DetachThread(thread);
    } else {
        SDL_Log(SDL_GetError());
    }

    if (texture) {
        textures_.push_back(texture);
    }

    return texture;
}


int LoadFontThread(void *ptr)
{
    unsigned int time1 = 0, time2 = 0;
    SDL_Renderer * renderer = (SDL_Renderer *)ptr;

    time1 = SDL_GetTicks();

    TTF_Font * font = TTF_OpenFont("FZLiBian.ttf", 64);

    if (font) {
        time2 = SDL_GetTicks();
        TTF_CloseFont(font);
    } else {
        SDL_Log(TTF_GetError());
    }

    SDL_Log("font profile time: %d, %d", time2 - time1, SDL_GetTicks() - time2);

    return 0;
}
