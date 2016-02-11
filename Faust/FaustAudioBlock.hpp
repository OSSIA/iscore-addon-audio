#pragma once
#include <Audio/AudioBlock.hpp>
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


        std::vector<float> data(int size, int buffer, int offset) const override;

    private:
        std::vector<std::vector<float>> in_vec_cnt(2);
        std::vector<std::vector<float>> out_vec_cnt(2);
};
}
}
