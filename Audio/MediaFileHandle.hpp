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

        float** audioData() const;

        int sampleRate() const
        { return m_sampleRate; }

        static bool isAudioFile(const QFile& f);

        // Number of samples in a channel.
        int64_t samples() const;
        int64_t channels() const;

        bool empty() const
        { return channels() == 0 || samples() == 0; }

    private:
        QString m_file;
        AudioArray m_array;
        std::array<float*, 2> m_data;
        int m_sampleRate;
};
}

Q_DECLARE_METATYPE(Audio::MediaFileHandle)
