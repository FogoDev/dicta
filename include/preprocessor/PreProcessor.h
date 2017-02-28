/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 06/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_PREPROCESSOR_H
#define DICTA_PREPROCESSOR_H

#include <queue>
#include <boost/circular_buffer.hpp>
#include <cmath>
#include <iostream>
#include "Frame.hpp"
#include "DFTHandler.h"
#include "MFCC.hpp"

namespace Dicta
{
    class PreProcessor
    {
        private:
        std::size_t sampleRate;
        std::size_t samplesPerFrame;
        std::queue<Frame<float>> processedFrames;
        DFTHandler dftHandler;
        MFCC<float> mfcc;
        
        static constexpr std::size_t filterBankCount = 26;
        static constexpr std::size_t lowerFrequency = 0;
        static constexpr float dftFloat = float{};
        static constexpr double dftDouble = double{};
        static constexpr double pi = std::atan(1) * 4;
        
        public:
        PreProcessor(std::size_t sampleRate) :
                sampleRate(sampleRate),
                samplesPerFrame(getNextPowerOf2(sampleRate / 100)), // To get 10ms sized processedFrames
                dftHandler(samplesPerFrame, filterBankCount, dftFloat),
                mfcc(sampleRate, filterBankCount, samplesPerFrame, lowerFrequency, calculateHigherFrequency(sampleRate))
        {}
        
        auto getSamplesPerFrame() const
        { return this->samplesPerFrame; }
        
        void addFrame(Frame<float> frame)
        { this->processedFrames.push(std::move(frame)); }
        
        float hannWindowFunction(std::size_t index)
        { return 0.5 * (1 - std::cos((2 * pi * index) / this->samplesPerFrame)); }
        
        DFTHandler& getDFTHandler()
        { return this->dftHandler; }
        
        MFCC<float>& getMFCC()
        { return this->mfcc; }
        
        std::size_t calculateHigherFrequency(std::size_t sampleRate)
        { return sampleRate / 2;}
        
        static void readFrameAndWindowRecordingBuffer(boost::circular_buffer<float>* circularBuffer, PreProcessor* preProcessor);
        
        void report();
        
        private:
        std::size_t getNextPowerOf2(std::size_t num)
        {
            std::size_t base2 = 1;
            while(base2 < num)
                base2 <<= 1;
            return base2;
        }
    };
}
#endif //DICTA_PREPROCESSOR_H
