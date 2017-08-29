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
        ISCORE_SETTINGS_PARAMETER_HPP(int, InChannels)
        ISCORE_SETTINGS_PARAMETER_HPP(int, OutChannels)
    private:
        QString m_Driver{};
        QString m_Card{};
        int m_BufferSize{};
        int m_Rate{};
        int m_InChannels{};
        int m_OutChannels{};
};

ISCORE_SETTINGS_PARAMETER(Model, Driver)
ISCORE_SETTINGS_PARAMETER(Model, Card)
ISCORE_SETTINGS_PARAMETER(Model, BufferSize)
ISCORE_SETTINGS_PARAMETER(Model, Rate)
ISCORE_SETTINGS_PARAMETER(Model, InChannels)
ISCORE_SETTINGS_PARAMETER(Model, OutChannels)

const QMap<QString, int>& DriverNameMap();
}
}
