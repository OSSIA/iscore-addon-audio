#include <Audio/MixProcess/MixProcessModel.hpp>
#include <Process/Dummy/DummyLayerModel.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <Audio/AudioStreamEngine/Scenario/ProcessComponent.hpp>
#include <QFile>

namespace Audio
{
namespace Mix
{

ProcessModel::ProcessModel(
        const TimeValue& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{duration, id, Metadata<ObjectKey_k, ProcessModel>::get(), parent}
{
    // TODO connect the removing() signal of the process to here.
    init();
    metadata().setName(Metadata<PrettyName_k, ProcessModel>::get() + QString(".%1").arg(*this->id().val()));
}

ProcessModel::ProcessModel(
        const ProcessModel& source,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{
        source.duration(),
        id,
        Metadata<ObjectKey_k, ProcessModel>::get(),
        parent},
    m_routings{source.m_routings},
    m_dataProcesses{source.m_dataProcesses},
    m_fxProcesses{source.m_fxProcesses},
    m_sendProcesses{source.m_sendProcesses}
{
}

ProcessModel::~ProcessModel()
{

}

double ProcessModel::mix(const Routing & r) const
{
    auto it = m_routings.find(r);
    ISCORE_ASSERT(it != m_routings.end());

    return it->mix;
}

double ProcessModel::mix(const DirectMix & dmx) const
{
    return findDirectMix(dmx)->mix;
}


const double* ProcessModel::mix_ptr(const Routing & r) const
{
    auto it = m_routings.find(r);
    ISCORE_ASSERT(it != m_routings.end());

    return &(it->mix);
}

const double* ProcessModel::mix_ptr(const DirectMix & dmx) const
{
    return &(findDirectMix(dmx)->mix);
}


void ProcessModel::updateRouting(const Routing & r)
{
    auto it = m_routings.find(r);
    ISCORE_ASSERT(it != m_routings.end());

    m_routings.modify(it, [&] (auto& obj) {
        obj.mix = r.mix;
        obj.enabled = r.enabled;
    });

    emit routingChanged(r);
}

void ProcessModel::updateDirectMix(const DirectMix & dmx)
{
    // TODO multi_index to the rescue...
    auto in_dmx = findDirectMix(dmx);
    in_dmx->mix = dmx.mix;
    emit directMixChanged(dmx);
}

void ProcessModel::init()
{
    if(auto ptr_cst = dynamic_cast<Scenario::ConstraintModel*>(parent()))
    {
        auto& cst = *ptr_cst;
        for(auto& proc : cst.processes)
        {
            on_processAdded(proc);
        }

        cst.processes.added.connect<ProcessModel, &ProcessModel::on_processAdded>(this);
        cst.processes.removing.connect<ProcessModel, &ProcessModel::on_processRemoved>(this);
    }
}

void ProcessModel::on_processAdded(const Process::ProcessModel & proc)
{
    auto fac = iscore::AppContext().components.factory<Audio::AudioStreamEngine::ProcessComponentFactoryList>().factory(proc);
    if(!fac)
        return;

    bool in = fac->hasInput();
    bool out = fac->hasOutput();

    auto proc_id = proc.id();
    if(!in && out)
    {
        m_dataProcesses.push_back({proc_id, 1.0});

        for(const auto& fx : m_fxProcesses)
        {
            m_routings.insert(Routing{proc_id, fx.process, 1.0});
        }
        for(const auto& send : m_sendProcesses)
        {
            m_routings.insert(Routing{proc_id, send, 1.0});
        }
    }
    else if(in && out)
    {
        // For now we forbid mixing FX each into another.
        m_fxProcesses.push_back({proc_id, 1.0});

        for(const auto& other : m_dataProcesses)
        {
            m_routings.insert(Routing{other.process, proc_id, 1.0});
        }

        for(const auto& other : m_fxProcesses)
        {
            // By default we disable mixing Fx's into each other
            // to prevent cycles.
            m_routings.insert(Routing{other.process, proc_id, 1.0, false});
            m_routings.insert(Routing{proc_id, other.process, 1.0, false});
        }

        for(const auto& send : m_sendProcesses)
        {
            m_routings.insert(Routing{proc_id, send, 1.0});
        }
    }
    else if(in && !out)
    {
        m_sendProcesses.push_back(proc_id);

        for(const auto& other : m_dataProcesses)
        {
            m_routings.insert(Routing{other.process, proc_id, 1.0});
        }
        for(const auto& other : m_fxProcesses)
        {
            m_routings.insert(Routing{other.process, proc_id, 1.0});
        }
    }
    else
    {
        //  mix
        return;
    }

    emit structureChanged();
}

void ProcessModel::on_processRemoved(const Process::ProcessModel & proc)
{
    auto proc_id = proc.id();
    auto fac = iscore::AppContext().components.factory<Audio::AudioStreamEngine::ProcessComponentFactoryList>().factory(proc);
    if(!fac)
        return;

    bool in = fac->hasInput();
    bool out = fac->hasOutput();
    if(!in && out)
    {
        auto it = find(m_dataProcesses, proc_id);
        if(it != m_dataProcesses.end())
            m_dataProcesses.erase(it);
    }
    else if(in && out)
    {
        auto it = find(m_fxProcesses, proc_id);
        if(it != m_fxProcesses.end())
            m_fxProcesses.erase(it);
    }
    else if(in && !out)
    {
        auto it = find(m_sendProcesses, proc_id);
        if(it != m_sendProcesses.end())
            m_sendProcesses.erase(it);
    }
    else
    {
        // send, mix
        return;
    }

    for (auto it = m_routings.begin(); it != m_routings.end(); )
    {
        if ((*it).in == proc.id() ||
            (*it).out == proc.id())
        {
            it = m_routings.erase(it);
        }
        else
        {
            ++ it;
        }
    }

    emit structureChanged();
}
}

}
