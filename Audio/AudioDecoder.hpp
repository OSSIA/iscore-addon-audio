#pragma once
#include <QAudioDecoder>
#include <vector>
#include <atomic>
#include <Audio/AudioArray.hpp>

namespace Audio
{
class AudioDecoder :
        public QObject
{
    public:
        static AudioArray readAudio(const QString& path);

    private:
        AudioDecoder(
                AudioArray& p_data,
                const QString& path);

        QAudioDecoder decoder;
        AudioArray& data;
        std::atomic_bool ready{false};
};
}
