#pragma once
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>

#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>

#include <Audio/Inspector/AudioInspector.hpp>
#include <Audio/Inspector/MixInspector.hpp>
#include <Audio/Inspector/ReturnInspector.hpp>
#include <Audio/Inspector/EffectInspector.hpp>

namespace Audio
{
namespace Sound
{
class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
        ISCORE_CONCRETE("10360b4c-4d73-4274-8339-408644408cab")
};
}

namespace Mix
{
class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
        ISCORE_CONCRETE("60efbf22-8374-4923-8a07-8caa4d242954")
};
}

namespace Return
{
class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
        ISCORE_CONCRETE("a5afd64d-750b-47c4-90ea-9788f47df9b8")
};
}

namespace Effect
{
class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
        ISCORE_CONCRETE("cc8ceff3-ef93-4b73-865a-a9f870d6e898")
};
}
}
