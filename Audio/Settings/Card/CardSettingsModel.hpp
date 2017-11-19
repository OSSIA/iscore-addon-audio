#pragma once
#include <score/plugins/settingsdelegate/SettingsDelegateModel.hpp>
#include <score_plugin_audio_export.h>
namespace Audio
{
namespace Settings
{

class SCORE_PLUGIN_AUDIO_EXPORT Model : public score::SettingsDelegateModel{
        Q_OBJECT

    public:
        Model(QSettings& set, const score::ApplicationContext&);

        int getDriverId() const;

        SCORE_SETTINGS_PARAMETER_HPP(QString, Driver)
        SCORE_SETTINGS_PARAMETER_HPP(QString, Card)
        SCORE_SETTINGS_PARAMETER_HPP(int, BufferSize)
        SCORE_SETTINGS_PARAMETER_HPP(int, Rate)
        SCORE_SETTINGS_PARAMETER_HPP(int, InChannels)
        SCORE_SETTINGS_PARAMETER_HPP(int, OutChannels)
    private:
        QString m_Driver{};
        QString m_Card{};
        int m_BufferSize{};
        int m_Rate{};
        int m_InChannels{};
        int m_OutChannels{};
};

SCORE_SETTINGS_PARAMETER(Model, Driver)
SCORE_SETTINGS_PARAMETER(Model, Card)
SCORE_SETTINGS_PARAMETER(Model, BufferSize)
SCORE_SETTINGS_PARAMETER(Model, Rate)
SCORE_SETTINGS_PARAMETER(Model, InChannels)
SCORE_SETTINGS_PARAMETER(Model, OutChannels)

const QMap<QString, int>& DriverNameMap();
}
}
