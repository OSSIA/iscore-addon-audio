#include "MediaFileHandle.hpp"
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <Audio/AudioDecoder.hpp>


template <>
void DataStreamReader::read(
        const Audio::MediaFileHandle& lm)
{
    m_stream << lm.name();
}

template <>
void DataStreamWriter::writeTo(
        Audio::MediaFileHandle& lm)
{
    QString name;
    m_stream >> name;
    lm = Audio::MediaFileHandle{name};
}



template <>
void JSONObjectReader::read(
        const Audio::MediaFileHandle& lm)
{
    obj["File"] = lm.name();
}

template <>
void JSONObjectWriter::writeTo(
        Audio::MediaFileHandle& lm)
{
    lm = Audio::MediaFileHandle(obj["File"].toString());
}


namespace Audio
{
MediaFileHandle::MediaFileHandle(const QString &filename):
    m_file{filename}
{
    if(isAudioFile(QFile(m_file)))
    {
        m_array = AudioDecoder::readAudio(m_file);
        m_sampleRate = 44100;

        m_data[0] = m_array[0].data();
        if(m_array.size() == 2)
            m_data[1] = m_array[1].data();
    }
}

float**MediaFileHandle::audioData() const
{
    return const_cast<float**>(m_data.data());
}

bool MediaFileHandle::isAudioFile(const QFile& file)
{
    return file.exists() && file.fileName().contains(QRegExp(".(wav|aif|aiff|flac|ogg|mp3)"));
}

int64_t MediaFileHandle::samples() const
{
    return channels() > 0 ? m_array[0].size() : 0;
}

int64_t MediaFileHandle::channels() const
{
    return m_array.size();
}

}
