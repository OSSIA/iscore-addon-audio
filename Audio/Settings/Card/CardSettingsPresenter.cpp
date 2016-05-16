#include "CardSettingsPresenter.hpp"
#include "CardSettingsModel.hpp"
#include "CardSettingsView.hpp"
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/command/Dispatchers/ICommandDispatcher.hpp>
#include <QApplication>
#include <QStyle>
#include <iscore/command/SettingsCommand.hpp>

namespace Audio {
namespace Settings {

Presenter::Presenter(
        Model& m,
        View& v,
        QObject *parent):
    iscore::SettingsDelegatePresenterInterface{m, v, parent}
{
    // Buffer size
    con(m, &Model::BufferSizeChanged, &v, &View::setBufferSize);
    con(v, &View::bufferSizeChanged,
        this, [&] (auto bufferSize) {
        if(bufferSize != m.getBufferSize())
        {
            m_disp.submitCommand<SetModelBufferSize>(this->model(this), bufferSize);
        }
    });

    v.setBufferSize(m.getBufferSize());

    // Rate
    con(m, &Model::RateChanged, &v, &View::setSampleRate);
    con(v, &View::rateChanged,
        this, [&] (auto sampleRate) {
        if(sampleRate != m.getRate())
        {
            m_disp.submitCommand<SetModelRate>(this->model(this), sampleRate);
        }
    });

    v.setSampleRate(m.getRate());

    // Driver
    con(m, &Model::DriverChanged, &v, &View::setDriver);
    con(v, &View::driverChanged,
        this, [&] (auto driver) {
        if(driver != m.getDriver())
        {
            m_disp.submitCommand<SetModelDriver>(this->model(this), driver);
        }
    });
    v.setDriver(m.getDriver());

    // Card
    con(m, &Model::CardChanged, &v, &View::setCard);
    con(v, &View::cardChanged,
        this, [&] (auto card) {
        if(card != m.getCard())
        {
            m_disp.submitCommand<SetModelCard>(this->model(this), card);
        }
    });

    v.setCard(m.getCard());

    qDebug() << m.getDriver() << m.getCard();
}

QString Presenter::settingsName()
{
    return tr("Audio");
}

QIcon Presenter::settingsIcon()
{
    return QApplication::style()->standardIcon(QStyle::SP_MediaVolume);
}


}
}
