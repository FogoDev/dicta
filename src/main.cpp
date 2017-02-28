/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 05/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include <future>
#include "../include/audio/AudioHandler.h"
#include "../include/preprocessor/PreProcessor.h"

int main(int argc, char** argv)
{
    Dicta::AudioHandler audioHandler{};
    
    Dicta::PreProcessor preProcessor(audioHandler.getSampleRate());
    
    audioHandler.startInputStream();
    
    auto future = std::async(
            std::launch::async,
            Dicta::PreProcessor::readFrameAndWindowRecordingBuffer,
            audioHandler.getCircularBuffer(),
            &preProcessor
    );
    
    std::cerr << audioHandler << std::endl;
    
    preProcessor.report();
    
    return 0;
}