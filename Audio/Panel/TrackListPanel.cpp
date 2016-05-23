#include "TrackListPanel.hpp"
#include <iscore/widgets/QmlContainerPanel.hpp>

namespace Audio {
namespace Panel {

TrackListPanel::TrackListPanel(const iscore::ApplicationContext &ctx):
    iscore::PanelDelegate{ctx},
    m_widget{new QMLContainerPanel} {

    m_widget->setSource(QString("qrc:/qml/TrackList.qml"));
    m_widget->setObjectName("TrackList");
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
                QKeySequence::fromString("Space")
    };

    return status;
}

void TrackListPanel::on_modelChanged(maybe_document_t oldm, maybe_document_t newm) {

}

}
}
