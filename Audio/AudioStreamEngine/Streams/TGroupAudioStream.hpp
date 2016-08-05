#pragma once

#include <3rdparty/libaudiostream/src/TAudioStream.h>
#include <3rdparty/libaudiostream/src/renderer/TAudioRenderer.h>
#include "TExpAudioMixer.h"
#include "TEffectAudioStream.h"

#include <set>
#include <iostream>
#include <memory>
#include <cmath>

#include <iscore/tools/Todo.hpp>




//#include <Audio/AudioStreamEngine/Scenario/TimeNodeComponent.hpp>


/*
class LV2AudioEffect : public TAudioEffectInterface
{
    public:
        LV2AudioEffect()
        {

        }

    private:

        ossia::time_constraint& m_root;
        void Process(float** input, float** output, long framesNum) override
        {
            m_root.tick();
            for(int i = 0; i < Channels(); i++)
            {
                std::copy_n(input[i], framesNum, output[i]);
            }
        }

        TAudioEffectInterface* Copy() override
        {
            return nullptr;
        }

        void Reset() override
        {

        }

        long Inputs() override { return Channels(); }
        long Outputs() override { return Channels(); }
        long Channels()
        {
            return 2;
        }

        long GetControlCount() override { return {}; }
        void GetControlParam(long param, char* label, float* min, float* max, float* init) override { }
        void SetControlValue(long param, float value) override { }
        void SetControlValue(const char* label, float value) override { }
        float GetControlValue(long param) override { return {}; }
        float GetControlValue(const char* labe) override { return {}; }

        void SetName(const std::string& name) override { }
        std::string GetName() override { return {}; }

};*/
/**
 * @brief The TTimeNodeControlCommand struct
 * Only useful for interactive time nodes.
 * Maybe for events too ?
 */
//struct TTimeNodeControlCommand : public TControlCommand
//{
//        TTimeNodeControlCommand(const Audio::AudioStreamEngine::TimeNodeComponent& ctx):
//            m_audio_parent{ctx}
//        {

//        }

//        const Audio::AudioStreamEngine::TimeNodeComponent& m_audio_parent;

//        //std::vector<SymbolicDate> m_beforeDates;
//        //std::vector<SymbolicDate> m_afterDates;

//        bool Execute(
//                TNonInterleavedAudioBuffer<float>* buffer,
//                map<SymbolicDate, audio_frame_t>& date_map,
//                audio_frame_t cur_frame,
//                long frames)
//        {
//            auto timenode_time = fStartDate->getDate();
//            if (InBuffer(timenode_time, cur_frame, frames))
//            {
//                m_audio_parent.onDateFixed(cur_frame);

//                // Set stop date of all previous constraint to the TN frame
//                for(SymbolicDate date : m_beforeDates)
//                    date->setDate(timenode_time);

//                // Execute the time node.
//                // If we are here, it has already been triggered.
//                // We only have to check the conditions.
//                m_node.trigger();

//                // For all true events, set start date of all next constraints's start, and
//                // trigger their start / their start + offset.
//                for(SymbolicDate date : m_afterDates)
//                    date->setDate(timenode_time + 1);


//                // TODO after each command, the commands should be re-sorted.
//                // TODO the commands should have a priority for hierarchy ?

//                // TODO else find a way to disable' em

//                return false;
//            }
//            else
//            {
//                return true;
//            }
//        }

//};

