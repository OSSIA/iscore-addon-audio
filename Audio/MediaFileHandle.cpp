#include "MediaFileHandle.hpp"
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <sndfile.hh>

// Taken from libwatermark
template<typename T>
static std::vector<std::vector<T> > deinterleave(std::vector<T>& in, unsigned int channels, unsigned int frames)
{
    std::vector<std::vector<T>> out;
    out.resize(channels);

    for(auto channel = 0U; channel < channels; ++channel)
    {
        out[channel].resize(frames);
        for(auto frame = 0U; frame < frames; ++frame)
            out[channel][frame] = in[frame * channels + channel];
    }

    return out;
}

template<>
void Visitor<Reader<DataStream>>::readFrom(
        const Audio::MediaFileHandle& lm)
{
    m_stream << lm.name();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::MediaFileHandle& lm)
{
    QString name;
    m_stream >> name;
    lm = Audio::MediaFileHandle{name};
}



template<>
void Visitor<Reader<JSONObject>>::readFrom(
        const Audio::MediaFileHandle& lm)
{
    m_obj["File"] = lm.name();
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::MediaFileHandle& lm)
{
    lm = Audio::MediaFileHandle(m_obj["File"].toString());
}


namespace Audio
{
MediaFileHandle::MediaFileHandle(const QString &filename):
    m_file{filename}
{
    m_array = readFile(QFile(m_file), &m_sampleRate);
}

bool MediaFileHandle::isAudioFile(const QFile& file)
{
    return file.exists() && file.fileName().contains(QRegExp(".(wav|aif|aiff)"));
}

int64_t MediaFileHandle::samples() const
{
    return channels() > 0 ? m_array[0].size() : 0;
}

int64_t MediaFileHandle::channels() const
{
    return m_array.size();
}

AudioArray MediaFileHandle::readFile(const QFile &file, int* sRate)
{
    if(isAudioFile(file))
    {
        auto myf = SndfileHandle(file.fileName().toStdString());
        if(myf.error() != SF_ERR_NO_ERROR)
            return {};

        if (sRate != nullptr) {
            *sRate = myf.samplerate();
        }

        switch(myf.channels())
        {
            case 0:
            {
                return {};
                break;
            }
            case 1:
            {
                std::vector<double> vec(myf.frames());
                myf.read(vec.data(), myf.frames());
                return {vec};
            }
            default:
            {
                std::vector<double> vec(myf.frames() * myf.channels());

                int parity = (myf.frames() % 2 != 0) ? -1 : 0;
                for(int i = 0; i < myf.channels(); ++i)
                    myf.read(vec.data() + i * (myf.frames() + parity),  myf.frames() + parity);

                return deinterleave(
                            vec,
                            (unsigned int) myf.channels(),
                            (unsigned int) myf.frames());
            }
        }
    }
    else
    {
        // TODO Load using QAudioDecoder or something like this; also, mmap / streaming ?
        // Or maybe directly use an AudioStream ?
    }

    return {};
}

}
