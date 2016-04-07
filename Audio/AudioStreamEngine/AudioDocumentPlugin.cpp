#include "AudioDocumentPlugin.hpp"
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <map>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <3rdparty/libaudiostream/src/LibAudioStreamMC++.h>

namespace Audio
{
namespace AudioStreamEngine
{

AudioStream CreateAudioStream(const Loop::ProcessModel& loop);
AudioStream CreateAudioStream(const Scenario::ConstraintModel& cst, SymbolicDate start, SymbolicDate end);
AudioStream CreateAudioStream(const Scenario::ScenarioModel& loop);
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
AudioStream CreateAudioStream(const Scenario::ConstraintModel& cst, SymbolicDate start, SymbolicDate end)
{
    using StreamPair = std::pair<Process::ProcessModel&, AudioStream>;

    if(cst.processes.empty())
    {

    }
    else if(cst.processes.size() == 1)
    {
        auto& proc = *cst.processes.begin();
        // if(auto mc_proc = dynamic_cast<MixerProcess*>(&proc)) { ... }
    }
    else
    {
        std::vector<StreamPair> inputStreams;
        std::vector<StreamPair> fxStreams;
        // MixProcess* mix_proc{};
        for(auto& proc : cst.processes)
        {
            if(auto scenar = dynamic_cast<Scenario::ScenarioModel*>(&proc))
            {
                inputStreams.emplace_back(*scenar, CreateAudioStream(*scenar));
            }
            else if(auto loop = dynamic_cast<Loop::ProcessModel*>(&proc))
            {
                inputStreams.emplace_back(*loop, CreateAudioStream(*loop));
            }
            /*
            else if(auto fx = dynamic_cast<Audio::EffectsProcess*>(&proc))
            {
                inputStreams.emplace_back(*fx, CreateAudioStream(*fx));
            }
            else if(auto mix = dynamic_cast<Audio::MixProcess*>(&proc))
            {
                mix_proc = mix;
            }
            */
        }
    }
    // Look for all the "contents" process :
    // - Scenario
    // - Loop
    // Look for all the "effects" process and apply them

    // Then look for the "Mix" process and do the mix

    return {};
}
AudioStream CreateAudioStream(const Scenario::ScenarioModel& scenario)
{
    std::map<Id<Scenario::TimeNodeModel>, SymbolicDate> synchros;
    std::map<Id<Scenario::ConstraintModel>, AudioStream> csts;
    // First generate a symbolic date for each of the timenode (fixed if there is no trigger ?)
    for(auto& tn : scenario.timeNodes)
    {
        // TODO right now audioplayer is not used in GenSymbolicDate but this may be dangerous...
        synchros.insert(std::make_pair(tn.id(), GenSymbolicDate(AudioPlayerPtr{})));
    }

    // Then create a stream for all the constraints
    // and set the start / end of each parent stream of the constraint
    // to the symbolic date of the trigger
    for(auto& cst : scenario.constraints)
    {
        csts.insert(
            std::make_pair(
                cst.id(),
                CreateAudioStream(
                            cst,
                            synchros.at(startEvent(cst, scenario).timeNode()),
                            synchros.at(endEvent(cst, scenario).timeNode())
                )
            )
        );
    }


    return {};
}

void DocumentPlugin::play()
{

}

void DocumentPlugin::stop()
{

}
}
}
