#include "CardSettingsView.hpp"
#include <QLabel>
#include <QComboBox>
#include <QFormLayout>
#include <3rdparty/libaudiostream/src/LibAudioStreamMC++.h>
#include <iscore/application/ApplicationContext.hpp>
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>

namespace Audio {
namespace Settings {

View::View() : View(nullptr) {}

View::View(AudioStreamEngine::ApplicationPlugin & p) : View(&p) {}

View::View(AudioStreamEngine::ApplicationPlugin * p) : m_aseplug{p}, m_widg{new QWidget}, m_driverb(new QComboBox), m_cardb{new QComboBox}, m_bsb{new QComboBox}, m_srb{new QComboBox}, m_ll{new QLabel}, m_infol{new QLabel}, driversMapping(std::map<long, int> ()) {
    auto lay = new QFormLayout;
    m_widg->setLayout(lay);

    m_bsb->addItems(QStringList({QString("32"), QString("48"), QString("64"), QString("128"), QString("256"), QString("512"), QString("1024"), QString("2048")}));
    m_srb->addItems(QStringList({QString("32000"), QString("44100"), QString("48000"), QString("88200"), QString("96000"), QString("192000")}));

    m_infol->setTextFormat(Qt::RichText);

    lay->addRow(tr("Buffer size"), m_bsb);
    lay->addRow(tr("Sample rate (Hz)"), m_srb);
    lay->addRow(tr("Latency"), m_ll);
    lay->addRow(tr("Driver"), m_driverb);
    lay->addRow(tr("Audio device"), m_cardb);
    lay->addRow(tr("Device info"), m_infol);

    connect(m_bsb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &View::bufferSizeChanged);
    connect(m_srb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &View::rateChanged);

    connect(m_driverb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &View::driverChanged);

    connect(m_cardb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &View::displayInfos);
    connect(m_cardb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &View::cardChanged);

    driversMapping.insert(std::pair<long, int> (kPortAudioRenderer, -1));
    driversMapping.insert(std::pair<long, int> (kJackRenderer, -1));
    driversMapping.insert(std::pair<long, int> (kNetJackRenderer, -1));
    driversMapping.insert(std::pair<long, int> (kCoreAudioRenderer, -1));
    driversMapping.insert(std::pair<long, int> (kOffLineAudioRenderer, -1));

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
        m_cardb->setCurrentIndex(idx);
    }
}

void View::setCard(int index) {
    if (index != m_cardb->currentIndex())
        m_cardb->setCurrentIndex(index);
}

void View::setBufferSize(int index) {
    if(index != m_bsb->currentIndex())
        m_bsb->setCurrentIndex(index);
    displayLatency();
}

void View::setSampleRate(int index) {
    if(index != m_srb->currentIndex())
        m_srb->setCurrentIndex(index);
    displayLatency();
}

void View::setDriver(int index) {
    if (index != m_driverb->currentIndex())
        m_driverb->setCurrentIndex(index);
    populateCards();
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
    int bs = m_bsb->currentText().toInt();
    int sr = m_srb->currentText().toInt();
    double latency = (double)(1000 * bs) / sr;
    m_ll->setText(QString::number(latency) + QString(" ms"));
}

void View::addDriverOption(long ren) {
    int index = m_driverb->count();
    if (CheckRendererAvailability(ren)) {
        switch (ren) {
        case kPortAudioRenderer:
            m_driverb->addItem(QString("PortAudio"));
            break;
        case kJackRenderer:
            m_driverb->addItem(QString("JACK"));
            break;
        case kNetJackRenderer:
            m_driverb->addItem(QString("NetJACK"));
            break;
        case kCoreAudioRenderer:
            m_driverb->addItem(QString("CoreAudio"));
            break;
        case kOffLineAudioRenderer:
            m_driverb->addItem(QString("Offline"));
            break;
        default:
            break;
        }
        driversMapping.find(ren)->second = index;
    }
}

void View::populateDrivers() {
    addDriverOption(kPortAudioRenderer);
    addDriverOption(kJackRenderer);
    addDriverOption(kCoreAudioRenderer);
    addDriverOption(kOffLineAudioRenderer);
    addDriverOption(kNetJackRenderer);

    if (m_driverb->count() == 0) {
        m_infol->setText(tr("No driver available"));
    }
    else {
        populateCards();
    }

    connect(m_driverb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &View::populateCards);
}

void View::populateCards() {
    long ren = getDriver();
    if (ren != -1) {
        long ndev = GetDeviceCount(ren);
        DeviceInfo devinfo;
        for (long i = 0; i < ndev ; ++i) {
            GetDeviceInfo(ren, i, &devinfo);
            m_cardb->addItem(QString(devinfo.fName));
        }
        m_cardb->setCurrentIndex(0);

        displayInfos();
    }
}

void View::displayInfos() {

    std::string info = "No devices found";
    long ren = getDriver();

    if (ren != -1) {
        DeviceInfo devinfo;
        GetDeviceInfo(ren, m_cardb->currentIndex(), &devinfo);

        std::stringstream ss;
        ss << "<i>"<< tr("Default buffer size").toStdString() << ": </i>" << devinfo.fDefaultBufferSize << "<br>";
        ss << "<i>" << tr("Default sample rate").toStdString() << ": </i>" << devinfo.fDefaultSampleRate << "<br>";
        ss << "<i>" << tr("Max input channels").toStdString() << ": </i>" << devinfo.fMaxInputChannels << "<br>";
        ss << "<i>" << tr("Max output channels").toStdString() << ": </i>" << devinfo.fMaxOutputChannels << "<br>";

        info = ss.str();

    }

    m_infol->setText(QString(info.c_str()));
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
