#include "SendComponent.hpp"
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
#include <Audio/AudioStreamEngine/Utility.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>

namespace Audio
{
namespace AudioStreamEngine
{

/* TODO use me
template <class T>
class TCircularNonInterleavedAudioBuffer
{
        TLocalNonInterleavedAudioBuffer<float>  fImpl;
        int64_t fWritePos = 0;
        int64_t fReadPos = 0;
        const int64_t fFrames, fTotal;
    public:
        TCircularNonInterleavedAudioBuffer(long frames, long channels, bool touch = false):
            fImpl{frames * 2, channels, touch},
            fWritePos{frames + 1},
            fFrames{frames},
            fTotal{frames + TAudioGlobals::fBufferSize}
        {
            UAudioTools::ZeroFloatBlk(fImpl.GetBuffer(), 2 * frames, channels);
        }

        void addSamples(float** source, long frames)
        {
            const long next = fWritePos + frames;
            if(next < fTotal)
            {
                for(long c = 0; c < fImpl.GetChannels(); c++)
                {
                    auto src_chan = source[c];
                    auto dst_chan = fImpl.GetBuffer()[c];
                    std::copy_n(src_chan, frames, dst_chan + fWritePos);
                }
                fWritePos = next;
            }
            else
            {
                const long toCopy = fTotal - fWritePos;
                const long remaining = next - fTotal;

                for(long c = 0; c < fImpl.GetChannels(); c++)
                {
                    auto src_chan = source[c];
                    auto dst_chan = fImpl.GetBuffer()[c];
                    std::copy_n(src_chan, toCopy, dst_chan + fWritePos);
                    std::copy_n(src_chan + toCopy, remaining, dst_chan);
                }

                fWritePos = remaining;
            }
        }

        void mixSamples(float** dest, long frames)
        {
            const long next = fReadPos + frames;
            if(next < fTotal)
            {
                for(long c = 0; c < fImpl.GetChannels(); c++)
                {
                    auto src_chan = fImpl.GetBuffer()[c];
                    auto dst_chan = dest[c];
                    std::copy_n(src_chan + fReadPos, frames, dst_chan);
                }
                fReadPos = next;
            }
            else
            {
                const long toCopy = fTotal - fReadPos;
                const long remaining = next - fTotal;

                for(long c = 0; c < fImpl.GetChannels(); c++)
                {
                    auto src_chan = fImpl.GetBuffer()[c];
                    auto dst_chan = dest[c];
                    std::copy_n(src_chan + fReadPos, toCopy, dst_chan);
                    std::copy_n(src_chan, remaining, dst_chan + toCopy);
                }

                fReadPos = remaining;

            }

        }
};
*/
SendComponent::SendComponent(
        Send::ProcessModel& sound,
        DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        QObject* parent_obj):
    ProcessComponent_T{sound, doc, id, "SendComponent", parent_obj}
{

}

void SendComponent::makeStream(const Context& ctx)
{
    // For all "generative" streams in the parent constraint,
    // take their "send" streams, create returns, mix the returns,
    // put it on input of the effect, create a send, return the output
    auto parent_cst = safe_cast<Scenario::ConstraintModel*>(process().parent());
    Constraint& cst_comp = iscore::component<Constraint>(parent_cst->components);

    m_stream = MakeSend(cst_comp.makeInputMix(this->process().id()));
}

}
}
