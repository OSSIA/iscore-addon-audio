#pragma once

#include <score/plugins/settingsdelegate/SettingsDelegateView.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>

class QSpinBox;
class QComboBox;
class QLabel;

namespace Audio {
namespace Settings {
class View : public score::SettingsDelegateView {
    Q_OBJECT
public:
    View();
    View(Audio::AudioStreamEngine::ApplicationPlugin&);

    void setCard(QString);
    void setBufferSize(int);
    void setSampleRate(int);
    void setInChannels(int);
    void setOutChannels(int);
    void setDriver(QString);

    long getDriver();
    ~View();

signals:
    void cardChanged(QString);
    void bufferSizeChanged(int);
    void rateChanged(int);
    void inChannelsChanged(int);
    void outChannelsChanged(int);
    void driverChanged(QString);

private:
    View(AudioStreamEngine::ApplicationPlugin *);

    QWidget* getWidget() override;
    QWidget* m_widg{};

    QComboBox* m_driverb{};
    QComboBox* m_cardb{};
    QComboBox* m_bsb{};
    QComboBox* m_srb{};
    QSpinBox* m_inChan{};
    QSpinBox* m_outChan{};

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
