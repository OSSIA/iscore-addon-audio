#pragma once
#include <iscore/plugins/settingsdelegate/SettingsDelegateModelInterface.hpp>

namespace Audio
{
namespace Settings
{

struct Keys
{
        static const QString driver;
        static const QString card;
        static const QString bufferSize;
        static const QString samplingRate;
};

class Model : public iscore::SettingsDelegateModelInterface{
        Q_OBJECT

    public:
        Model();

        int getDriverId() const;
        QString getDriver() const;
        void setDriver(const QString&);

        QString getCard() const;
        void setCard(const QString&);

        int  getBufferSize() const;
        void setBufferSize(int);

        int  getRate() const;
        void setRate(int);

    signals:
        void driverChanged(const QString&);
        void cardChanged(const QString&);
        void bufferSizeChanged(int);
        void rateChanged(int);

    private:
        void setFirstTimeSettings() override;
        QString m_driver{};
        QString m_card{};
        int m_bufferSize{};
        int m_rate{};
};

ISCORE_SETTINGS_PARAMETER(Model, Driver)
ISCORE_SETTINGS_PARAMETER(Model, Card)
ISCORE_SETTINGS_PARAMETER(Model, BufferSize)
ISCORE_SETTINGS_PARAMETER(Model, Rate)

const QMap<QString, int>& DriverNameMap();
}
}
