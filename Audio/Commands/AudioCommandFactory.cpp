#include "AudioCommandFactory.hpp"
namespace Audio
{
const CommandParentFactoryKey& CommandFactoryName() {
    static const CommandParentFactoryKey key{"Audio"};
    return key;
}
}
