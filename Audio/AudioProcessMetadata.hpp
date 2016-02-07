#pragma once


namespace Audio
{
struct ProcessMetadata
{
        static const UuidKey<Process::ProcessFactory>& abstractFactoryKey();

        static QString processObjectName();

        static QString factoryPrettyName();
};
}
