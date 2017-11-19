#pragma once
#include <Process/ProcessMetadata.hpp>
#include <score_plugin_audio_export.h>

namespace Audio
{
namespace Effect
{
class ProcessModel;
}
}

PROCESS_METADATA(
        SCORE_PLUGIN_AUDIO_EXPORT,
        Audio::Effect::ProcessModel,
        "d27bc0ed-a93e-434c-913d-ccab0b22b4e8",
        "Effects",
        "Effects"
        )


UNDO_NAME_METADATA(EMPTY_MACRO, Audio::Effect::ProcessModel, "Effects")
