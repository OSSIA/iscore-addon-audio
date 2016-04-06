#include "CardSettingsModel.hpp"
#include <QSettings>
#include <QFile>
#include <QJsonDocument>

#include <Process/Style/Skin.hpp>
namespace Audio
{
namespace Settings
{

const QString Keys::driver = QStringLiteral("Audio/Driver");
const QString Keys::card = QStringLiteral("Audio/Card");
const QString Keys::bufferSize = QStringLiteral("Audio/BufferSize");
const QString Keys::samplingRate = QStringLiteral("Audio/SamplingRate");


Model::Model()
{
    QSettings s;

    if(!s.contains(Keys::driver) ||
       !s.contains(Keys::card) ||
       !s.contains(Keys::bufferSize) ||
       !s.contains(Keys::samplingRate))
    {
        setFirstTimeSettings();
    }
    else
    {
        setCard(s.value(Keys::driver).toString());
        setCard(s.value(Keys::card).toString());
        setBufferSize(s.value(Keys::bufferSize).toInt());
        setRate(s.value(Keys::samplingRate).toInt());
    }

    // TODO validate audio settings
}

QString Model::getDriver() const
{
    return m_driver;
}

void Model::setDriver(const QString& driver)
{
    if (m_driver == driver)
        return;

    m_driver = driver;

    QSettings s;
    s.setValue(Keys::driver, m_driver);
    emit driverChanged(driver);
}


QString Model::getCard() const
{
    return m_card;
}

void Model::setCard(const QString& card)
{
    if (m_card == card)
        return;

    m_card = card;

    QSettings s;
    s.setValue(Keys::card, m_card);
    emit cardChanged(card);
}

int Model::getBufferSize() const
{
    return m_bufferSize;
}

void Model::setBufferSize(int buf)
{
    if(m_bufferSize == buf)
        return;

    m_bufferSize = buf;
    QSettings s;
    s.setValue(Keys::bufferSize, m_bufferSize);
    emit bufferSizeChanged(m_bufferSize);
}

int Model::getRate() const
{
    return m_rate;
}

void Model::setRate(int rate)
{
    if(m_rate == rate)
        return;

    m_rate = rate;
    QSettings s;
    s.setValue(Keys::samplingRate, m_rate);
    emit rateChanged(m_rate);
}


void Model::setFirstTimeSettings()
{
    setDriver("");
    setCard("");
    setBufferSize(512);
    setRate(44100);
}

}
}
