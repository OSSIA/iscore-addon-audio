#pragma once

#include <3rdparty/libaudiostream/src/TAudioStream.h>
#include <3rdparty/libaudiostream/src/renderer/TAudioRenderer.h>

#include <iostream>
#include <memory>
#include <cmath>

struct TBufferManager
{
        float** fInputBuffer{};
        float** fOutputBuffer{};

        long fInput{};
        long fOutput{};
        long fBufferSize{};
        long fSampleRate{};

        TBufferManager() = default;

        TBufferManager(
                long inChan,
                long outChan,
                long bufferSize,
                long sampleRate):
            fInput{inChan},
            fOutput{outChan},
            fInputBuffer{inChan > 0 ? new float*[inChan] : nullptr},
            fOutputBuffer{outChan > 0 ? new float*[outChan] : nullptr},
            fBufferSize{bufferSize},
            fSampleRate{sampleRate}
        {
            alloc_all();
        }

        TBufferManager& operator=(const TBufferManager& other) = delete;

        TBufferManager& operator=(TBufferManager&& other)
        {
            free_all();

            // Set input
            fInputBuffer = other.fInputBuffer;
            fOutputBuffer = other.fOutputBuffer;

            fInput = other.fInput;
            fOutput = other.fOutput;

            fBufferSize = other.fBufferSize;
            fSampleRate = other.fSampleRate;

            // Clear output
            other.fInputBuffer = nullptr;
            other.fOutputBuffer = nullptr;

            other.fInput = 0;
            other.fOutput = 0;

            other.fBufferSize = 0;
            other.fSampleRate = 0;

            return *this;
        }

        ~TBufferManager()
        {
        }

    private:
        void alloc(float** ptr, int n_chan)
        {
            if(n_chan > 0)
            {
                ptr[0] = new float[n_chan * fBufferSize];
                for(int i = 1; i < n_chan; i++)
                {
                    ptr[i] = ptr[0] + i * fBufferSize;
                }
            }
        }

        void alloc_all()
        {
            alloc(fInputBuffer, fInput);
            alloc(fOutputBuffer, fOutput);

            UAudioTools::ZeroFloatBlk(fOutputBuffer, fBufferSize, fOutput);
        }

        void free_all()
        {
            if(fInputBuffer)
            {
                delete [] fInputBuffer[0];
            }

            if(fOutputBuffer)
            {
                delete [] fOutputBuffer[0];
            }

            delete [] fInputBuffer;
            delete [] fOutputBuffer;
            fInputBuffer = nullptr;
            fOutputBuffer = nullptr;
        }
};

class TGroupRenderer :
        public TAudioRenderer
{
    private:
        TBufferManager fBuffers;
        RendererInfo fInfo;

        long OpenImp(
                long inputDevice,
                long outputDevice,
                long inChan,
                long outChan,
                long bufferSize,
                long sampleRate) override;

    public:
        TGroupRenderer();

        virtual ~TGroupRenderer();

        long Open(
                long inChan,
                long outChan,
                long bufferSize,
                long sampleRate) override;

        long Close() override;

        long Start() override;

        long Stop() override;

        long Pause() override;

        long Cont() override;

        void Process(int64_t frames);

        float ** GetOutputBuffer() const;

        void GetInfo(RendererInfoPtr info) override;
};

class TSinusAudioStream final : public TAudioStream
{
        long fDuration{};
        float fFreq{};
        int fCurI = 0;

    public:
        TSinusAudioStream(long duration, float freq) ;

        ~TSinusAudioStream() ;

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        void Reset() override;

        // Cut the beginning of the stream
        TAudioStreamPtr CutBegin(long frames) override;

        // Length in frames
        long Length() override;

        long Channels() override;

        TAudioStreamPtr Copy() override;
};


class TPlayerAudioStream final : public TAudioStream
{
        TGroupRenderer& fRenderer;

    public:
        TPlayerAudioStream(TGroupRenderer& renderer) ;

        ~TPlayerAudioStream() ;

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override;

        void Reset() override;

        // Cut the beginning of the stream
        TAudioStreamPtr CutBegin(long frames) override;

        // Length in frames
        long Length() override;

        long Channels() override;

        TAudioStreamPtr Copy() override;
};

/**
 * @brief The TSendAudioStream class
 *
 * Calls Process on the watched stream and make an inner copy.
 */
class TSendAudioStream final :
        public TAudioStream,
        public TUnaryAudioStream
{
        TBufferManager fBuffers;
        TSharedNonInterleavedAudioBuffer<float> fTempBuffer;
        int fCount = 0;
    public:
        TSendAudioStream(TAudioStreamPtr as):
            fBuffers{0, as->Channels(), TAudioGlobals::fBufferSize, TAudioGlobals::fSampleRate},
            fTempBuffer{fBuffers.fOutputBuffer, fBuffers.fBufferSize, fBuffers.fOutput}
        {
            fStream = as;
        }

        ~TSendAudioStream()
        {

        }

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override
        {
            // The process of the send should be transparent.
            // This way, the "send" can be re-used as a stream.
            // If there is nothing more to pull, the result is filled with silence.

            // Clear the local buffer
            UAudioTools::ZeroFloatBlk(fBuffers.fOutputBuffer, fBuffers.fBufferSize, fBuffers.fOutput);

            // Write the new data into the local buffer
            fStream->Read(&fTempBuffer, framesNum, framePos);

            // Mix the local buffer into the output buffer
            float** temp1 = (float**)alloca(buffer->GetChannels()*sizeof(float*));
            UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                             fBuffers.fOutputBuffer,
                                             framesNum,
                                             fBuffers.fOutput);

            fCount++;
            return framesNum;
        }

        float ** GetOutputBuffer() const
        {
            return fBuffers.fOutputBuffer;
        }

        int GetReadBufferCount() const
        {
            return fCount;
        }

        void Reset() override
        {
            fCount = 0;
        }

        // Length in frames
        long Length() override
        {
            return fStream->Length();
        }

        long Channels() override
        {
            return fBuffers.fOutput;
        }

        TAudioStreamPtr Copy() override
        {
            return new TSendAudioStream{fStream->Copy()};
        }
};
using TSendAudioStreamPtr = LA_SMARTP<TSendAudioStream>;

/**
 * @brief The TReturnAudioStream class
 *
 * Make a copy of the watched stream, and apply a copy on it
 */
class TReturnAudioStream final :
        public TAudioStream
{
        TSendAudioStreamPtr fSend;
        int fLastSendCount = -1;
        bool fStarted = false;
    public:
        TReturnAudioStream(TSendAudioStreamPtr as):
            fSend{as}
        {
            assert(fSend.getPointer());
        }

        ~TReturnAudioStream()
        {

        }

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override
        {
            if(framesNum == 0)
                return 0;

            auto sc = fSend->GetReadBufferCount();
            if(sc <= fLastSendCount)
            {
                // The return has already read its last buffer and was stopped;
                // hence we don't have anything to mix in.
                // Note : if we have some effects generating sound
                // we should instead play a silent buffer. But this applies everywhere.
                return 0;
            }
            else
            {
                fLastSendCount = sc;
            }

            // Write the current buffer to the output.
            const auto channels = buffer->GetChannels();
            const auto channel_bytes = channels * sizeof(float*);

            float** temp1 = (float**) alloca(channel_bytes);

            float ** in_buffer;
            if(framesNum < TAudioGlobals::fBufferSize)
            {
                if(!fStarted)
                {
                    std::cerr << (void*)this << " ==> 0: ";
                    // We haven't started streaming, hence we're mid-buffer.
                    fStarted = true;

                    in_buffer = (float**) alloca(channel_bytes);
                    for(int chan = 0; chan < buffer->GetChannels(); chan++)
                    {
                        in_buffer[chan] = fSend->GetOutputBuffer()[chan] + TAudioGlobals::fBufferSize - framesNum;
                    }
                }
                else
                {
                    std::cerr << (void*)this << " ==> 1: ";
                    // We already started streaming so this is the last buffer
                    // which begins at the frame 0.
                    in_buffer = fSend->GetOutputBuffer();
                }
            }
            else
            {
                //std::cerr << (void*)this << " ==> 2: ";
                in_buffer = fSend->GetOutputBuffer();
            }

            if(fSend->Channels() == buffer->GetChannels())
            {
                std::cerr << framePos <<  " " << framesNum << std::endl;
                UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                                 in_buffer,
                                                 framesNum, fSend->Channels());
            }
            else if(fSend->Channels() < buffer->GetChannels())
            {
                std::cerr << "Not enough channels\n";
                // TODO
                // Two possibilities for channel management :
                // - ableton-like : everything is stereo
                // - S1-like : can create any kind of channels
                // Here we could duplicate, or fill with silence.
            }
            else if(fSend->Channels() > buffer->GetChannels())
            {
                std::cerr << "Too much channels\n";
                // TODO
                // here we should cut the other channels ?
            }
            // Everything should be adapted to work correctly on the "end channels".

            return framesNum;
        }

        void Reset() override
        {

        }

        // Length in frames
        long Length() override
        {
            return LONG_MAX;
        }

        long Channels() override
        {
            return fSend->Channels();
        }

        TAudioStreamPtr Copy() override
        {
            return new TReturnAudioStream{fSend};
        }
};


/**
 * @brief The TChannelAudioStream class
 *
 * Mixer channel as an audiostream ; for now only volume
 */
class TChannelAudioStream final :
        public TAudioStream,
        public TUnaryAudioStream
{
        double const * const fVolume{};
        TLocalNonInterleavedAudioBuffer<float> fBuffer;
    public:
        TChannelAudioStream(
                TAudioStreamPtr as,
                double const * volume):
            fVolume{volume},
            fBuffer{TAudioGlobals::fBufferSize, as->Channels()}
        {
            fStream = as;
        }

        ~TChannelAudioStream()
        {

        }

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override
        {
            float** in = (float**)alloca(fBuffer.GetChannels()*sizeof(float*));
            float** out = (float**)alloca(buffer->GetChannels()*sizeof(float*));
            fBuffer.GetFrame(0, in);
            buffer->GetFrame(framePos, out);

            UAudioTools::ZeroFloatBlk(in, fBuffer.GetSize(), fBuffer.GetChannels());

            long res = fStream->Read(&fBuffer, framesNum, 0);

            auto& vol = *fVolume;
            for(int chan = 0; chan < fBuffer.GetChannels(); chan++)
            {
                for(int j = 0; j < res; j++)
                {
                    out[chan][j] = in[chan][j] * vol;
                }
            }

            return res;
        }

        void Reset() override
        {
        }

        // Length in frames
        long Length() override
        {
            return fStream->Length();
        }

        long Channels() override
        {
            return fStream->Channels();
        }

        TAudioStreamPtr Copy() override
        {
            return new TChannelAudioStream{fStream->Copy(), fVolume};
        }
};
using TChannelAudioStreamPtr = LA_SMARTP<TChannelAudioStream>;
