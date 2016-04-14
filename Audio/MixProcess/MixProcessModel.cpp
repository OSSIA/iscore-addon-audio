#include <Audio/MixProcess/MixProcessModel.hpp>
#include <DummyProcess/DummyLayerModel.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioStreamEngine/AudioDocumentPlugin.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#include <Audio/EffectProcess/EffectProcessModel.hpp>
#include <Loop/LoopProcessModel.hpp>
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

void ProcessModel::updateRouting(const Routing & r)
{
    auto it = m_routings.find(r);
    if(it != m_routings.end())
    {
        m_routings.modify(it, [&] (auto& obj) { obj.mix = r.mix; });

    }
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
    bool isFx = false;
    if(auto scenar = dynamic_cast<const Scenario::ScenarioModel*>(&proc))
    {
        m_dataProcesses.push_back(scenar->id());
    }
    else if(auto loop = dynamic_cast<const Loop::ProcessModel*>(&proc))
    {
        m_dataProcesses.push_back(loop->id());
    }
    else if(auto sound = dynamic_cast<const Sound::ProcessModel*>(&proc))
    {
        m_dataProcesses.push_back(sound->id());
    }
    else if(auto sfx = dynamic_cast<const Effect::ProcessModel*>(&proc))
    {
        m_fxProcesses.push_back(sfx->id());
        isFx = true;
    }
    else if(auto mix = dynamic_cast<const Mix::ProcessModel*>(&proc))
    {
        // TODO submixes ?
        return;
    }
    else
    {
        return;
    }

    if(isFx)
    {
        for(const auto& other : m_dataProcesses)
        {
            m_routings.insert(Routing{other, proc.id(), 1.0});
        }
    }
    else
    {
        for(const auto& fx : m_fxProcesses)
        {
            m_routings.insert(Routing{proc.id(), fx, 1.0});
        }
    }

    emit routingChanged();
}

void ProcessModel::on_processRemoved(const Process::ProcessModel & proc)
{
    bool isFx = false;
    if(auto scenar = dynamic_cast<const Scenario::ScenarioModel*>(&proc))
    {
        auto it = find(m_dataProcesses, scenar->id());
        if(it != m_dataProcesses.end())
            m_dataProcesses.erase(it);
    }
    else if(auto loop = dynamic_cast<const Loop::ProcessModel*>(&proc))
    {
        auto it = find(m_dataProcesses, loop->id());
        if(it != m_dataProcesses.end())
            m_dataProcesses.erase(it);
    }
    else if(auto sound = dynamic_cast<const Sound::ProcessModel*>(&proc))
    {
        auto it = find(m_dataProcesses, sound->id());
        if(it != m_dataProcesses.end())
            m_dataProcesses.erase(it);
    }
    else if(auto sfx = dynamic_cast<const Effect::ProcessModel*>(&proc))
    {
        auto it = find(m_fxProcesses, sfx->id());
        if(it != m_fxProcesses.end())
            m_fxProcesses.erase(it);
        isFx = true;
    }
    else if(auto mix = dynamic_cast<const Mix::ProcessModel*>(&proc))
    {
        // TODO submixes ?
        return;
    }
    else
    {
        return;
    }

    auto routing_member = isFx ? &Routing::out : &Routing::in;

    for (auto it = m_routings.begin(); it != m_routings.end(); )
    {
        if ((*it).*routing_member == proc.id())
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
