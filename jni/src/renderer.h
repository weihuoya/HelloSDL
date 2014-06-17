
#ifndef __HelloSDL__renderer__
#define __HelloSDL__renderer__

#include <SDL2/SDL.h>
#include <vector>


class Renderer
{
public:
    Renderer(SDL_Window * window, int index = -1, Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    ~Renderer();

    void Present();
    int Clear();
    int Copy(SDL_Texture * texture, const SDL_Rect * srcrect = NULL, const SDL_Rect * dstrect = NULL);

    SDL_Texture* LoadImage(const char* filename);
    SDL_Texture* LoadBMP(const char* filename);
    SDL_Texture* LoadLabel(const char * message, const char * fontname, const SDL_Color& fontcolor, int fontsize);

private:
    std::vector<SDL_Texture*> textures_;
    SDL_Renderer * renderer_;
};


#endif /* defined(__HelloSDL__renderer__) */
