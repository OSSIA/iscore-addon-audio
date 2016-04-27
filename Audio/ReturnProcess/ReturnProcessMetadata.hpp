#pragma once
#include <Process/ProcessMetadata.hpp>
#include <iscore_plugin_audio_export.h>

namespace Process
{
class ProcessFactory;
}

namespace Audio
{
namespace Return
{
class ProcessModel;
}
}

PROCESS_METADATA(
        ISCORE_PLUGIN_AUDIO_EXPORT,
        Audio::Return::ProcessModel,
        "1a1462af-4fe4-4743-a26e-fc990160416d",
        "Return",
        "Return"
        )


UNDO_NAME_METADATA(EMPTY_MACRO, Audio::Return::ProcessModel, "Return")
