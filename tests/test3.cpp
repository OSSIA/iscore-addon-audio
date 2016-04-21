#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/GroupAudioStream.h>
#include <QtTest/QtTest>
#include <QMetaType>
#include <QObject>
#include <thread>
#include <chrono>
#include <iostream>
#include <ctime>
#include <boost/container/static_vector.hpp>
using namespace std::chrono_literals;

using clck = std::chrono::high_resolution_clock;
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

            m_ctx.player = OpenAudioPlayer(2, 2, 44100, 8192, 65536*4, 44100*60*20, kJackRenderer, 1);
            m_ctx.renderer = GetAudioPlayerRenderer(m_ctx.player);

            auto snd1 = MakeConstantSound(2, 8192, 0.3);
            auto snd2 = MakeConstantSound(2, 8192, 0.5);
            auto date1 = GenSymbolicDate(m_ctx.player);
            auto date2 = GenSymbolicDate(m_ctx.player);
            StartSound(m_ctx.player, snd1, date1);
            StartSound(m_ctx.player, snd2, date2);

            auto fun = [&] (int i ) {
                StartSound(m_ctx.player,
                           MakeConstantSound(2, 512, 0.7),
                           GenRealDate(m_ctx.player, 10 * 44100 + i * 8192));
            };
            for(int i = 0; i < 100; i++)
                fun(i);

            StartAudioPlayer(m_ctx.player);

            std::this_thread::sleep_for(10s);
            std::cerr << std::flush;
            std::cout << std::flush;

            auto orig = clck::now();
            auto res = (orig - orig);
            bool a = false, b = false;
            int64_t frame1, frame2;
            auto t1 = res;
            auto t2 = res;
            while(res < 10s)
            {
                res = (clck::now() - orig);
                auto t = GetAudioPlayerDateInFrame(m_ctx.player);;
                if(t >= 44100 * 11 && !a) {
                    a = true;
                    t1 = res;
                    frame1 = t;
                    SetSymbolicDate(m_ctx.player, date1, frame1);
                }
                if(t >= 44100 * 16 && !b) {
                    b = true;
                    t2 = res;
                    frame2 = t;
                    SetSymbolicDate(m_ctx.player, date2, frame2);
                }
            }

            qDebug() << frame1 << std::chrono::duration_cast<std::chrono::microseconds>(t1).count();
            qDebug() << frame2 << std::chrono::duration_cast<std::chrono::microseconds>(t2).count();

            StopAudioPlayer(m_ctx.player);
            CloseAudioClient(m_ctx.player);
            CloseAudioRenderer(m_ctx.renderer);
            AudioGlobalsDestroy();
        }
};

QTEST_APPLESS_MAIN(test1)
#include "test3.moc"
