#include "MediaFileHandle.hpp"
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <sndfile.hh>
#include <libwatermark/mathutils/math_util.h>


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
    m_array = readFile(QFile(m_file));
}

AudioArray MediaFileHandle::readFile(const QFile &file)
{
    if(file.exists() && file.fileName().contains(".wav"))
    {
        auto myf = SndfileHandle(file.fileName().toStdString());
        if(myf.error() != SF_ERR_NO_ERROR)
            return {};

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

                return MathUtil::deinterleave(
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
