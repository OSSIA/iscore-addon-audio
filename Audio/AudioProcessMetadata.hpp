#pragma once
#include <Process/ProcessFactoryKey.hpp>

namespace Audio
{
struct ProcessMetadata
{
        static const ProcessFactoryKey& abstractFactoryKey();

        static QString processObjectName();

        static QString factoryPrettyName();
};
}
