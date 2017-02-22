/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 05/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../include/audio/AudioHandler.h"

int main(int argc, char** argv)
{
    Dicta::AudioHandler audioHandler{};
    
    auto future = std::async(
            std::launch::async,
            Dicta::AudioHandler::readRecordingBuffer,
            audioHandler.getRingBuffer()
    );
    
    std::cerr << audioHandler << std::endl;
    
    return 0;
}