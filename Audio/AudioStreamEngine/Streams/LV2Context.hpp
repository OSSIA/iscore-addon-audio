#pragma once

#include <lilv/lilvmm.hpp>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/ext/data-access/data-access.h>
#include <functional>
#include <atomic>
#include <vector>

struct LV2EffectContext;

struct LV2Nodes
{

};

struct LV2HostContext
{
        LV2EffectContext* current{};
        LV2_Feature const * const * features{};
        LilvWorld& world;


        Lilv::Node make_node(const char* desc) {
            Lilv::Node n{nullptr};
            n.me = lilv_new_uri(&world, desc);
            return n;
        }

        Lilv::Node input_class{make_node(LILV_URI_INPUT_PORT)};
        Lilv::Node output_class{make_node(LILV_URI_OUTPUT_PORT)};
        Lilv::Node control_class{make_node(LILV_URI_CONTROL_PORT)};
        Lilv::Node audio_class{make_node(LILV_URI_AUDIO_PORT)};
        Lilv::Node event_class{make_node(LILV_URI_EVENT_PORT)};
        Lilv::Node midi_class{make_node(LILV_URI_MIDI_EVENT)};
        Lilv::Node cv_class{make_node(LV2_CORE__CVPort)};
        Lilv::Node work_interface{make_node(LV2_WORKER__interface)};
        Lilv::Node work_schedule{make_node(LV2_WORKER__schedule)};
};

struct LV2EffectContext
{
        Lilv::Plugin plugin{nullptr};
        LilvInstance* instance{};
        const LV2_Worker_Interface* worker{};
        LV2_Extension_Data_Feature data{};

        std::function<void()> on_outControlsChanged;
        std::atomic_bool worker_response{false};
        std::vector<char> worker_data;
};
