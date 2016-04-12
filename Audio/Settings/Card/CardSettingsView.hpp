#pragma once

#include <iscore/plugins/settingsdelegate/SettingsDelegateViewInterface.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>

class QComboBox;
class QLabel;

namespace Audio {
namespace Settings {

class View : public iscore::SettingsDelegateViewInterface {
    Q_OBJECT
public:
    View();
    View(Audio::AudioStreamEngine::ApplicationPlugin&);

    void setCard(QString);
    void setCard(int);
    void setBufferSize(int);
    void setSampleRate(int);
    void setDriver(int);

    int getCardIndex(QString);
    long getDriver();
    ~View();

signals:
    void cardChanged(int);
    void bufferSizeChanged(int);
    void rateChanged(int);
    void driverChanged(int);

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

    void displayLatency();
    void populateDrivers();
    void populateCards();
    void displayInfos();
    void addDriverOption(long);
};

}
}
