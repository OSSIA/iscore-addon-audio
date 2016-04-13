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
    con(v, &View::bufferSizeChanged,
        this, [&] (auto bufferSize) {
        if(bufferSize != m.getBufferSize())
        {
            m_disp.submitCommand<SetBufferSize>(this->model(this), bufferSize);
        }
    });


    con(m, &Model::bufferSizeChanged, &v, &View::setBufferSize);
    v.setBufferSize(m.getBufferSize());

    con(v, &View::rateChanged,
        this, [&] (auto sampleRate) {
        if(sampleRate != m.getRate())
        {
            m_disp.submitCommand<SetRate>(this->model(this), sampleRate);
        }
    });

    con(m, &Model::rateChanged, &v, &View::setSampleRate);
    v.setSampleRate(m.getRate());

    con(m, &Model::driverChanged, &v, &View::setDriver);
    con(m, &Model::cardChanged, &v, &View::setCard);

    con(v, &View::driverChanged, &m, &Model::setDriver);
    con(v, &View::cardChanged, &m, &Model::setCard);

    v.setDriver(m.getDriver());
    v.setCard(m.getCard());
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
