#pragma once
#include <Process/ProcessMetadata.hpp>
#include <iscore_plugin_audio_export.h>

namespace Audio
{
namespace Input
{
class ProcessModel;
}
}

PROCESS_METADATA(
        ISCORE_PLUGIN_AUDIO_EXPORT,
        Audio::Input::ProcessModel,
        "89963256-8e47-4590-bc72-522040f2fa1c",
        "Input",
        "Input"
        )


UNDO_NAME_METADATA(EMPTY_MACRO, Audio::Input::ProcessModel, "Input")
