#pragma once
#include <Process/ProcessMetadata.hpp>
#include <iscore_plugin_audio_export.h>

namespace Process
{
class ProcessFactory;
}

namespace Audio
{
namespace SoundProcess
{
class ProcessModel;
}
}

PROCESS_METADATA(
        ISCORE_PLUGIN_AUDIO_EXPORT,
        Audio::SoundProcess::ProcessModel,
        "e4cd0c16-0b5a-4d75-b8f4-067f52bb4c50",
        "Audio",
        "Audio"
        )


UNDO_NAME_METADATA(EMPTY_MACRO, Audio::SoundProcess::ProcessModel, "Audio")
