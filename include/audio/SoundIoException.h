/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 05/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_SOUNDIOEXCEPTION_H
#define DICTA_SOUNDIOEXCEPTION_H

#include <stdexcept>
#include <soundio/soundio.h>

class SoundIoException : public std::runtime_error
{
    const std::string messageHeader{"LibSoundIo error:"};
    
    public:
    SoundIoException(const std::string& message) : std::runtime_error(messageHeader + message)
    {}
    SoundIoException(const std::string& message, int error) : std::runtime_error(
            messageHeader + message + ", " + soundio_strerror(error))
    {}
    
    static SoundIoException&& OutOfMemory(const std::string& thing)
    {
        SoundIoException e("Unable to allocate memory to " + thing);
        return std::move(e);
    }
};

#endif //DICTA_SOUNDIOEXCEPTION_H
