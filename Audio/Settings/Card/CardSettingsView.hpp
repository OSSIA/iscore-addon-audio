#pragma once

#include <iscore/plugins/settingsdelegate/SettingsDelegateView.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>

class QComboBox;
class QLabel;

namespace Audio {
namespace Settings {
class View : public iscore::SettingsDelegateView {
    Q_OBJECT
public:
    View();
    View(Audio::AudioStreamEngine::ApplicationPlugin&);

    void setCard(QString);
    void setBufferSize(int);
    void setSampleRate(int);
    void setDriver(QString);

    long getDriver();
    ~View();

signals:
    void cardChanged(QString);
    void bufferSizeChanged(int);
    void rateChanged(int);
    void driverChanged(QString);

private:
    View(AudioStreamEngine::ApplicationPlugin *);

    QWidget* getWidget() override;
    QWidget* m_widg{};

    QComboBox* m_driverb{};
    QComboBox* m_cardb{};
    QComboBox* m_bsb{};
    QComboBox* m_srb{};

    // Label displaying latency depending on selected options
    QLabel* m_ll{};
    // Label displaying audio card info
    QLabel* m_infol{};

    Audio::AudioStreamEngine::ApplicationPlugin* m_aseplug;
    std::map<long, int> driversMapping;

    int getCardIndex(QString);
    int getDriverIndex(QString);
    void setBufferSizeFromIndex(int);
    void setSampleRateFromIndex(int);
    void setDriverFromIndex(int);
    void setCardFromIndex(int);

    void displayLatency();
    void populateDrivers();
    void populateCards();
    void displayInfos();
    void addDriverOption(long);
};

}
}
