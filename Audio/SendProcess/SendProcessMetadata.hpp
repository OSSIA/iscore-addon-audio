#pragma once
#include <Process/ProcessMetadata.hpp>
#include <iscore_plugin_audio_export.h>

namespace Process
{
class ProcessFactory;
}

namespace Audio
{
namespace Send
{
class ProcessModel;
}
}

PROCESS_METADATA(
        ISCORE_PLUGIN_AUDIO_EXPORT,
        Audio::Send::ProcessModel,
        "fcd6e732-3eac-4cda-a870-047e8795905c",
        "Send",
        "Send"
        )


UNDO_NAME_METADATA(EMPTY_MACRO, Audio::Send::ProcessModel, "Send")
