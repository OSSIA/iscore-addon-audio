#include <Audio/MixProcess/MixProcessModel.hpp>
#include <DummyProcess/DummyLayerModel.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
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
    pluginModelList = new iscore::ElementPluginModelList{
                      iscore::IDocument::documentContext(*parent),
                      this};

    init();
}

ProcessModel::ProcessModel(
        const ProcessModel& source,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{
        source.duration(),
        id,
        Metadata<ObjectKey_k, ProcessModel>::get(),
        parent}
{
    pluginModelList = new iscore::ElementPluginModelList{
                      *source.pluginModelList,
            this};
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

void ProcessModel::updateRouting(const Routing & r)
{
    auto it = m_routings.find(r);
    ISCORE_ASSERT(it != m_routings.end());

    m_routings.modify(it, [&] (auto& obj) { obj.mix = r.mix; });

    emit routingChanged();
}

void ProcessModel::updateDirectMix(const DirectMix & dmx)
{
    // TODO multi_index to the rescue...
    auto in_dmx = findDirectMix(dmx);
    in_dmx->mix = dmx.mix;
    emit routingChanged();
}

ProcessModel* ProcessModel::clone(
        const Id<Process::ProcessModel>& newId,
        QObject* newParent) const
{
    return new ProcessModel{*this, newId, newParent};
}

QString ProcessModel::prettyName() const
{
    return "Mix Process";
}

QByteArray ProcessModel::makeLayerConstructionData() const
{
    return {};
}

void ProcessModel::setDurationAndScale(const TimeValue& newDuration)
{
    setDuration(newDuration);
}

void ProcessModel::setDurationAndGrow(const TimeValue& newDuration)
{
    setDuration(newDuration);
}

void ProcessModel::setDurationAndShrink(const TimeValue& newDuration)
{
    setDuration(newDuration);
}

void ProcessModel::startExecution()
{
}

void ProcessModel::stopExecution()
{
}

void ProcessModel::reset()
{
}

ProcessStateDataInterface* ProcessModel::startStateData() const
{
    return nullptr;
}

ProcessStateDataInterface* ProcessModel::endStateData() const
{
    return nullptr;
}

Selection ProcessModel::selectableChildren() const
{
    return {};
}

Selection ProcessModel::selectedChildren() const
{
    return {};
}

void ProcessModel::setSelection(const Selection&) const
{
}

void ProcessModel::serialize_impl(const VisitorVariant& s) const
{
    serialize_dyn(s, *this);
}

Process::LayerModel* ProcessModel::makeLayer_impl(
        const Id<Process::LayerModel>& viewModelId,
        const QByteArray& constructionData,
        QObject* parent)
{
    return new Dummy::DummyLayerModel{*this, viewModelId, parent};
}

Process::LayerModel* ProcessModel::loadLayer_impl(
        const VisitorVariant& vis,
        QObject* parent)
{
    return deserialize_dyn(vis, [&] (auto&& deserializer)
    {
        auto autom = new Dummy::DummyLayerModel{
                        deserializer, *this, parent};

        return autom;
    });
}

Process::LayerModel* ProcessModel::cloneLayer_impl(
        const Id<Process::LayerModel>& newId,
        const Process::LayerModel& source,
        QObject* parent)
{
    return new Dummy::DummyLayerModel{
        safe_cast<const Dummy::DummyLayerModel&>(source),
                *this,
                newId,
                parent};
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
        cst.processes.removed.connect<ProcessModel, &ProcessModel::on_processRemoved>(this);
    }

}

void ProcessModel::on_processAdded(const Process::ProcessModel & proc)
{
    auto proc_id = proc.id();
    if(dynamic_cast<const Scenario::ScenarioModel*>(&proc) ||
       dynamic_cast<const Loop::ProcessModel*>(&proc) ||
       dynamic_cast<const Sound::ProcessModel*>(&proc) ||
       dynamic_cast<const Return::ProcessModel*>(&proc))
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
    else if(auto sfx = dynamic_cast<const Effect::ProcessModel*>(&proc))
    {
        // For now we forbid mixing FX each into another.
        m_fxProcesses.push_back({sfx->id(), 1.0});

        for(const auto& other : m_dataProcesses)
        {
            m_routings.insert(Routing{other.process, proc_id, 1.0});
        }
        for(const auto& send : m_sendProcesses)
        {
            m_routings.insert(Routing{proc_id, send, 1.0});
        }
    }
    else if(auto send = dynamic_cast<const Send::ProcessModel*>(&proc))
    {
        m_sendProcesses.push_back(send->id());

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

    emit routingChanged();
}

void ProcessModel::on_processRemoved(const Process::ProcessModel & proc)
{
    auto proc_id = proc.id();
    if(dynamic_cast<const Scenario::ScenarioModel*>(&proc) ||
       dynamic_cast<const Loop::ProcessModel*>(&proc) ||
       dynamic_cast<const Return::ProcessModel*>(&proc) ||
       dynamic_cast<const Sound::ProcessModel*>(&proc))
    {
        auto it = find(m_dataProcesses, proc_id);
        if(it != m_dataProcesses.end())
            m_dataProcesses.erase(it);
    }
    else if(auto sfx = dynamic_cast<const Effect::ProcessModel*>(&proc))
    {
        auto it = find(m_fxProcesses, proc_id);
        if(it != m_fxProcesses.end())
            m_fxProcesses.erase(it);
    }
    else if(auto send = dynamic_cast<const Send::ProcessModel*>(&proc))
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

    emit routingChanged();
}
}

}
