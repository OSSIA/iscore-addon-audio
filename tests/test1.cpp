#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/Streams/GroupAudioStream.h>
#include <QtTest/QtTest>
#include <QMetaType>
#include <QObject>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

class test1: public QObject
{
        Q_OBJECT
    public:


        struct AudioContext
        {
                AudioPlayerPtr player{};
                AudioRendererPtr renderer{};
                DeviceInfo device_info{};
                RendererInfo renderer_info{};
        };

    private slots:

        void equalityTest()
        {
            using namespace Audio;
            using namespace Audio::AudioStreamEngine;
            AudioContext m_ctx;
            GetDeviceInfo(kJackRenderer, 0, &m_ctx.device_info);
            auto& dev = m_ctx.device_info;
            qDebug() << dev.fName
                     << dev.fMaxInputChannels
                     << dev.fMaxOutputChannels
                     << dev.fDefaultBufferSize
                     << dev.fDefaultSampleRate;

            m_ctx.player = OpenAudioPlayer(2, 2, 44100, 512, 65536*4, 44100*60*20, kJackRenderer, 1);
            m_ctx.renderer = GetAudioPlayerRenderer(m_ctx.player);

            auto player = MakeGroupPlayer();
            auto symdate1 = GenSymbolicDate(player);
            auto symdate2 = GenSymbolicDate(player);

            // First sound
            StartSound(player,
                       MakeFadeSound(
                           MakeSinusStream(44100 * 10, 330), 44100, 2048),
                       symdate1);

            // Second sound
            StartSound(player,
                       MakeFadeSound(
                           MakeSinusStream(44100 * 10, 220), 44100, 2048),
                       symdate2);

            // Third sound
            auto file = MakeReadSound("/tmp/1.wav");
            StartSound(player, file, symdate2);

            // Make a stream on the sound
            auto stream = MakeGroupStream(player);

            // Add an effec on the stream
            auto fx = MakeFaustAudioEffect("/tmp/examples/freeverb.dsp", "", "");
            qDebug() << GetLastLibError();

            auto effect = MakeEffectSound(stream, fx, 0, 0);
            StartSound(m_ctx.player, effect, GenRealDate(m_ctx.player, 0));
            StartAudioPlayer(m_ctx.player);

            for(int i = 10; i --> 0;)
            {
                std::this_thread::sleep_for(1s);
                switch(i)
                {
                    case 3:
                        SetSymbolicDate(player, symdate1, GetAudioPlayerDateInFrame(player));
                        break;
                    case 5:
                        SetSymbolicDate(player, symdate2, GetAudioPlayerDateInFrame(player));
                        break;
                }
            }

            StopAudioPlayer(m_ctx.player);
            CloseAudioClient(m_ctx.player);
            CloseAudioRenderer(m_ctx.renderer);
            AudioGlobalsDestroy();
        }
};

QTEST_APPLESS_MAIN(test1)
#include "test1.moc"
