#include "CardSettingsModel.hpp"
#include <QSettings>
#include <QFile>
#include <QJsonDocument>
#include <LibAudioStreamMC++.h>
#include <Process/Style/Skin.hpp>

namespace Audio
{
namespace Settings
{

namespace Parameters
{
        const iscore::sp<ModelDriverParameter> Driver{QStringLiteral("Audio/Driver"), ""};
        const iscore::sp<ModelCardParameter> Card{QStringLiteral("Audio/Card"), ""};
        const iscore::sp<ModelBufferSizeParameter> BufferSize{QStringLiteral("Audio/BufferSize"), 512};
        const iscore::sp<ModelRateParameter> Rate{QStringLiteral("Audio/SamplingRate"), 44100};

        auto list() {
            return std::tie(Driver, Card, BufferSize, Rate);
        }
}


Model::Model(QSettings& set, const iscore::ApplicationContext&)
{
    iscore::setupDefaultSettings(set, Parameters::list(), *this);
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

ISCORE_SETTINGS_PARAMETER_CPP(QString, Model, Driver)
ISCORE_SETTINGS_PARAMETER_CPP(QString, Model, Card)
ISCORE_SETTINGS_PARAMETER_CPP(int, Model, BufferSize)
ISCORE_SETTINGS_PARAMETER_CPP(int, Model, Rate)
}
}
