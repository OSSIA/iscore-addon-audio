#pragma once
#include <iscore/plugins/panel/PanelDelegateFactory.hpp>

namespace Audio {
namespace Panel {

class TrackListPanelFactory final :
        public iscore::PanelDelegateFactory
{
    ISCORE_CONCRETE("51fb68e6-d7ed-48df-95c5-09c7482da61f")

    std::unique_ptr<iscore::PanelDelegate> make(
            const iscore::ApplicationContext& ctx) override;
};

}
}
