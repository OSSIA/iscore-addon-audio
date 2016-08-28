#pragma once
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <TAudioEffectInterface.h>

class ExecutorAudioEffect final : public TAudioEffectInterface
{
    public:
        ExecutorAudioEffect(ossia::time_constraint& cst):
            m_root{cst}
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

};
