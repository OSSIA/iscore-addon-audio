#pragma once
#include <iscore/plugins/panel/PanelDelegate.hpp>

class TrackModel;
class QMLContainerPanel;

namespace Audio {
namespace Panel {

class Track;
class TrackListPanel final: public iscore::PanelDelegate {
public:
    TrackListPanel(const iscore::ApplicationContext &ctx);

private:
    QWidget* widget() override;

    const iscore::PanelStatus& defaultPanelStatus() const override;

    void on_modelChanged(
            maybe_document_t oldm,
            maybe_document_t newm) override;

        QMLContainerPanel* m_widget{};
        TrackModel* m_trackModel{};
};

}
}
