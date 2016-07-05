#pragma once
#include <Process/ProcessMetadata.hpp>
#include <iscore_plugin_audio_export.h>

namespace Process
{
class ProcessFactory;
}

namespace Audio
{
namespace Gen
{
class ProcessModel;
}
}

PROCESS_METADATA(
        ISCORE_PLUGIN_AUDIO_EXPORT,
        Audio::Gen::ProcessModel,
        "fd9cfca9-5d3a-4846-9459-fee6c2f78c5f",
        "Gen",
        "Gen"
        )


UNDO_NAME_METADATA(EMPTY_MACRO, Audio::Gen::ProcessModel, "Gen")
