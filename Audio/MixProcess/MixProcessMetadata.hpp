#pragma once
#include <Process/ProcessMetadata.hpp>
#include <score_plugin_audio_export.h>

namespace Audio
{
namespace Mix
{
class ProcessModel;
}
}

PROCESS_METADATA(
        SCORE_PLUGIN_AUDIO_EXPORT,
        Audio::Mix::ProcessModel,
        "8d2eb884-398c-4812-a777-dc457f898106",
        "Mix",
        "Mix"
        )


UNDO_NAME_METADATA(EMPTY_MACRO, Audio::Mix::ProcessModel, "Mix")
