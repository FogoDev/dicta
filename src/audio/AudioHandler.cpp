/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 05/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../../include/audio/AudioHandler.h"

namespace Dicta
{
    AudioHandler::AudioHandler()
    {
        initializeSoundIoContext();
        initializeDevice();
        selectSampleRate();
        checkSupportedFormat();
        initializeInputStream();
        openInputStream();
        initializeCircularBuffer();
    }
    
    AudioHandler::~AudioHandler() noexcept
    {
        delete this->circularBuffer;
        soundio_instream_destroy(this->inStream);
        soundio_device_unref(this->device);
        soundio_destroy(this->soundIo);
    }
    
    void AudioHandler::readCallback(SoundIoInStream* inStream, int frameCountMin, int frameCountMax)
    {
        auto circularBuffer = static_cast<boost::circular_buffer<float>*>(inStream->userdata);
        SoundIoChannelArea* areas;
        int error;
        float channelsSum = 0;
        int channelCount = inStream->layout.channel_count;
        
        int writeFrames = frameCountMax;
        int framesLeft = writeFrames;
        
        while (framesLeft > 0) {
            int frameCount = framesLeft;
            
            if (error = soundio_instream_begin_read(inStream, &areas, &frameCount))
                throw SoundIoException("Unable to begin reading", error);
            
            if (!frameCount) break;
            
            if (!areas) {
                // Due to an overflow there is a hole. Fill the circular buffer with silence for the fftSize of the hole.
                for (int frame = 0; frame != frameCount; ++frame)
                    circularBuffer->push_back(0);
            } else {
                for (int frame = 0; frame != frameCount; ++frame) {
                    for (int channel = 0; channel != channelCount; ++channel) {
                        channelsSum += *(reinterpret_cast<float*>(areas[channel].ptr + frame * areas[channel].step));
                    }
                    circularBuffer->push_back(channelsSum / channelCount);
                    channelsSum = 0;
                }
            }
            if (error = soundio_instream_end_read(inStream))
                throw SoundIoException("Unable to end reading", error);
            
            framesLeft -= frameCount;
        }
    }
    
    void AudioHandler::initializeSoundIoContext()
    {
        this->soundIo = soundio_create();
        
        if (!this->soundIo)
            throw SoundIoException::OutOfMemory("soundIo context");
        
        if (int error = soundio_connect(this->soundIo))
            throw SoundIoException("Unable to connect to audio backend", error);
        
        soundio_flush_events(this->soundIo);
    }
    
    void AudioHandler::initializeDevice()
    {
        this->device = soundio_get_input_device(this->soundIo, soundio_default_input_device_index(this->soundIo));
        
        if (!this->device)
            throw SoundIoException("No input device available");
        
        if (int error = this->device->probe_error)
            throw SoundIoException("Unable to probe device", error);
        
        soundio_device_sort_channel_layouts(this->device);
    }
    
    void AudioHandler::selectSampleRate()
    {
        std::array<int, 5> sampleRates = {48000, 44100, 96000, 24000, 0};
        
        for (const auto& sampleRate : sampleRates)
            if (soundio_device_supports_sample_rate(this->device, sampleRate)) {
                this->sampleRate = sampleRate;
                break;
            }
        
        if (!this->sampleRate)
            throw SoundIoException("Device doesn't support the sample rate used");
    }
    
    void AudioHandler::checkSupportedFormat()
    {
        if (!soundio_device_supports_format(this->device, this->format))
            throw SoundIoException("Device doesn't support the format used");
    }
    
    void AudioHandler::initializeInputStream()
    {
        this->inStream = soundio_instream_create(this->device);
        if (!this->inStream)
            throw SoundIoException::OutOfMemory("input stream");
        
        this->inStream->format = this->format;
        this->inStream->sample_rate = this->sampleRate;
        this->inStream->read_callback = AudioHandler::readCallback;
    }
    
    void AudioHandler::openInputStream()
    {
        if (int error = soundio_instream_open(this->inStream))
            throw SoundIoException("Unable to open input stream", error);
    }
    
    void AudioHandler::initializeCircularBuffer()
    {
        this->circularBuffer =
                new boost::circular_buffer<float>(this->circularBufferDuration * this->inStream->sample_rate);
        
        this->inStream->userdata = this->circularBuffer;
    }
    
    void AudioHandler::startInputStream()
    {
        if (int error = soundio_instream_start(this->inStream))
            throw SoundIoException("Unable to start input stream", error);
    }
    
    std::ostream& operator<<(std::ostream& out, const AudioHandler& audioHandler)
    {
        out << "Device: "
            << audioHandler.device->name
            << "\nSample Rate: "
            << audioHandler.sampleRate
            << "Hz\nLayout: "
            << audioHandler.inStream->layout.name
            << "\nFormat: "
            << soundio_format_string(audioHandler.format)
            << std::endl;
        
        return out;
    }
}

