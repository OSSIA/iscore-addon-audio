#include "CardSettingsModel.hpp"
#include <QSettings>
#include <QFile>
#include <QJsonDocument>
#include <LibAudioStreamMC++.h>
#include <score/model/Skin.hpp>

namespace Audio
{
namespace Settings
{

namespace Parameters
{
        const score::sp<ModelDriverParameter> Driver{QStringLiteral("Audio/Driver"), "Offline"};
        const score::sp<ModelCardParameter> Card{QStringLiteral("Audio/Card"), ""};
        const score::sp<ModelBufferSizeParameter> BufferSize{QStringLiteral("Audio/BufferSize"), 512};
        const score::sp<ModelRateParameter> Rate{QStringLiteral("Audio/SamplingRate"), 44100};
        const score::sp<ModelInChannelsParameter> InChannels{QStringLiteral("Audio/InChannels"), 2};
        const score::sp<ModelOutChannelsParameter> OutChannels{QStringLiteral("Audio/OutChannels"), 2};

        static auto list() {
            return std::tie(Driver, Card, BufferSize, Rate, InChannels, OutChannels);
        }
}


Model::Model(QSettings& set, const score::ApplicationContext&)
{
    score::setupDefaultSettings(set, Parameters::list(), *this);
}

int Model::getDriverId() const
{
    auto& idmap = DriverNameMap();
    auto it = idmap.constFind(m_Driver);
    if(it != idmap.constEnd())
    {
        return *it;
    }

    return -1;
}

const QMap<QString, int>& DriverNameMap()
{
    static const QMap<QString, int> idmap{
        std::make_pair("PortAudio", kPortAudioRenderer),
                std::make_pair("JACK", kJackRenderer),
                std::make_pair("NetJACK", kNetJackRenderer),
                std::make_pair("CoreAudio", kCoreAudioRenderer),
                std::make_pair("Offline", kOffLineAudioRenderer)
    };

    return idmap;
}

SCORE_SETTINGS_PARAMETER_CPP(QString, Model, Driver)
SCORE_SETTINGS_PARAMETER_CPP(QString, Model, Card)
SCORE_SETTINGS_PARAMETER_CPP(int, Model, BufferSize)
SCORE_SETTINGS_PARAMETER_CPP(int, Model, Rate)
SCORE_SETTINGS_PARAMETER_CPP(int, Model, InChannels)
SCORE_SETTINGS_PARAMETER_CPP(int, Model, OutChannels)
}
}
