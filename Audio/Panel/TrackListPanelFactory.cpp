#include "TrackListPanel.hpp"
#include "TrackListPanelFactory.hpp"

namespace Audio {
namespace Panel {

std::unique_ptr<score::PanelDelegate> TrackListPanelFactory::make(
        const score::GUIApplicationContext &ctx)
{
    return std::make_unique<TrackListPanel>(ctx);
}

}
}
