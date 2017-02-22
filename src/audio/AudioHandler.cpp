/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 05/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../../include/audio/AudioHandler.h"
#include "../../include/audio/SoundIoException.h"

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
        initializeRingbuffer();
        startInputStream();
    }
    
    AudioHandler::~AudioHandler() noexcept
    {
        soundio_ring_buffer_destroy(this->ringBuffer);
        soundio_instream_destroy(this->inStream);
        soundio_device_unref(this->device);
        soundio_destroy(this->soundIo);
    }
    
    void AudioHandler::readCallback(SoundIoInStream* inStream, int frameCountMin, int frameCountMax)
    {
        SoundIoRingBuffer* ringBuffer = static_cast<SoundIoRingBuffer*>(inStream->userdata);
        SoundIoChannelArea* areas;
        int error;
        
        char* writePtr = soundio_ring_buffer_write_ptr(ringBuffer);
        
        int freeBytes = soundio_ring_buffer_free_count(ringBuffer);
        int freeCount = freeBytes / inStream->bytes_per_frame;
        
        if (freeCount < frameCountMin)
            throw SoundIoException("Ring buffer overflow.");
        
        int writeFrames = freeCount < frameCountMax ? freeCount : frameCountMax;
        int framesLeft = writeFrames;
        
        while (framesLeft > 0) {
            int frameCount = framesLeft;
            
            if (error = soundio_instream_begin_read(inStream, &areas, &frameCount))
                throw SoundIoException("Unable to begin reading", error);
            
            if (!frameCount) break;
            
            if (!areas) {
                std::memset(writePtr, 0, frameCount * inStream->bytes_per_frame);
            } else {
                for (int frame = 0; frame != frameCount; ++frame) {
                    for (int channel = 0; channel != inStream->layout.channel_count; ++channel) {
                        std::memcpy(writePtr, areas[channel].ptr, inStream->bytes_per_sample);
                        areas[channel].ptr += areas[channel].step;
                        writePtr += inStream->bytes_per_sample;
                    }
                }
            }
            if (error = soundio_instream_end_read(inStream))
                throw SoundIoException("Unable to end reading", error);
            
            framesLeft -= frameCount;
        }
        soundio_ring_buffer_advance_write_ptr(ringBuffer, writeFrames * inStream->bytes_per_frame);
    }
    
    void AudioHandler::readRecordingBuffer(SoundIoRingBuffer* ringBuffer)
    {
        while (true) {
            int fillBytes = soundio_ring_buffer_fill_count(ringBuffer);
            float* readBuffer = reinterpret_cast<float*>(soundio_ring_buffer_read_ptr(ringBuffer));
            for (int samples = 0; samples != fillBytes; ++samples) {
                std::cout << (readBuffer[samples]) << "\n";
            }
            soundio_ring_buffer_advance_read_ptr(ringBuffer, fillBytes);
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
        std::vector<int> sampleRates = {48000, 44100, 96000, 24000, 0};
        
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
    
    void AudioHandler::initializeRingbuffer()
    {
        this->ringBuffer = soundio_ring_buffer_create(this->soundIo,
                                                      ringBufferDuration * this->inStream->sample_rate
                                                              * this->inStream->bytes_per_frame);
        if (!this->ringBuffer)
            throw SoundIoException::OutOfMemory("ring buffer");
        
        this->inStream->userdata = this->ringBuffer;
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

