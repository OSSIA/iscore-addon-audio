#include "EffectComponent.hpp"
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
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
#if defined(LILV_SHARED)
#include <Audio/EffectProcess/LV2/LV2EffectModel.hpp>
#include <Midi/MidiExecutor.hpp>
#include <Midi/MidiProcess.hpp>
#endif
namespace Audio
{
namespace AudioStreamEngine
{

EffectProcessComponent::EffectProcessComponent(
        Effect::ProcessModel& sound,
        DocumentPlugin& doc,
        const Id<score::Component>& id,
        QObject* parent_obj):
    ProcessComponent_T{sound, doc, id, "EffectComponent", parent_obj}
{

}

void EffectProcessComponent::makeStream(const Context& ctx)
{
    // For all "generative" streams in the parent interval,
    // take their "send" streams, create returns, mix the returns,
    // put it on input of the effect, create a send, return the output
    auto parent_cst = safe_cast<Scenario::IntervalModel*>(process().parent());

    // Get its audio component
    Interval& cst_comp = score::component<Interval>(parent_cst->components());

    // The interval has the mix information, hence we request it to create
    // the mix.
    AudioStream sound = cst_comp.makeInputMix(this->process().id());

    if(!sound)
    {
        // No input -> maybe the effect will still be making some noise
        // or act as an instrument ?
        sound = MakeNullSound(LONG_MAX);
    }

#if defined(LILV_SHARED)
    // Berk, moveme, refactorme, whatever
    Midi::Executor::ProcessExecutor* midi_source{};
    auto& procs = cst_comp.interval().processes;
    for(auto& proc : procs)
    {
      if(auto ptr = dynamic_cast<Midi::ProcessModel*>(&proc))
      {
        auto comp = score::findComponent<Midi::Executor::Component>(ptr->components());
        if(comp)
        {
          //midi_source = &comp->OSSIAProcess();
          break;
        }
      }
    }
#endif
    int i = 0;
    for(auto& fx : process().effects())
    {
        auto compiled_fx = fx.effect();
        if(compiled_fx)
        {
          sound = MakeEffectSound(sound, compiled_fx, 0, 0);
          if(sound)
          {
#if defined(LILV_SHARED)
            if(dynamic_cast<Effect::LV2EffectModel*>(&fx))
            {
              if(i == 0 && midi_source)
              {
                SetLV2MidiSource(compiled_fx, midi_source);
              }
            }
#endif
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

void EffectProcessComponent::stop()
{
#if defined(LILV_SHARED)
  for(const auto& fx : process().effects())
  {
      auto compiled_fx = fx.effect();
      if(dynamic_cast<const Effect::LV2EffectModel*>(&fx))
        LV2MidiNotesOff(compiled_fx);
  }
#endif
}

}
}
