#include "TrackListPanel.hpp"
#include <iscore/widgets/QmlContainerPanel.hpp>
#include "TrackModel.hpp"
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QWidget>
#include <QVBoxLayout>
#include <iscore/document/DocumentContext.hpp>
#include <core/presenter/DocumentManager.hpp>
#include <core/document/Document.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

namespace Audio {
namespace Panel {

TrackListPanel::TrackListPanel(const iscore::ApplicationContext &ctx):
    iscore::PanelDelegate{ctx},
    m_widget{new QWidget},
    m_layout{new QVBoxLayout},
    m_containerpanel{new QMLContainerPanel}
{


    m_containerpanel->setSource(QString("qrc:/qml/TrackList.qml"));
    m_containerpanel->setContainerSize(m_containerpanel->size());
    m_containerpanel->setObjectName("TrackList");
    m_containerpanel->setBaseSize(m_widget->size());

    m_layout->addWidget(m_containerpanel);
    m_widget->setLayout(m_layout);
}

QWidget *TrackListPanel::widget()
{
    return m_widget;
}

const iscore::PanelStatus& TrackListPanel::defaultPanelStatus() const {
    static const iscore::PanelStatus status {
        true,
        Qt::BottomDockWidgetArea,
                1,
                QObject::tr("TrackList"),
                QKeySequence::fromString("Alt+T")
    };

    return status;
}

void TrackListPanel::on_modelChanged(maybe_document_t oldm, maybe_document_t newm) {
    if(!newm) return;

    delete m_containerpanel;

    m_containerpanel = new QMLContainerPanel{};

    auto& audioplug = newm->plugin<AudioStreamEngine::DocumentPlugin>();
    QQuickWidget* container = m_containerpanel->container();
    QQmlEngine* engine = container->engine();
    QQmlContext* rootctxt = engine->rootContext();

    rootctxt->setContextProperty(QString("trackModel"), &(audioplug.trackModel()));

    m_containerpanel->setSource(QString("qrc:/qml/TrackList.qml"));
    m_containerpanel->setContainerSize(m_containerpanel->size());
    m_containerpanel->setObjectName("TrackList");
    m_containerpanel->setBaseSize(m_widget->size());

    m_layout->addWidget(m_containerpanel);
    m_widget->setLayout(m_layout);
}

}
}
