#pragma once
#include <iscore/plugins/settingsdelegate/SettingsDelegateModel.hpp>
#include <iscore_plugin_audio_export.h>
namespace Audio
{
namespace Settings
{

class ISCORE_PLUGIN_AUDIO_EXPORT Model : public iscore::SettingsDelegateModel{
        Q_OBJECT

    public:
        Model(QSettings& set, const iscore::ApplicationContext&);

        int getDriverId() const;

        ISCORE_SETTINGS_PARAMETER_HPP(QString, Driver)
        ISCORE_SETTINGS_PARAMETER_HPP(QString, Card)
        ISCORE_SETTINGS_PARAMETER_HPP(int, BufferSize)
        ISCORE_SETTINGS_PARAMETER_HPP(int, Rate)
    private:
        QString m_Driver{};
        QString m_Card{};
        int m_BufferSize{};
        int m_Rate{};
};

ISCORE_SETTINGS_PARAMETER(Model, Driver)
ISCORE_SETTINGS_PARAMETER(Model, Card)
ISCORE_SETTINGS_PARAMETER(Model, BufferSize)
ISCORE_SETTINGS_PARAMETER(Model, Rate)

const QMap<QString, int>& DriverNameMap();
}
}
