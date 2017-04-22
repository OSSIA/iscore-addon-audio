#include "TGroupAudioStream.hpp"

#include "TSharedBuffers.h"
#include <iostream>
#include <cmath>
#include <QDebug>
#include <3rdparty/libaudiostream/src/UAudioTools.h>



//////////////////////////
//////////////////////////





//////////////////////////
//////////////////////////


#include <iscore_plugin_audio_export.h>
/*
extern "C" {
struct TLoopCommand : public TControlCommand
{
        AudioStream fStream;
        SymbolicDate fStopDate;

        TLoopCommand(AudioStream s, SymbolicDate start, SymbolicDate stop):
            TControlCommand{start},
            fStream{s},
            fStopDate{stop}
        {

        }

        bool Execute(
                TNonInterleavedAudioBuffer<float>* buffer,
                std::map<SymbolicDate, audio_frame_t>& date_map,
                audio_frame_t cur_frame,
                long frames)
        {
            auto loop_time = fStopDate->get_date();
            auto loop_dur = fStopDate->get_date() - fStartDate->get_date();
            qDebug() << loop_time << loop_dur << InBuffer(loop_time, cur_frame, frames);
            if (InBuffer(loop_time, cur_frame, frames))
            {
                fStream->Reset();
                fStream->SetPos(0);

                fStartDate->setDate(loop_time + 1);
                fStopDate->setDate(loop_time + 1 + loop_dur);
            }

            // We always return true to prevent the command from being removed
            return true;
        }
};

ISCORE_PLUGIN_AUDIO_EXPORT void MakeScenarioTimeNodeCommand(
        AudioPlayer player,
        Audio::AudioStreamEngine::TimeNodeComponent& t
        )
{
    auto cmd = new TTimeNodeControlCommand{t};
    player.fMixer->AddControlCommand(cmd);
}
}
*/

