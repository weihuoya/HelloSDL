
#ifndef __hellosdl__sdlrwops__
#define __hellosdl__sdlrwops__


#include <SDL.h>
#include <stdio.h>


class SDLRWops
{
public:
    enum RWSeek {
        SeekSet = 0,
        SeekCur = 1,
        SeekEnd = 2
    };

    SDLRWops();
    ~SDLRWops();

    bool fromFile(const char *file, const char *mode);

#ifdef HAVE_STDIO_H
    bool fromFP(FILE * fp, bool autoclose = true);
#else
    bool fromFP(void * fp, bool autoclose = true);
#endif

    bool fromMem(void *mem, int size);
    bool fromConstMem(const void *mem, int size);

    size_t size();
    size_t seek(size_t offset, int whence);
    size_t tell();
    size_t read(void *ptr, size_t size, size_t num = 1);
    size_t write(const void *ptr, size_t size, size_t num = 1);
    int close();

private:
    SDL_RWops * rwops_;
};


#endif /* defined(__hellosdl__sdlrwops__) */
