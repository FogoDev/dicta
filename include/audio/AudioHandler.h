/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 05/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_AUDIOHANDLER_H
#define DICTA_AUDIOHANDLER_H

#include <iostream>
#include <array>
#include <boost/circular_buffer.hpp>
#include <soundio/soundio.h>
#include "SoundIoException.h"

namespace Dicta
{
    class AudioHandler
    {
        friend std::ostream& operator<<(std::ostream& out, const AudioHandler& o);
        
        public:
        AudioHandler();
        ~AudioHandler() noexcept;
        
        // Deleted copy and move constructors and operators
        AudioHandler(const AudioHandler&) = delete;
        AudioHandler& operator=(const AudioHandler&) = delete;
        AudioHandler(AudioHandler&&) = delete;
        AudioHandler& operator=(AudioHandler&&) = delete;
        
        private:
        SoundIo* soundIo = nullptr;
        SoundIoDevice* device = nullptr;
        SoundIoInStream* inStream = nullptr;
        boost::circular_buffer<float>* circularBuffer = nullptr;
        SoundIoFormat format = SoundIoFormatFloat32NE;
        int sampleRate = 0;
        const int circularBufferDuration = 30;
        
        void initializeSoundIoContext();
        void initializeDevice();
        void selectSampleRate();
        void checkSupportedFormat();
        void initializeInputStream();
        void openInputStream();
        void initializeCircularBuffer();
        
        static void readCallback(SoundIoInStream* inStream, int frameCountMin, int frameCountMax);
        
        public:
        auto getCircularBuffer() const
        { return this->circularBuffer; }
        
        auto getSampleRate() const
        { return this->sampleRate; }
        
        void startInputStream();
    };
    
    std::ostream& operator<<(std::ostream& out, const AudioHandler& audioHandler);
}

#endif //DICTA_AUDIOHANDLER_H
