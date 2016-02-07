#include "AudioProcessMetadata.hpp"
const UuidKey<Process::ProcessFactory>& Audio::ProcessMetadata::abstractFactoryKey()
{
    static const UuidKey<Process::ProcessFactory>name{"10536311-3b36-4743-bb39-b3229ed5eb6b"};
    return name;
}

QString Audio::ProcessMetadata::processObjectName()
{
    return "Audio";
}

QString Audio::ProcessMetadata::factoryPrettyName()
{
    return QObject::tr("Audio");
}
