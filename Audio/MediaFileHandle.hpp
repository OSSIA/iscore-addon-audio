#pragma once
#include <QFile>
#include <QAudioDecoder>
#include <Audio/AudioArray.hpp>
namespace Audio
{
struct MediaFileHandle
{
    public:
        MediaFileHandle() = default;
        MediaFileHandle(const MediaFileHandle& other) = default;
        MediaFileHandle(MediaFileHandle&& other) = default;
        MediaFileHandle& operator=(const MediaFileHandle& other) = default;
        MediaFileHandle& operator=(MediaFileHandle&& other) = default;

        MediaFileHandle(const QString& filename);

        QString name() const
        { return m_file; }

        const AudioArray& data() const
        { return m_array; }

        int sampleRate() const
        { return m_sampleRate; }

    private:
        AudioArray readFile(const QFile& filename, int* sRate = nullptr);
        QString m_file;
        AudioArray m_array;
        int m_sampleRate;
};
}

Q_DECLARE_METATYPE(Audio::MediaFileHandle)
