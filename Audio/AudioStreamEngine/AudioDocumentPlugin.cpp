#include "AudioDocumentPlugin.hpp"
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <map>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <core/document/DocumentModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

AudioStream CreateAudioStream(const Loop::ProcessModel& loop);
AudioStream CreateAudioStream(const Loop::ProcessModel& loop)
{
    // Apply to the constraint
    // If trigger at the end, see:
    /*
    date = GetCurDate();
    symb1 = GenSymbolicDate(gAudioPlayer);

    s1 = MakeLoopSound(MakeRegionSound(FILENAME1, 5*SR, 100*SR), INFINI);
    s2 = MakeRegionSound(FILENAME2, 5*SR, 10*SR);

    StartSound(gAudioPlayer, s1, GenRealDate(gAudioPlayer, date));
    StopSound(gAudioPlayer, s1, symb1);
    StartSound(gAudioPlayer, s2, symb1);

    // Instancie la date symbolique à la date courante "capturée"
    set_symbolic_date(symb1);
    */


    return {};
}

void DocumentPlugin::play()
{
    // First find the root constraint
    auto doc = dynamic_cast<Scenario::ScenarioDocumentModel*>(&m_ctx.doc.document.model().modelDelegate());
    if(!doc)
        return;

    // Initialize libaudiostream structures
    if(m_ctx.player)
        CloseAudioPlayer(m_ctx.player);

    GetDeviceInfo(kJackRenderer, 0, &m_ctx.device_info);
    auto& dev = m_ctx.device_info;
    qDebug() << dev.fName
             << dev.fMaxInputChannels
             << dev.fMaxOutputChannels
             << dev.fDefaultBufferSize
             << dev.fDefaultSampleRate;

    m_ctx.player = OpenAudioPlayer(0, 2, 44100, 512, 65536*4, 44100*60*20, kJackRenderer, 1);
    m_ctx.renderer = GetAudioPlayerRenderer(m_ctx.player);

    GetAudioRendererInfo(m_ctx.renderer, &m_ctx.renderer_info);

    // Create our tree
    // TODO make id from components !!!!
    auto comp = new ConstraintComponent(
                Id<iscore::Component>{1},
                doc->baseConstraint(),
                *this,
                m_ctx.doc,
                this);
    doc->baseConstraint().components.add(comp);

    auto stream = comp->makeStream(
                m_ctx,
                GenRealDate(m_ctx.player, 0),
                GenSymbolicDate(m_ctx.player));

    StartSound(m_ctx.player, stream, GenRealDate(m_ctx.player, 0));

    // Play
    StartAudioPlayer(m_ctx.player);
}

void DocumentPlugin::stop()
{
    if(m_ctx.player)
    {
        StopAudioPlayer(m_ctx.player);
        CloseAudioPlayer(m_ctx.player);
    }
    m_ctx.player = nullptr;
    m_ctx.renderer = nullptr;
    m_ctx.device_info = DeviceInfo{};
    m_ctx.renderer_info = RendererInfo{};

}
}
}
