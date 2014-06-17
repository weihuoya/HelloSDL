#include "sdlrwops.h"


SDLRWops::SDLRWops() : rwops_(NULL)
{
}


SDLRWops::~SDLRWops()
{
    if (rwops_) {
        this->close();
    }
}


bool SDLRWops::fromFile(const char *file, const char *mode)
{
    rwops_ = SDL_RWFromFile(file, mode);
    return (rwops_ != NULL);
}

#ifdef HAVE_STDIO_H
bool SDLRWops::fromFP(FILE * fp, bool autoclose)
#else
bool SDLRWops::fromFP(void * fp, bool autoclose)
#endif
{
    rwops_ = SDL_RWFromFP(fp, (autoclose ? SDL_TRUE : SDL_FALSE));
    return (rwops_ != NULL);
}

bool SDLRWops::fromMem(void *mem, int size)
{
    rwops_ = SDL_RWFromMem(mem, size);
    return (rwops_ != NULL);
}

bool SDLRWops::fromConstMem(const void *mem, int size)
{
    rwops_ = SDL_RWFromConstMem(mem, size);
    return (rwops_ != NULL);
}


size_t SDLRWops::size()
{
    return SDL_RWsize(rwops_);
}

size_t SDLRWops::seek(size_t offset, int whence)
{
    return SDL_RWseek(rwops_, offset, whence);
}

size_t SDLRWops::tell()
{
    return SDL_RWtell(rwops_);
}

size_t SDLRWops::read(void *ptr, size_t size, size_t num)
{
    return SDL_RWread(rwops_, ptr, num, size);
}

size_t SDLRWops::write(const void *ptr, size_t size, size_t num)
{
    return SDL_RWwrite(rwops_, ptr, num, size);
}

int SDLRWops::close()
{
    int ret = SDL_RWclose(rwops_);
    rwops_ = NULL;
    return ret;
}
