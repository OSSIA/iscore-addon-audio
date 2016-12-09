#include "AudioCommandFactory.hpp"
namespace Audio
{
const CommandGroupKey& CommandFactoryName() {
    static const CommandGroupKey key{"Audio"};
    return key;
}
}
