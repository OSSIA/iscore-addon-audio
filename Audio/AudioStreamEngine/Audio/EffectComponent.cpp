#include "EffectComponent.hpp"
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ConstraintComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/ScenarioComponent.hpp>
#include <Audio/AudioStreamEngine/Scenario/LoopComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/SoundComponent.hpp>
#include <Audio/AudioStreamEngine/Audio/ReturnComponent.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
#include <Audio/AudioStreamEngine/Utility.hpp>

#include <Audio/EffectProcess/Effect/Faust/FaustEffectModel.hpp>
#include <Audio/EffectProcess/LocalTree/LocalTreeEffectProcessComponent.hpp>
#include <Audio/EffectProcess/LocalTree/LocalTreeFaustEffectComponent.hpp>

#include <Midi/MidiExecutor.hpp>
#include <Midi/MidiProcess.hpp>
namespace Audio
{
namespace AudioStreamEngine
{

EffectProcessComponent::EffectProcessComponent(
        Effect::ProcessModel& sound,
        DocumentPlugin& doc,
        const Id<iscore::Component>& id,
        QObject* parent_obj):
    ProcessComponent_T{sound, doc, id, "EffectComponent", parent_obj}
{

}

void EffectProcessComponent::makeStream(const Context& ctx)
{
    // For all "generative" streams in the parent constraint,
    // take their "send" streams, create returns, mix the returns,
    // put it on input of the effect, create a send, return the output
    auto parent_cst = safe_cast<Scenario::ConstraintModel*>(process().parent());

    // Get its audio component
    Constraint& cst_comp = iscore::component<Constraint>(parent_cst->components());

    // The constraint has the mix information, hence we request it to create
    // the mix.
    AudioStream sound = cst_comp.makeInputMix(this->process().id());

    if(!sound)
    {
        // No input -> maybe the effect will still be making some noise
        // or act as an instrument ?
        sound = MakeNullSound(LONG_MAX);
    }

    // Berk, moveme, refactorme, whatever
    Midi::Executor::ProcessExecutor* midi_source{};
    auto& procs = cst_comp.constraint().processes;
    for(auto& proc : procs)
    {
      if(auto ptr = dynamic_cast<Midi::ProcessModel*>(&proc))
      {
        auto comp = iscore::findComponent<Midi::Executor::Component>(ptr->components());
        if(comp)
        {
          midi_source = &comp->OSSIAProcess();
          break;
        }
      }
    }
    int i = 0;
    for(auto& fx : process().effects())
    {
        auto compiled_fx = fx.effect();
        if(compiled_fx)
        {
          sound = MakeEffectSound(sound, compiled_fx, 0, 0);
          if(sound)
          {
            if(i == 0 && midi_source)
            {
              SetLV2MidiSource(compiled_fx, midi_source);
            }
          }
          else
          {
            // TODO maybe mark it as not working or something...
          }
        }

        i++;
    }

    if(sound)
        m_stream = MakeSend(sound);
    else
    {
        qDebug() << "Effect component could not be created";
        m_stream = MakeNullSound(LONG_MAX);
    }
}

}
}
