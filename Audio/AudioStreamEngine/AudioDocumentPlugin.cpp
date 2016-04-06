#include "AudioDocumentPlugin.hpp"
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <3rdparty/libaudiostream/src/LibAudioStreamMC++.h>

namespace Audio
{

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
AudioStream CreateAudioStream(const Scenario::ConstraintModel& scenario)
{
    // First look for all the "contents" process :
    // - Scenario
    // - Loop

    // Then look for all the "effects" process and apply them

    // Then look for the "Mix" process and do the mix

    return {};
}

AudioStream CreateAudioStream(const Scenario::ScenarioModel& scenario)
{
    // First create a stream for all the constraints

    // Then make a graph from the scenario :
    // go from the first timenode
    return {};
}

void AudioDocumentPlugin::play()
{

}

void AudioDocumentPlugin::stop()
{

}
}
