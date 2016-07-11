#include "TGroupRenderer.hpp"
#include <TBufferedAudioStream.h>
#include <TSharedBuffers.h>

/**
 * @brief The SharedBufferLocker class
 *
 * Will save and restore the state of shared buffers
 */
class TSharedBufferLocker
{
        float** fPrevBuffer{TSharedBuffers::GetOutBuffer()};

    public:
        TSharedBufferLocker(float** out)
        {
            // TODO maybe we should also save the number of channels ... ?
            TSharedBuffers::SetOutBuffer(out);
        }

        ~TSharedBufferLocker()
        {
            TSharedBuffers::SetOutBuffer(fPrevBuffer);
        }
};

long TGroupRenderer::OpenImp(
        long inputDevice,
        long outputDevice,
        long inChan,
        long outChan,
        long bufferSize,
        long sampleRate)
{
    assert(false);
    // Shouldn't be used.
    return 0;
}

TGroupRenderer::TGroupRenderer():
    TAudioRenderer{},
    fInfo{{}, {}, {}, {}, {}, {}, {}, {}, {}, {}}
{
}

TGroupRenderer::~TGroupRenderer()
{
}

long TGroupRenderer::Open(long inChan, long outChan, long bufferSize, long sampleRate)
{
    TAudioRenderer::Open(inChan, outChan, bufferSize, sampleRate);
    fBuffers = TBufferManager(inChan, outChan, bufferSize, sampleRate);
    return 0;
}

long TGroupRenderer::Close()
{
    return 0;
}

long TGroupRenderer::Start()
{
    return false;
}

long TGroupRenderer::Stop()
{
    fInfo.fCurFrame = 0;
    fInfo.fCurUsec = 0;
    return 0;
}

long TGroupRenderer::Pause()
{
    // TODO
    return Stop();
}

long TGroupRenderer::Cont()
{
    // TODO
    return Start();
}

void TGroupRenderer::Process(int64_t frames)
{
    TSharedBufferLocker shared_buffers{fBuffers.fOutputBuffer};

    // Clear output buffers
    UAudioTools::ZeroFloatBlk(fBuffers.fOutputBuffer, frames, fOutput); // TODO set correct channel count
    // Client callback are supposed to *mix* their result in outputs
    auto iter = fClientList.cbegin();
    while (iter != fClientList.cend())
    {
        if (auto client = iter->fRTClient)
        {
            client->AudioCallback(nullptr, fBuffers.fOutputBuffer, frames);
            ++iter;
        }
        else
        {
            iter = fClientList.erase(iter);
        }
    }
    fInfo.fCurFrame += frames;
    auto cur_usec_double = ConvertSample2Usec(fInfo.fCurFrame);
    if(cur_usec_double <= (double) LONG_MAX)
        fInfo.fCurUsec = cur_usec_double;
    else
        fInfo.fCurUsec = LONG_MAX;
}

float** TGroupRenderer::GetOutputBuffer() const
{ return fBuffers.fOutputBuffer; }

void TGroupRenderer::GetInfo(RendererInfoPtr info)
{
    (*info) = fInfo;
}
