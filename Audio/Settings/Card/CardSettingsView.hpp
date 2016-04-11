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

    int getCardIndex(QString);
    ~View();

signals:
    void cardChanged(int);
    void bufferSizeChanged(int);
    void rateChanged(int);

private:
    View(AudioStreamEngine::ApplicationPlugin *);

    QWidget* getWidget() override;
    QWidget* m_widg{};

    QComboBox* m_cardb{};
    QComboBox* m_bsb{};
    QComboBox* m_srb{};

    // Label displaying latency depending on selected options
    QLabel* m_ll{};
    // Label displaying audio card info
    QLabel* m_infol{};

    Audio::AudioStreamEngine::ApplicationPlugin* m_aseplug;

    void displayLatency();
    void populateCards();
    void displayInfos();
};

}
}
