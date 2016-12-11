#pragma once
#include <iscore/plugins/panel/PanelDelegate.hpp>

class QWidget;
class QMLContainerPanel;
class QVBoxLayout;

namespace Audio {
namespace Panel {

class TrackModel;
class Track;

class TrackListPanel final: public iscore::PanelDelegate
{
    public:
        TrackListPanel(const iscore::GUIApplicationContext &ctx);

    private:
        QWidget* widget() override;

        const iscore::PanelStatus& defaultPanelStatus() const override;

        void on_modelChanged(
                iscore::MaybeDocument oldm,
                iscore::MaybeDocument newm) override;

        QWidget* m_widget{};
        TrackModel* m_trackModel{};
        QVBoxLayout* m_layout{};
        QMLContainerPanel* m_containerpanel {};
};

}
}
