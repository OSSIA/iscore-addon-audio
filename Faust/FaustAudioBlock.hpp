#pragma once
#include <Audio/CustomEngine/AudioBlock.hpp>
#include <vector>
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


        FaustAudioBlock(
                const QString& script,
                AudioEngine& params);

        ~FaustAudioBlock();


        AudioArray data(int size, int buffer, int offset) const override;

    private:
        AudioArray in_vec_cnt(2);
        AudioArray out_vec_cnt(2);
};
}
}
