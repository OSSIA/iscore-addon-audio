#include "TReturnAudioStream.hpp"
#include <iscore/tools/Todo.hpp>
TReturnAudioStream::TReturnAudioStream(TSendAudioStreamPtr as):
    fSend{as}
{
    assert(fSend.getPointer());
}

TReturnAudioStream::~TReturnAudioStream()
{

}

long TReturnAudioStream::Read(FLOAT_BUFFER buffer, long framesNum, long framePos)
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

void TReturnAudioStream::Reset()
{
    ISCORE_TODO;
}

long TReturnAudioStream::Length()
{
    return LONG_MAX;
}

long TReturnAudioStream::Channels()
{
    return fSend->Channels();
}

TAudioStreamPtr TReturnAudioStream::Copy()
{
    return new TReturnAudioStream{fSend};
}
