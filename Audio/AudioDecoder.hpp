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
  Q_OBJECT
    public:
        AudioDecoder();
        void decode(const QString& path);

        QAudioDecoder decoder;
        AudioArray data;
        std::atomic_bool ready{false};

    signals:
        void finished();
        void failed();
};

}
