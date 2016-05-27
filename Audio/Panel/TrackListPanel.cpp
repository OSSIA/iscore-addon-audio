#include "TrackListPanel.hpp"
#include <iscore/widgets/QmlContainerPanel.hpp>
#include "TrackModel.hpp"
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <iscore/document/DocumentContext.hpp>
#include <core/presenter/DocumentManager.hpp>
#include <core/document/Document.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>

#include <Audio/Commands/AddTrack.hpp>
namespace Audio {
namespace Panel {

TrackListPanel::TrackListPanel(const iscore::ApplicationContext &ctx):
    iscore::PanelDelegate{ctx},
    m_widget{new QWidget}
{
    m_layout = new QVBoxLayout;
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

void TrackListPanel::on_modelChanged(maybe_document_t oldm, maybe_document_t newm)
{
    if(!newm) return;
    auto& audioplug = newm->plugin<AudioStreamEngine::DocumentPlugin>();
    TrackModel& tm = audioplug.trackModel();

    m_layout->removeWidget(m_containerpanel);
    if(m_containerpanel) m_containerpanel->setParent(nullptr);
    m_containerpanel = tm.m_containerpanel;
    m_layout->addWidget(m_containerpanel);m_containerpanel->show();
    tm.m_containerpanel->setBaseSize(m_widget->size());
}

}
}
