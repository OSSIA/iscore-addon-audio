#include "AudioDecoder.hpp"
#include <QApplication>

namespace Audio
{

std::vector<std::vector<float> > AudioDecoder::readAudio(const QString& path)
{
    std::vector<std::vector<float>> dat;
    AudioDecoder s{dat, path};

    while(!s.ready)
        qApp->processEvents();

    return dat;
}

static float int_to_float(int16_t i)
{
    return (i + .5)/(0x7FFF+.5);
}

AudioDecoder::AudioDecoder(std::vector<std::vector<float> >& p_data, const QString& path):
    data{p_data}
{
    QAudioFormat desiredFormat;
    desiredFormat.setChannelCount(-1);
    desiredFormat.setCodec("audio/x-raw");
    desiredFormat.setSampleRate(44100);
    desiredFormat.setSampleSize(16);
    desiredFormat.setSampleType(QAudioFormat::SignedInt);

    data.resize(2);
    decoder.setAudioFormat(desiredFormat);
    decoder.setSourceFilename(path);

    if(decoder.error() != QAudioDecoder::NoError)
    {
        qDebug() << decoder.errorString();
        ready = true;
        return;
    }

    qDebug() << (decoder.audioFormat() == desiredFormat);
    connect(&decoder, &QAudioDecoder::bufferReady, this,
            [&] () {
        const auto& buf = decoder.read();
        if(!buf.isValid())
        {
            ready = true;
            return;
        }

        auto fmt = buf.format();
        int n = buf.frameCount();
        if(fmt.sampleType() == QAudioFormat::SignedInt &&
           fmt.sampleSize() == 16)
        {
            if(fmt.channelCount() == 2)
            {
                auto dat = buf.data<QAudioBuffer::S16S>();
                data[0].reserve(data[0].size() + buf.frameCount());
                data[1].reserve(data[1].size() + buf.frameCount());
                for(int j = 0; j < n; j++)
                {
                    data[0].push_back(int_to_float(dat[j].left));
                    data[1].push_back(int_to_float(dat[j].right));
                }
            }
            else if(fmt.channelCount() == 1)
            {
                auto dat = buf.data<int16_t>();
                data[0].reserve(data[0].size() + buf.frameCount());
                for(int j = 0; j < n; j++)
                {
                    data[0].push_back(int_to_float(dat[j]));
                }
            }
        }
        else
        {
            qDebug() << "Bad format :" << buf.format();
        }

    });

    connect(&decoder, &QAudioDecoder::stateChanged, this,
            [=] (QAudioDecoder::State s) {
        if(s == QAudioDecoder::StoppedState)
        {
            qDebug() << "okay !";
            if(data[1].empty())
                data.resize(1);

            ready.store(true);
        }
    });

    decoder.start();
}

}
