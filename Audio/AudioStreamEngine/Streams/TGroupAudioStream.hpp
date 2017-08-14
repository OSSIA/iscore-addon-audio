#pragma once

#if defined(LILV_SHARED) // TODO instead add a proper preprocessor macro that also works in static case
#include <3rdparty/libaudiostream/src/TAudioStream.h>
#include <3rdparty/libaudiostream/src/renderer/TAudioRenderer.h>
#include "TExpAudioMixer.h"
#include "TEffectAudioStream.h"
#include "lv2_atom_helpers.hpp"
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>

#include <Midi/MidiExecutor.hpp>
#include <Midi/MidiProcess.hpp>
#include <ModernMIDI/midi_message.h>
#include <set>
#include <iostream>
#include <memory>
#include <cmath>
#include <unordered_map>
#include <ossia/detail/math.hpp>
#include <iscore/tools/Todo.hpp>
#include <ossia/detail/logger.hpp>
// TODO rename this file

#include "LV2Context.hpp"
struct LV2Data
{
        LV2Data(LV2HostContext& h, LV2EffectContext& ctx):
            host{h},
            effect{ctx}
        {
            for(auto res : {effect.plugin.get_required_features(), effect.plugin.get_optional_features()})
            {
                std::cerr << effect.plugin.get_name().as_string() << " requires " << std::endl;
                auto it = res.begin();
                while(it)
                {
                    auto node = res.get(it);
                    if(node.is_uri())
                        std::cerr << "Required uri: " << node.as_uri() << std::endl;
                    it = res.next(it);
                }
                std::cerr << std::endl << std::endl;
            }

            const auto numports = effect.plugin.get_num_ports();
            for(int32_t i = 0; i < numports; i++)
            {
                Lilv::Port port = effect.plugin.get_port_by_index(i);

                std::cerr << "Port : " << lilv_node_as_string(port.get_name())<< std::endl;
                auto cl = port.get_classes();
                auto beg = lilv_nodes_begin(cl);
                while(!lilv_nodes_is_end(cl, beg))
                {
                  auto node = lilv_nodes_get(cl, beg);
                  std::cerr << " --> " << lilv_node_as_string(node) << std::endl;
                  beg = lilv_nodes_next(cl, beg);
                }

                if(port.is_a(host.audio_class))
                {
                    if(port.is_a(host.input_class))
                    {
                        in_ports.push_back(i);
                    }
                    else if(port.is_a(host.output_class))
                    {
                        out_ports.push_back(i);
                    }
                    else
                    {
                        cv_ports.push_back(i);
                        qDebug() << "Audio port not input or output";
                    }
                }
                else if(port.is_a(host.atom_class))
                {
                  // TODO use  atom:supports midi:MidiEvent
                  if(port.is_a(host.input_class))
                  {
                      midi_in_ports.push_back(i);
                  }
                  else if(port.is_a(host.output_class))
                  {
                      midi_out_ports.push_back(i);
                  }
                  else
                  {
                      midi_other_ports.push_back(i);
                  }
                }
                else if(port.is_a(host.cv_class))
                {
                    cv_ports.push_back(i);
                }
                else if(port.is_a(host.control_class))
                {
                    if(port.is_a(host.input_class))
                    {
                        control_in_ports.push_back(i);
                    }
                    else if(port.is_a(host.output_class))
                    {
                        control_out_ports.push_back(i);
                    }
                    else
                    {
                        control_other_ports.push_back(i);
                    }
                }
                else
                {
                    control_other_ports.push_back(i);
                }
            }
        }

        ~LV2Data()
        {

        }

        LV2HostContext& host;
        LV2EffectContext& effect;
        std::vector<int>
          in_ports, out_ports,
          control_in_ports, control_out_ports, control_other_ports,
          midi_in_ports, midi_out_ports, midi_other_ports,
          cv_ports;
};

class LV2AudioEffect : public TAudioEffectInterface
{
    protected:
        LV2Data data;
        std::vector<float> fInControls, fOutControls, fParamMin, fParamMax, fParamInit, fOtherControls;
        std::unordered_map<std::string, int> fLabelsMap;
        std::vector<std::vector<float>> fCVs;
        std::vector<AtomBuffer> fMidiIns, fMidiOuts;

        LilvInstance* fInstance{};
        Midi::Executor::ProcessExecutor* fMidiSource{};
    public:
        LV2AudioEffect(LV2Data dat):
            data{dat}
        {
            const int in_size = data.control_in_ports.size();
            const int out_size = data.control_out_ports.size();
            const int midi_in_size = data.midi_in_ports.size();
            const int midi_out_size = data.midi_out_ports.size();
            const int cv_size = data.cv_ports.size();
            const int other_size = data.control_other_ports.size();
            const int num_ports = data.effect.plugin.get_num_ports();

            qDebug() << "in\t" << in_size << "\n"
                     << "out\t" << out_size << "\n"
                     << "min\t" << midi_in_size << "\n"
                     << "mout\t" << midi_out_size << "\n"
                     << "np\t" << num_ports;

            fInControls.resize(in_size);
            fOutControls.resize(out_size);
            fOtherControls.resize(other_size);
            fCVs.resize(cv_size);
            for(int i = 0; i < cv_size; i++)
            {
                fCVs[i].resize(TAudioGlobals::fBufferSize);
            }

            fParamMin.resize(num_ports);
            fParamMax.resize(num_ports);
            fParamInit.resize(num_ports);

            data.effect.plugin.get_port_ranges_float(fParamMin.data(), fParamMax.data(), fParamInit.data());

            for(int i = 0; i < in_size; i++)
            {
                Lilv::Port p{data.effect.plugin.get_port_by_index(data.control_in_ports[i])};
                Lilv::Node n = p.get_name();
                fLabelsMap.emplace(n.as_string(), i);
            }

            fInstance = lilv_plugin_instantiate(
                        data.effect.plugin.me,
                        TAudioGlobals::fSampleRate,
                        data.host.features);
            data.effect.instance = fInstance;

            if(!fInstance)
                throw std::runtime_error("Error while creating a LV2 plug-in");

            data.effect.data.data_access = lilv_instance_get_descriptor(fInstance)->extension_data;

            // MIDI
            fMidiIns.reserve(midi_in_size);
            for(int i = 0; i < midi_in_size; i++)
            {
              fMidiIns.emplace_back(2048, data.host.atom_chunk_id, data.host.midi_event_id, true);
            }

            fMidiOuts.reserve(midi_out_size);
            for(int i = 0; i < midi_out_size; i++)
            {
              fMidiOuts.emplace_back(2048, data.host.atom_chunk_id, data.host.midi_event_id, false);
            }
            // Worker
            if(lilv_plugin_has_feature(data.effect.plugin.me, data.host.work_schedule)
            && lilv_plugin_has_extension_data(data.effect.plugin.me, data.host.work_interface))
            {
                data.effect.worker =
                        static_cast<const LV2_Worker_Interface*>(
                            lilv_instance_get_extension_data(fInstance, LV2_WORKER__interface));
            }

            for(int i = 0; i < in_size; i++)
            {
                auto port_i = data.control_in_ports[i];
                std::cerr << port_i << ": " << fParamMin[port_i] << " " << fParamMax[port_i] << std::endl;
                fInControls[i] = fParamInit[port_i];
                lilv_instance_connect_port(fInstance, port_i, &fInControls[i]);
            }

            for(int i = 0; i < out_size; i++)
            {
                lilv_instance_connect_port(fInstance, data.control_out_ports[i], &fOutControls[i]);
            }

            for(int i = 0; i < cv_size; i++)
            {
                lilv_instance_connect_port(fInstance, data.cv_ports[i], fCVs[i].data());
            }

            for(int i = 0; i < other_size; i++)
            {
                lilv_instance_connect_port(fInstance, data.control_other_ports[i], &fOtherControls[i]);
            }

            for(int i = 0; i < midi_in_size; i++)
            {
              lilv_instance_connect_port(fInstance, data.midi_in_ports[i], &fMidiIns[i].buf->atoms);
            }

            for(int i = 0; i < midi_out_size; i++)
            {
              lilv_instance_connect_port(fInstance, data.midi_out_ports[i], &fMidiOuts[i].buf->atoms);
            }

            lilv_instance_activate(fInstance);
        }

        void SetMidiSource(Midi::Executor::ProcessExecutor* proc)
        {
          fMidiSource = proc;
        }

        long Inputs() override { return 2; }
        long Outputs() final override { return 2; }

        long GetControlCount() final override
        {
            return data.control_in_ports.size();
        }

        long GetControlOutCount() final override
        {
            return data.control_out_ports.size();
        }

        void GetControlParamImpl(long param, char* label, float* min, float* max, float* init, std::vector<int>& v)
        {
            if(param >= 0 && param < v.size())
            {
                auto port_i = v[param];
                Lilv::Port p = data.effect.plugin.get_port_by_index(port_i);
                Lilv::Node n = p.get_name();
                strcpy(label, n.as_string());
                *min = fParamMin[port_i];
                *max = fParamMax[port_i];
                *init = fParamInit[port_i];
            }
        }

        void GetControlParam(long param, char* label, float* min, float* max, float* init) final override
        {
            GetControlParamImpl(param, label, min, max, init, data.control_in_ports);
        }

        void GetControlOutParam(long param, char* label, float* min, float* max, float* init) final override
        {
            GetControlParamImpl(param, label, min, max, init, data.control_out_ports);
        }

        void SetName(const std::string& name) final override
        {

        }

        std::string GetName() final override
        {
            return data.effect.plugin.get_name().as_string();
        }


        void SetControlValue(long param, float value) final override
        {
            if(param >= 0 && param < (int64_t)data.control_in_ports.size())
            {
                auto param_i = data.control_in_ports[param];
                fInControls[param] = ossia::clamp(value, fParamMin[param_i], fParamMax[param_i]);
            }
        }

        void SetControlValue(const char* label, float value) final override
        {
            auto it = fLabelsMap.find(label);
            if(it != fLabelsMap.end())
                SetControlValue(it->second, value);
        }

        float GetControlValue(long param) final override
        {
            if(param >= 0 && param < (int64_t)data.control_in_ports.size())
                return fInControls[param];
            return {};
        }
        float GetControlValue(const char* label) final override
        {
            auto it = fLabelsMap.find(label);
            if(it != fLabelsMap.end())
                return GetControlValue(it->second);
            return {};
        }

        float GetControlOutValue(long param) final override
        {
            if(param >= 0 && param < (int64_t)data.control_out_ports.size())
                return fOutControls[param];
            return {};
        }

        void Reset() override
        {
        }

        void AllNotesOff()
        {
          if(fMidiSource)
          {
            for(AtomBuffer& port : fMidiIns)
            {
              static float stopbuf_[1];
              static float* stopbuf[2] = { +stopbuf_, +stopbuf_ };
              Iterator it{port.buf};
              for(int i = 0; i < 127; i++)
              {
                auto off = mm::MakeNoteOff(fMidiSource->process().channel(), i, 0);
                it.write(0, 0, data.host.midi_event_id, 3, off.data.data());
              }

              Process(+stopbuf, +stopbuf, 0);
            }
          }
        }

        void preProcess()
        {
          if(fMidiSource)
          {
            for(AtomBuffer& port : fMidiIns)
            {
              Iterator it{port.buf};
              for(auto& note : fMidiSource->timedState.currentAudioStart)
              {
                auto on = mm::MakeNoteOn(fMidiSource->process().channel(), note.first.pitch(), note.first.velocity());
                it.write(note.second, 0, data.host.midi_event_id, 3, on.data.data());
              }

              for(auto& note : fMidiSource->timedState.currentAudioStop)
              {
                auto off = mm::MakeNoteOff(fMidiSource->process().channel(), note.first.pitch(), note.first.velocity());
                it.write(note.second, 0, data.host.midi_event_id, 3, off.data.data());
              }
            }
            fMidiSource->timedState.currentAudioStart.clear();
            fMidiSource->timedState.currentAudioStop.clear();
          }
        }

        void postProcess()
        {
            if(data.effect.worker && data.effect.worker_response && data.effect.worker->work_response )
            {
                data.effect.worker->work_response(
                            data.effect.instance->lv2_handle,
                            data.effect.worker_data.size(),
                            data.effect.worker_data.data());
                data.effect.worker_response = false;
            }

            if(data.effect.on_outControlsChanged && !fOutControls.empty())
            {
                data.effect.on_outControlsChanged();
            }

            if(data.effect.worker && data.effect.worker->end_run)
            {
                data.effect.worker->end_run(data.effect.instance->lv2_handle);
            }


            for(AtomBuffer& port : fMidiIns)
            {
              port.buf->reset(true);
            }
            for(AtomBuffer& port : fMidiOuts)
            {
              port.buf->reset(false);
            }
        }

        ~LV2AudioEffect()
        {
            lilv_instance_deactivate(fInstance);
            lilv_instance_free(fInstance);
        }
};

class StereoLV2AudioEffect final : public LV2AudioEffect
{
    public:
        StereoLV2AudioEffect(LV2Data dat):
            LV2AudioEffect{std::move(dat)}
        {
        }

    private:
        void Process(float** input, float** output, long framesNum) override
        {
            if(framesNum <= 0)
                return;

            data.host.current = &data.effect;
            preProcess();

            lilv_instance_connect_port(fInstance, data.in_ports[0], input[0]);
            lilv_instance_connect_port(fInstance, data.in_ports[1], input[1]);
            lilv_instance_connect_port(fInstance, data.out_ports[0], output[0]);
            lilv_instance_connect_port(fInstance, data.out_ports[1], output[1]);

            lilv_instance_run(fInstance, framesNum);

            postProcess();
        }

        TAudioEffectInterface* Copy() override
        {
            return nullptr;
        }
};

class MonoLV2AudioEffect final : public LV2AudioEffect
{
    public:

        MonoLV2AudioEffect(LV2Data dat):
            LV2AudioEffect{std::move(dat)}
        {
        }

    private:
        void Process(float** input, float** output, long framesNum) override
        {
            if(framesNum <= 0)
                return;

            data.host.current = &data.effect;
            preProcess();

            lilv_instance_connect_port(fInstance, data.in_ports[0], input[0]);
            lilv_instance_connect_port(fInstance, data.out_ports[0], output[0]);

            lilv_instance_run(fInstance, framesNum);

            postProcess();
            std::copy_n(output[0], framesNum, output[1]);
        }

        TAudioEffectInterface* Copy() override
        {
            return nullptr;
        }
};


class StereoLV2AudioInstrument final : public LV2AudioEffect
{
    public:
        StereoLV2AudioInstrument(LV2Data dat):
            LV2AudioEffect{std::move(dat)}
        {
        }

    private:
        long Inputs() final override { return 0; }
        void Process(float** input, float** output, long framesNum) override
        {
            if(framesNum <= 0)
                return;

            data.host.current = &data.effect;
            preProcess();

            lilv_instance_connect_port(fInstance, data.out_ports[0], output[0]);
            lilv_instance_connect_port(fInstance, data.out_ports[1], output[1]);

            lilv_instance_run(fInstance, framesNum);

            postProcess();
        }

        TAudioEffectInterface* Copy() override
        {
            return nullptr;
        }
};

class MonoLV2AudioInstrument final : public LV2AudioEffect
{
    public:

        MonoLV2AudioInstrument(LV2Data dat):
            LV2AudioEffect{std::move(dat)}
        {
        }

    private:
        long Inputs() final override { return 0; }
        void Process(float** input, float** output, long framesNum) override
        {
            if(framesNum <= 0)
                return;

            data.host.current = &data.effect;
            preProcess();

            lilv_instance_connect_port(fInstance, data.out_ports[0], output[0]);

            lilv_instance_run(fInstance, framesNum);

            postProcess();
            std::copy_n(output[0], framesNum, output[1]);
        }

        TAudioEffectInterface* Copy() override
        {
            return nullptr;
        }
};
#endif
/**/
/**
 * @brief The TTimeNodeControlCommand struct
 * Only useful for interactive time nodes.
 * Maybe for events too ?
 */
//struct TTimeNodeControlCommand : public TControlCommand
//{
//        TTimeNodeControlCommand(const Audio::AudioStreamEngine::TimeNodeComponent& ctx):
//            m_audio_parent{ctx}
//        {

//        }

//        const Audio::AudioStreamEngine::TimeNodeComponent& m_audio_parent;

//        //std::vector<SymbolicDate> m_beforeDates;
//        //std::vector<SymbolicDate> m_afterDates;

//        bool Execute(
//                TNonInterleavedAudioBuffer<float>* buffer,
//                map<SymbolicDate, audio_frame_t>& date_map,
//                audio_frame_t cur_frame,
//                long frames)
//        {
//            auto timenode_time = fStartDate->get_date();
//            if (InBuffer(timenode_time, cur_frame, frames))
//            {
//                m_audio_parent.onDateFixed(cur_frame);

//                // Set stop date of all previous constraint to the TN frame
//                for(SymbolicDate date : m_beforeDates)
//                    date->setDate(timenode_time);

//                // Execute the time node.
//                // If we are here, it has already been triggered.
//                // We only have to check the conditions.
//                m_node.trigger();

//                // For all true events, set start date of all next constraints's start, and
//                // trigger their start / their start + offset.
//                for(SymbolicDate date : m_afterDates)
//                    date->setDate(timenode_time + 1);


//                // TODO after each command, the commands should be re-sorted.
//                // TODO the commands should have a priority for hierarchy ?

//                // TODO else find a way to disable' em

//                return false;
//            }
//            else
//            {
//                return true;
//            }
//        }

//};

