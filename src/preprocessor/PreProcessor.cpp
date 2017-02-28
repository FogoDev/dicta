/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 06/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../../include/preprocessor/PreProcessor.h"

namespace Dicta
{
    void PreProcessor::readFrameAndWindowRecordingBuffer(boost::circular_buffer<float>* circularBuffer, PreProcessor* preProcessor)
    {
        auto samplesPerFrame = preProcessor->getSamplesPerFrame();
        auto frameMidPoint = samplesPerFrame / 2;
        float currentSample = 0;
        auto dftHandler = preProcessor->getDFTHandler();
        auto mfcc = preProcessor->getMFCC();
        Frame<float> firstFrame;
        Frame<float> secondFrame;
        Frame<float> thirdFrameFirstHalf;
        Frame<float> thirdFrameComplete;
        
        while (true) {
            firstFrame = Frame<float>(samplesPerFrame);
            secondFrame = Frame<float>(samplesPerFrame);
            thirdFrameFirstHalf = Frame<float>(samplesPerFrame);
            
            for (auto sample = 0; sample != samplesPerFrame + frameMidPoint; ++sample) {
                if (!circularBuffer->empty()) {
                    currentSample = circularBuffer->front();
                    if (currentSample < -1) currentSample = -1;
                    if (currentSample > 1) currentSample = 1;
                    
                    circularBuffer->pop_front();
                    
                    if (sample < frameMidPoint) {
                        firstFrame.push(currentSample * preProcessor->hannWindowFunction(firstFrame.size()));
                        
                        if (!thirdFrameComplete.empty())
                            thirdFrameComplete.push(currentSample * preProcessor->hannWindowFunction(thirdFrameComplete.size()));
                        
                    } else if (sample >= frameMidPoint && sample < samplesPerFrame) {
                        if (!thirdFrameComplete.empty()) {
                            preProcessor->addFrame(dftHandler.processDCT(mfcc.computeMFCC(dftHandler.processFFT(thirdFrameComplete))));
                        }
                        
                        firstFrame.push(currentSample * preProcessor->hannWindowFunction(firstFrame.size()));
                        secondFrame.push(currentSample * preProcessor->hannWindowFunction(secondFrame.size()));
                        
                    } else {
                        secondFrame.push(currentSample * preProcessor->hannWindowFunction(secondFrame.size()));
                        thirdFrameFirstHalf.push(currentSample * preProcessor->hannWindowFunction(thirdFrameFirstHalf.size()));
                    }
                } else
                    --sample;
            }
            thirdFrameComplete = std::move(thirdFrameFirstHalf);
            preProcessor->addFrame(dftHandler.processDCT(mfcc.computeMFCC(dftHandler.processFFT(firstFrame))));
            preProcessor->addFrame(dftHandler.processDCT(mfcc.computeMFCC(dftHandler.processFFT(secondFrame))));
        }
    }
    
    void PreProcessor::report() // Execute on terminal: graph -T png -C --bitmap-size 4000x4000 < A.txt > plot.png
    {
        while (true) {
            if (!this->processedFrames.empty()) {
                auto& frame = this->processedFrames.front();
                for (int pos = 0; pos != frame.size(); ++pos)
                    std::cout << pos << " " << frame[pos] << "\n";
                
                std::cout << "\n";
                this->processedFrames.pop();
            }
        }
    }
}