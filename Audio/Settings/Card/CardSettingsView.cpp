#include "CardSettingsView.hpp"
#include "CardSettingsModel.hpp"
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <sstream>
#include <QFormLayout>
#include <LibAudioStreamMC++.h>
#include <iscore/application/ApplicationContext.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
#include <iscore/widgets/SignalUtils.hpp>

namespace Audio {
namespace Settings {

View::View() : View(nullptr) {}

View::View(AudioStreamEngine::ApplicationPlugin & p) : View(&p) {}

View::View(AudioStreamEngine::ApplicationPlugin * p) :
    m_widg{new QWidget},
    m_driverb(new QComboBox),
    m_cardb{new QComboBox},
    m_bsb{new QComboBox},
    m_srb{new QComboBox},
    m_inChan{new QSpinBox},
    m_outChan{new QSpinBox},
    m_ll{new QLabel},
    m_infol{new QLabel},
    m_aseplug{p},
    driversMapping(std::map<long, int> ())
{
    auto lay = new QFormLayout;
    m_widg->setLayout(lay);

    m_bsb->addItems(QStringList({QString("32"), QString("48"), QString("64"), QString("128"), QString("256"), QString("512"), QString("1024"), QString("2048")}));
    m_srb->addItems(QStringList({QString("32000"), QString("44100"), QString("48000"), QString("88200"), QString("96000"), QString("192000")}));

    m_infol->setTextFormat(Qt::RichText);

    m_inChan->setRange(0, 1024);
    m_outChan->setRange(0, 1024);

    lay->addRow(tr("Buffer size"), m_bsb);
    lay->addRow(tr("Sample rate (Hz)"), m_srb);
    lay->addRow(tr("Inputs"), m_inChan);
    lay->addRow(tr("Outputs"), m_outChan);
    lay->addRow(tr("Latency"), m_ll);
    lay->addRow(tr("Driver"), m_driverb);
    lay->addRow(tr("Audio device"), m_cardb);
    lay->addRow(tr("Device info"), m_infol);

    connect(m_bsb, SignalUtils::QComboBox_currentIndexChanged_int(),
            this, &View::setBufferSizeFromIndex);
    connect(m_srb, SignalUtils::QComboBox_currentIndexChanged_int(),
            this, &View::setSampleRateFromIndex);

    connect(m_driverb, &QComboBox::currentTextChanged,
            this, &View::driverChanged);

    connect(m_cardb, SignalUtils::QComboBox_currentIndexChanged_int(),
            this, &View::displayInfos);
    connect(m_cardb, &QComboBox::currentTextChanged,
            this, &View::cardChanged);

    connect(m_inChan, SignalUtils::QSpinBox_valueChanged_int(),
            this, [=] (int v) { emit inChannelsChanged(v); });
    connect(m_outChan, SignalUtils::QSpinBox_valueChanged_int(),
            this, [=] (int v) { emit outChannelsChanged(v); });

    driversMapping.insert(std::pair<long, int> (kPortAudioRenderer, -1));
    driversMapping.insert(std::pair<long, int> (kJackRenderer, -1));
    driversMapping.insert(std::pair<long, int> (kNetJackRenderer, -1));
    driversMapping.insert(std::pair<long, int> (kCoreAudioRenderer, -1));
    driversMapping.insert(std::pair<long, int> (kOffLineAudioRenderer, -1));

    // TODO do this async
    populateDrivers();
    displayLatency();
}

int View::getCardIndex(QString name) {
    for (int i = 0; i < m_cardb->count(); ++i) {
        if (name == m_cardb->itemText(i)) {
            return i;
        }
    }
    return -1;
}

void View::setCard(QString name) {
    int idx = getCardIndex(name);
    if (idx != -1) {
        setCardFromIndex(idx);
    }
}

void View::setCardFromIndex(int index) {
    if (index != m_cardb->currentIndex()) {
        m_cardb->setCurrentIndex(index);
        emit cardChanged(m_cardb->currentText());
        displayInfos();
    }
}

void View::setBufferSize(int bufferSize) {
    int index = m_bsb->currentIndex();
    for (int i = 0; i < m_bsb->count(); ++i) {
        if (bufferSize == m_bsb->itemText(i).toInt())
            index = i;
    }
    setBufferSizeFromIndex(index);
}

void View::setBufferSizeFromIndex(int index) {
    if(index != m_bsb->currentIndex()) {
        m_bsb->setCurrentIndex(index);
        emit bufferSizeChanged(m_bsb->currentText().toInt());
    }
    displayLatency();
}

void View::setSampleRate(int sampleRate) {
    int index = m_srb->currentIndex();
    for (int i = 0; i < m_srb->count(); ++i) {
        if (sampleRate == m_srb->itemText(i).toInt())
            index = i;
    }
    setSampleRateFromIndex(index);
}

void View::setInChannels(int chan) {
  int cur = m_inChan->value();
  if(chan != cur)
  {
    m_inChan->setValue(chan);
    emit inChannelsChanged(chan);
  }
}

void View::setOutChannels(int chan) {
  int cur = m_outChan->value();
  if(chan != cur)
  {
    m_outChan->setValue(chan);
    emit outChannelsChanged(chan);
  }
}

void View::setSampleRateFromIndex(int index) {
    if(index != m_srb->currentIndex()) {
        m_srb->setCurrentIndex(index);
        emit rateChanged(m_srb->currentText().toInt());
    }
    displayLatency();
}

int View::getDriverIndex(QString name) {
    for (int i = 0; i < m_driverb->count(); ++i) {
        if (name == m_driverb->itemText(i))
            return i;
    }
    return -1;
}

void View::setDriver(QString name) {
    int index = getDriverIndex(name);
    if (index != -1)
        setDriverFromIndex(index);
}

void View::setDriverFromIndex(int index) {
    if (index != m_driverb->currentIndex()) {
        m_driverb->setCurrentIndex(index);
        populateCards();
        emit driverChanged(m_driverb->currentText());
    }
}

long View::getDriver() {
    if (m_driverb->currentIndex() != -1) {
        for (const auto &pair : driversMapping) {
            if (pair.second == m_driverb->currentIndex())
                return pair.first;
        }
    }
    return -1;
}

QWidget *View::getWidget() {
    return m_widg;
}

void View::displayLatency() {
    if (m_bsb->currentIndex() != -1 && m_srb->currentIndex() != -1) {
        int bs = m_bsb->currentText().toInt();
        int sr = m_srb->currentText().toInt();
        double latency = (double)(1000 * bs) / sr;
        m_ll->setText(QString::number(latency) + QString(" ms"));
    }
    else {
        m_ll->setText(QString("-"));
    }
}

void View::addDriverOption(long ren) {
    int index = m_driverb->count();
    if (CheckRendererAvailability(ren)) {

        auto& idmap = DriverNameMap();
        auto str = idmap.key(ren);
        if(!str.isEmpty())
        {
            m_driverb->addItem(str);
        }
        driversMapping.find(ren)->second = index;
    }
}

void View::populateDrivers() {
    for(int driver = kPortAudioRenderer; driver <= kNetJackRenderer; ++driver )
        addDriverOption(driver);

    if (m_driverb->count() == 0) {
        m_infol->setText(tr("No driver available"));
    }
    else {
        populateCards();
    }

    connect(m_driverb, SignalUtils::QComboBox_currentIndexChanged_int(),
            this, &View::populateCards);
}

void View::populateCards() {

    disconnect(m_cardb, SignalUtils::QComboBox_currentIndexChanged_int(),
               this, &View::displayInfos);
    disconnect(m_cardb, &QComboBox::currentTextChanged,
               this, &View::cardChanged);

    m_cardb->clear();

    connect(m_cardb, SignalUtils::QComboBox_currentIndexChanged_int(),
               this, &View::displayInfos);
    connect(m_cardb, &QComboBox::currentTextChanged,
               this, &View::cardChanged);

    long ren = getDriver();

    if (ren != -1) {
        long ndev = GetDeviceCount(ren);
        if (ndev > 0) {
            DeviceInfo devinfo;
            for (long i = 0; i < ndev ; ++i) {
                GetDeviceInfo(ren, i, &devinfo);
                m_cardb->addItem(QString(devinfo.fName));
            }
            m_cardb->setCurrentIndex(0);
        }
        displayInfos();
    }
}

void View::displayInfos() {

    QString info = tr("No device selected");
    long ren = getDriver();

    if (ren != -1 && m_cardb->currentIndex() != -1) {
        DeviceInfo devinfo;
        GetDeviceInfo(ren, m_cardb->currentIndex(), &devinfo);

        QTextStream ss{&info};
        ss << "<i>"<< tr("Default buffer size") << ": </i>" << devinfo.fDefaultBufferSize << "<br>";
        ss << "<i>" << tr("Default sample rate") << ": </i>" << devinfo.fDefaultSampleRate << "<br>";
        ss << "<i>" << tr("Max input channels") << ": </i>" << devinfo.fMaxInputChannels << "<br>";
        ss << "<i>" << tr("Max output channels") << ": </i>" << devinfo.fMaxOutputChannels << "<br>";
    }

    m_infol->setText(info);
}

View::~View() {
    delete m_cardb;
    delete m_bsb;
    delete m_srb;
    delete m_ll;
    delete m_infol;
    delete m_driverb;
}

}
}
