#include "AudioDecoder.hpp"
#include <QApplication>
#include <QTimer>
#include <eggs/variant.hpp>
namespace Audio
{
AudioArray AudioDecoder::readAudio(const QString& path)
{
    qDebug() << "decoding...";
    AudioArray dat;
    AudioDecoder s{dat, path};

    std::atomic_bool timeout{false};
    QTimer::singleShot(5000, [&] () { timeout = true; });
    while(!s.ready && !timeout)
        qApp->processEvents();

    if(s.decoder.error() != QAudioDecoder::NoError)
        qDebug() << s.decoder.errorString();
    else if(timeout)
        qDebug() << s.decoder.errorString() << s.decoder.state();

    qDebug() << "decoded..." << dat.size() << dat[0].size();
    return dat;
}

namespace
{
template<QAudioFormat::SampleType SampleFormat, int N>
struct ConvertToFloat;

template<>
struct ConvertToFloat<QAudioFormat::SignedInt, 16>
{
        using base_type = int16_t;
        static const constexpr int multiplier = 1;
        constexpr float operator()(int16_t i) const
        { return (i + .5) / (0x7FFF + .5); }
};

template<>
struct ConvertToFloat<QAudioFormat::SignedInt, 24>
{
        using base_type = const unsigned char;
        static const constexpr int multiplier = 3;
        constexpr float operator()(const unsigned char& src_r) const
        {
            return impl(&src_r);
        }

        constexpr static float impl(const unsigned char* src)
        {
            return int32_t(src[2] << 24 | src[1] << 16 | src[0] << 8) / (float)(std::numeric_limits<int32_t>::max() - 256);
        }
};

template<>
struct ConvertToFloat<QAudioFormat::SignedInt, 32>
{
        using base_type = int32_t;
        static const constexpr int multiplier = 1;
        constexpr float operator()(int32_t i) const
        { return i / (float)(std::numeric_limits<int32_t>::max()); }
};

template<>
struct ConvertToFloat<QAudioFormat::Float, 32>
{
        using base_type = float;
        static const constexpr int multiplier = 1;
        constexpr float operator()(float i) const
        { return i; }
};


template<int Channels, QAudioFormat::SampleType SampleFormat, int SampleSize>
struct Decoder;

template<QAudioFormat::SampleType SampleFormat, int SampleSize>
struct Decoder<1, SampleFormat, SampleSize>
{
        using converter_t = ConvertToFloat<SampleFormat, SampleSize>;
        void operator()(AudioArray& data, const QAudioBuffer& buf)
        {
            int n = buf.frameCount();
            auto dat = buf.data<typename converter_t::base_type>();
            data[0].reserve(data[0].size() + buf.frameCount());
            for(int j = 0; j < n; j++)
            {
                data[0].push_back(converter_t{}(dat[j * converter_t::multiplier]));
            }
        }
};

template<>
struct Decoder<2, QAudioFormat::SignedInt, 24>
{
        using converter_t = ConvertToFloat<QAudioFormat::SignedInt, 24>;
        void operator()(AudioArray& data, const QAudioBuffer& buf)
        {
            int n = buf.frameCount();
            auto dat = buf.data<const unsigned char>();
            data[0].reserve(data[0].size() + buf.frameCount());
            data[1].reserve(data[1].size() + buf.frameCount());
            for(int j = 0; j < 2 * n; )
            {
                data[0].push_back(converter_t{}(dat[j * 3]));
                j++;
                data[1].push_back(converter_t{}(dat[j * 3]));
                j++;
            }
        }
};

template<QAudioFormat::SampleType SampleFormat, int SampleSize>
struct Decoder<2, SampleFormat, SampleSize>
{
        using converter_t = ConvertToFloat<SampleFormat, SampleSize>;
        void operator()(AudioArray& data, const QAudioBuffer& buf)
        {
            int n = buf.frameCount();
            auto dat = buf.data<typename converter_t::base_type>();
            data[0].reserve(data[0].size() + buf.frameCount());
            data[1].reserve(data[1].size() + buf.frameCount());
            for(int j = 0; j < 2 * n; )
            {
                data[0].push_back(converter_t{}(dat[j * converter_t::multiplier]));
                j++;
                data[1].push_back(converter_t{}(dat[j * converter_t::multiplier]));
                j++;
            }
        }
};
struct decode_visitor
{
        AudioArray& data;
        const QAudioBuffer& buf;

        template<typename T>
        void operator()(T decoder)
        {
            decoder(data, buf);
        }
};

 static eggs::variant<
 Decoder<1, QAudioFormat::SignedInt, 16>,
 Decoder<2, QAudioFormat::SignedInt, 16>,
 Decoder<1, QAudioFormat::SignedInt, 24>,
 Decoder<2, QAudioFormat::SignedInt, 24>,
 Decoder<1, QAudioFormat::SignedInt, 32>,
 Decoder<2, QAudioFormat::SignedInt, 32>,
 Decoder<1, QAudioFormat::Float, 32>,
 Decoder<2, QAudioFormat::Float, 32>> make_decoder(QAudioFormat format)
{
    int size = format.sampleSize();
    int chan = format.channelCount();

    switch(format.sampleType())
    {
        case QAudioFormat::SignedInt:
            switch(size)
            {
                case 16:
                    switch(chan)
                    {
                        case 1: return Decoder<1, QAudioFormat::SignedInt, 16>{};
                        case 2: return Decoder<2, QAudioFormat::SignedInt, 16>{};
                        default: return {};
                    }
                case 24:
                    switch(chan)
                    {
                        case 1: return Decoder<1, QAudioFormat::SignedInt, 24>{};
                        case 2: return Decoder<2, QAudioFormat::SignedInt, 24>{};
                        default: return {};
                    }
                case 32:
                    switch(chan)
                    {
                        case 1: return Decoder<1, QAudioFormat::SignedInt, 32>{};
                        case 2: return Decoder<2, QAudioFormat::SignedInt, 32>{};
                        default: return {};
                    }
                default:
                    return {};
            }

        case QAudioFormat::Float:
            if(size == 32)
            {
                switch(chan)
                {
                    case 1: return Decoder<1, QAudioFormat::Float, 32>{};
                    case 2: return Decoder<2, QAudioFormat::Float, 32>{};
                    default: return {};
                }
            }
        default:
            return {};
    }
}
}
AudioDecoder::AudioDecoder(AudioArray& p_data, const QString& path):
    data{p_data}
{
    QAudioFormat desiredFormat;
    desiredFormat.setChannelCount(-1);
    desiredFormat.setCodec("audio/x-raw");
    desiredFormat.setSampleRate(44100);
    desiredFormat.setSampleSize(16);
    desiredFormat.setSampleType(QAudioFormat::SignedInt);
    desiredFormat.setByteOrder(QAudioFormat::LittleEndian);

    data.resize(2);
    decoder.setAudioFormat(desiredFormat);
    decoder.setSourceFilename(path);

    if(decoder.error() != QAudioDecoder::NoError)
    {
        qDebug() << decoder.errorString();
        ready = true;
        return;
    }

    connect(&decoder, &QAudioDecoder::bufferReady, this,
            [&] () {
        const auto& buf = decoder.read();
        if(!buf.isValid())
        {
            decoder.stop();
            ready = true;
            return;
        }
        auto dec = make_decoder(buf.format());
        if(dec)
        {
            eggs::variants::apply(decode_visitor{data, buf}, dec);
        }
        else
        {
            qDebug() << "Unsupported format :" << buf.format();
            decoder.stop();
            ready = true;
        }

    });

    connect(&decoder, &QAudioDecoder::stateChanged, this,
            [=] (QAudioDecoder::State s) {
        if(s == QAudioDecoder::StoppedState)
        {
            if(data[1].empty())
                data.resize(1);

            ready.store(true);
        }
    });

    decoder.start();
    if(decoder.error() != QAudioDecoder::NoError)
    {
        qDebug() << decoder.errorString();
        ready = true;
        return;
    }
}

}
