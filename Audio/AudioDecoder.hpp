#pragma once
#include <QAudioDecoder>
#include <vector>
#include <atomic>

namespace Audio
{
class AudioDecoder :
        public QObject
{
    public:
        static std::vector<std::vector<float>> readAudio(const QString& path);

    private:
        AudioDecoder(
                std::vector<std::vector<float>>& p_data,
                const QString& path);

        QAudioDecoder decoder;
        std::vector<std::vector<float>>& data;
        std::atomic_bool ready{false};
};
}
