#pragma once
#include <score/plugins/panel/PanelDelegate.hpp>

class QWidget;
class QMLContainerPanel;
class QVBoxLayout;

namespace Audio {
namespace Panel {

class TrackModel;
class Track;

class TrackListPanel final: public score::PanelDelegate
{
    public:
        TrackListPanel(const score::GUIApplicationContext &ctx);

    private:
        QWidget* widget() override;

        const score::PanelStatus& defaultPanelStatus() const override;

        void on_modelChanged(
                score::MaybeDocument oldm,
                score::MaybeDocument newm) override;

        QWidget* m_widget{};
        TrackModel* m_trackModel{};
        QVBoxLayout* m_layout{};
        QMLContainerPanel* m_containerpanel {};
};

}
}
