#pragma once
#include <Audio/AudioBlock.hpp>
#include <faust/dsp/llvm-dsp.h>
namespace Audio
{
namespace Faust
{
class Block : public AudioBlock
{
    public:
        llvm_dsp_factory* m_faustFactory{};
        llvm_dsp* m_faustPlug{};
        std::vector<float> m_audio;


        FaustAudioBlock(
                const QString& script,
                std::vector<float> audio,
                AudioEngine& params);

        ~FaustAudioBlock();


        std::vector<float> data(int size, int buffer, int offset) const override;


};
}
}
