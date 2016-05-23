#include "TrackListPanel.hpp"
#include "TrackListPanelFactory.hpp"

namespace Audio {
namespace Panel {

std::unique_ptr<iscore::PanelDelegate> TrackListPanelFactory::make(
        const iscore::ApplicationContext &ctx)
{
    return std::make_unique<TrackListPanel>(ctx);
}

}
}
