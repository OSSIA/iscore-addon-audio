#pragma once
#include <score/plugins/panel/PanelDelegateFactory.hpp>

namespace Audio {
namespace Panel {

class TrackListPanelFactory final :
        public score::PanelDelegateFactory
{
    SCORE_CONCRETE("51fb68e6-d7ed-48df-95c5-09c7482da61f")

    std::unique_ptr<score::PanelDelegate> make(
            const score::GUIApplicationContext& ctx) override;
};

}
}
