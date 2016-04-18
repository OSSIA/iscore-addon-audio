#pragma once

#include <3rdparty/libaudiostream/src/TAudioStream.h>
#include <3rdparty/libaudiostream/src/renderer/TAudioRenderer.h>

#include <memory>
class TGroupRenderer : public TAudioRenderer
{
    private:
        float** fInputBuffer{};
        float** fOutputBuffer{};

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
