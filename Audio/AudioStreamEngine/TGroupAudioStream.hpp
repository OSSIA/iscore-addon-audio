#pragma once

#include <3rdparty/libaudiostream/src/TAudioStream.h>
#include <3rdparty/libaudiostream/src/renderer/TAudioRenderer.h>

#include <memory>

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
            fBufferSize{bufferSize},
            fSampleRate{sampleRate},
            fInputBuffer{new float*[inChan]},
            fOutputBuffer{new float*[outChan]}
        {
            for (int i = 0; i < inChan; i++) {
                fInputBuffer[i] = new float[bufferSize];
            }
            for (int i = 0; i < outChan; i++) {
                fOutputBuffer[i] = new float[bufferSize];
            }

            UAudioTools::ZeroFloatBlk(fOutputBuffer, fBufferSize, fOutput);
        }

        ~TBufferManager()
        {
            if(fInputBuffer)
            {
                for (int i = 0; i < fInput; i++) {
                    delete [] fInputBuffer[i];
                }
            }

            if(fOutputBuffer)
            {
                for (int i = 0; i < fOutput; i++) {
                    delete [] fOutputBuffer[i];
                }
            }

            delete [] fInputBuffer;
            delete [] fOutputBuffer;
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

        void Process();

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

// We need both Bus Send and Bus Return
// Bus send will pull the data from the underlying process
// Bus return will copy this data and use it..
class TBusAudioStream final :
        public TAudioStream
{
        struct impl {
                int64_t fReadCount = 0; // Number of buffer reads for the current callback
                TGroupRenderer fRenderer;
                TAudioStream* fStream{};
                TBusAudioStream* fRootBus{};
        };
        std::shared_ptr<impl> fImpl;

    public:
        TBusAudioStream(TAudioStreamPtr as)
        {

        }

        TBusAudioStream(const TBusAudioStream& as) = default;

        ~TBusAudioStream()
        {

        }

        long Read(FLOAT_BUFFER buffer, long framesNum, long framePos) override
        {
            impl& shared = *fImpl;
            auto use_count = fImpl.use_count();

            // Process the next buffer if all the users
            // have read once.
            if(shared.fReadCount >= use_count)
            {
                shared.fReadCount = 0;
                shared.fRenderer.Process();
            }

            // Write the current buffer to the output.
            float** temp1 = (float**)alloca(buffer->GetChannels()*sizeof(float*));

            auto out_buffer = shared.fRenderer.GetOutputBuffer();

            UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                             out_buffer,
                                             framesNum, TAudioGlobals::fInput);

            // Go to the next frame.
            shared.fReadCount++;

            return framesNum;
        }

        void Reset() override
        {

        }

        // Cut the beginning of the stream
        TAudioStreamPtr CutBegin(long frames) override
        {
            return {};
        }

        // Length in frames
        long Length() override
        {
            return 0;
        }

        long Channels() override
        {
            return 0;
        }

        TAudioStreamPtr Copy() override
        {
            return {};
        }
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
    public:
        TSendAudioStream(TAudioStreamPtr as):
            fBuffers{0, as->Channels(), TAudioGlobals::fBufferSize, TAudioGlobals::fSampleRate},
            fTempBuffer{fBuffers.fOutputBuffer, fBuffers.fBufferSize, fBuffers.fSampleRate}
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

            return framesNum;
        }

        float ** GetOutputBuffer() const
        {
            return fBuffers.fOutputBuffer;
        }

        void Reset() override
        {

        }

        // Cut the beginning of the stream
        TAudioStreamPtr CutBegin(long frames) override
        {
            return {};
        }

        // Length in frames
        long Length() override
        {
            return LONG_MAX;
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

#include <iostream>
/**
 * @brief The TReturnAudioStream class
 *
 * Make a copy of the watched stream, and apply a copy on it
 */
class TReturnAudioStream final :
        public TAudioStream
{
        TSendAudioStreamPtr fSend;
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
            // Write the current buffer to the output.
            float** temp1 = (float**)alloca(buffer->GetChannels()*sizeof(float*));

            if(fSend->Channels() == buffer->GetChannels())
            {
                std::cerr << framePos << std::endl;
                UAudioTools::MixFrameToFrameBlk1(buffer->GetFrame(framePos, temp1),
                                                 fSend->GetOutputBuffer(),
                                                 framesNum, fSend->Channels());
            }
            else if(fSend->Channels() < buffer->GetChannels())
            {
                // TODO
            }
            else if(fSend->Channels() > buffer->GetChannels())
            {
                // TODO
            }

            return framesNum;
        }

        void Reset() override
        {

        }

        // Cut the beginning of the stream
        TAudioStreamPtr CutBegin(long frames) override
        {
            return {};
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

