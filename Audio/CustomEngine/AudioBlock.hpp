#pragma once
#include <vector>
#include <QString>
#include <libwatermark/Parameters.h>
#include <Audio/AudioArray.hpp>

class AudioEngine;
class AudioBlock
{
    public:
        AudioBlock(AudioEngine& en):
            m_engine{en}
        {

        }

        const Parameters<float>& parameters() const;

        virtual ~AudioBlock();
        virtual AudioArray data(int size, int buffer, int offset) const = 0;

        void start();
        void stop();

        int currentBuffer = 0;
        int offset = 0; // Offset in samples between the playing audio and the buffer.

        bool m_deleting = false;
        AudioEngine& m_engine;
};
