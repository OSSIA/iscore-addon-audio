#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/Streams/GroupAudioStream.h>
#include <QtTest/QtTest>
#include <QMetaType>
#include <QObject>
#include <thread>
#include <chrono>
#include <iostream>
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
            auto renderer = kPortAudioRenderer;
            AudioContext m_ctx;
            GetDeviceInfo(renderer, 0, &m_ctx.device_info);
            auto& dev = m_ctx.device_info;
            qDebug() << dev.fName
                     << dev.fMaxInputChannels
                     << dev.fMaxOutputChannels
                     << dev.fDefaultBufferSize
                     << dev.fDefaultSampleRate;

            m_ctx.player = OpenAudioPlayer(2, 2, 44100, 512, 65536*4, 44100*60*20, renderer, 1);
            m_ctx.renderer = GetAudioPlayerRenderer(m_ctx.player);

            std::cout << std::flush;
            std::cerr << std::flush;
            // Build the first stream
/*
            auto stream1 = [] {
                auto player = MakeGroupPlayer();
                auto date1 = GenRealDate(player, 44100);
                auto date2 = GenRealDate(player, 44100*2);

                // First sound
                StartSound(player,
                           MakeFadeSound(
                               MakeSinusStream(44100 * 10, 330), 44100, 2048),
                           date1);

                // Second sound
                StartSound(player,
                           MakeFadeSound(
                               MakeSinusStream(44100 * 10, 220), 44100, 2048),
                           date2);

                // Third sound
                //auto file = MakeReadSound("/tmp/1.wav");
               // StartSound(player, file, date2);

                // Make a stream on the sound
                return MakeGroupStream(player);
            }();



            StartAudioPlayer(m_ctx.player);

            StartSound(m_ctx.player, stream1, GenRealDate(m_ctx.player, 0));
            for(int i = 5; i --> 0;)
            {
                std::this_thread::sleep_for(1s);
            }
*/
           auto stream1 = MakeReadSound("/tmp/1-stereo.wav");
           auto stream2 = MakeReadSound("/tmp/1-stereo.wav");

           StartSound(m_ctx.player, stream2, GenRealDate(m_ctx.player, 0));

            auto fx_send = MakeSend(stream1);

            // TODO for now the sound has to be played first; it could be
            // nice if this was a real graph
            StartSound(m_ctx.player, fx_send, GenRealDate(m_ctx.player, 44100 * 1.5));
            StopSound(m_ctx.player, fx_send, GenRealDate(m_ctx.player, 44100 * 3));

            // Build a first effect stream
            {

                auto fx_1 = MakeFaustAudioEffect("/tmp/examples/freeverb.dsp", "/usr/local/lib/faust/architecture", "");
                auto ctrl = GetControlCountEffect(fx_1);
                for(int i = 0; i < ctrl; i++)
                    SetControlValueEffect(fx_1, i, 1.0);
                auto fx_return_1 = MakeReturn(fx_send);
                auto fx_chain_1 = MakeEffectSound(fx_return_1, fx_1, 0, 0);

                StartSound(m_ctx.player, fx_chain_1, GenRealDate(m_ctx.player, 44100));
                StopSound(m_ctx.player, fx_chain_1, GenRealDate(m_ctx.player, 44100 *2));

            }

            // Build a second effect stream
            {
                auto fx_2 = MakeFaustAudioEffect("/tmp/examples/freeverb.dsp", "/usr/local/lib/faust/architecture/", "");
                auto fx_return_2 = MakeReturn(fx_send);
                auto fx_chain_2 = MakeEffectSound(fx_return_2, fx_2, 0, 0);

                StartSound(m_ctx.player, fx_chain_2, GenRealDate(m_ctx.player, 44100 * 2));
            }

            std::cout << std::flush;
            std::cerr << std::flush;



            // Mix the streams
            StartAudioPlayer(m_ctx.player);

            for(int i = 15; i --> 0;)
                std::this_thread::sleep_for(1s);

            StopAudioPlayer(m_ctx.player);
            CloseAudioClient(m_ctx.player);
            CloseAudioRenderer(m_ctx.renderer);
            AudioGlobalsDestroy();
        }
};

QTEST_APPLESS_MAIN(test1)
#include "test2.moc"
