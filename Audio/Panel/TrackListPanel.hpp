#pragma once
#include <iscore/plugins/panel/PanelDelegate.hpp>
#include <QPushButton>

class QWidget;
class QMLContainerPanel;
class QVBoxLayout;
namespace Audio {
namespace Panel {
using maybe_document_t = iscore::MaybeDocument;

class TrackModel;
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

        QMLContainerPanel* m_containerpanel {};
        QWidget* m_widget{};
        QVBoxLayout* m_layout{};
        TrackModel* m_trackModel{};
};

}
}
