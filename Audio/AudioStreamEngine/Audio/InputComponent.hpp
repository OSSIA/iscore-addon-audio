#pragma once
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>

namespace Audio
{
namespace Input
{
class ProcessModel;
}
namespace AudioStreamEngine
{
class InputComponent final :
        public ProcessComponent_T<Input::ProcessModel, false, true>
{
        COMPONENT_METADATA("54370cd9-0b4a-454e-8dab-1fab48ed5875")

    public:
            InputComponent(
                Input::ProcessModel&,
                DocumentPlugin& doc,
                const Id<iscore::Component>& id,
                QObject* parent_obj);

        void makeStream(const Context& ctx) override;
};

using InputComponentFactory = ProcessComponentFactory_T<InputComponent>;
}
}
