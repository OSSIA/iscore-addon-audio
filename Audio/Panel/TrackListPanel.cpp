#include "TrackListPanel.hpp"
#include <iscore/widgets/QmlContainerPanel.hpp>
#include "TrackModel.hpp"
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QWidget>
#include <QVBoxLayout>

namespace Audio {
namespace Panel {

TrackListPanel::TrackListPanel(const iscore::ApplicationContext &ctx):
    iscore::PanelDelegate{ctx},
    m_widget{new QWidget},
    m_trackModel(new TrackModel),
    m_layout{new QVBoxLayout},
    m_containerpanel{new QMLContainerPanel}
{
    m_trackModel->addTrack(Track(100, 0, 0));

    QQuickWidget* container = m_containerpanel->container();
    QQmlEngine* engine = container->engine();
    QQmlContext* rootctxt = engine->rootContext();
    rootctxt->setContextProperty(QString("trackModel"), m_trackModel);

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

void TrackListPanel::on_modelChanged(
        iscore::MaybeDocument oldm,
        iscore::MaybeDocument newm) {

}

}
}
