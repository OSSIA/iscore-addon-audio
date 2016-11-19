#pragma once

#include <lilv/lilvmm.hpp>
#include <lv2/lv2plug.in/ns/ext/options/options.h>

struct LV2HostContext
{
        LilvInstance* current;
        LV2_Feature const * const * features;
        LilvWorld& world;
};

struct LV2EffectContext
{
        LV2HostContext& host;
        Lilv::Plugin plugin;
};
