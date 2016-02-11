#include "AudioProcessModel.hpp"
#include <DummyProcess/DummyLayerModel.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Audio/AudioBlock.hpp>
#include <Audio/AudioDocumentPlugin.hpp>
#include <iscore/plugins/documentdelegate/plugin/ElementPluginModelList.hpp>
#include <sndfile.hh>
#include <libwatermark/mathutils/math_util.h>

// MOVEME
template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::ProcessModel& proc)
{
    readFrom(*proc.pluginModelList);

    m_stream << proc.m_script;

    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::ProcessModel& proc)
{
    proc.pluginModelList = new iscore::ElementPluginModelList{*this, &proc};

    QString str;
    m_stream >> str;
    proc.setScript(str);

    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::ProcessModel& proc)
{
    m_obj["PluginsMetadata"] = toJsonValue(*proc.pluginModelList);
    m_obj["Script"] = proc.script();
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::ProcessModel& proc)
{
    Deserializer<JSONValue> elementPluginDeserializer(m_obj["PluginsMetadata"]);
    proc.pluginModelList = new iscore::ElementPluginModelList{elementPluginDeserializer, &proc};

    proc.setScript(m_obj["Script"].toString());
}

namespace Audio
{
std::vector<float> ProcessModel::readFile(const QString& filename)
{
    auto myf = SndfileHandle(filename.toStdString());
    switch(myf.channels())
    {
        case 0:
        {
            return {};
            break;
        }
        case 1:
        {
            std::vector<float> vec(myf.frames());
            myf.read(vec.data(), myf.frames());
            return vec;
            break;
        }
        default:
        {
            std::vector<float> vec(myf.frames() * myf.channels());

            int parity = (myf.frames() % 2 != 0) ? -1 : 0;
            for(int i = 0; i < myf.channels(); ++i)
                myf.read(vec.data() + i * (myf.frames() + parity),  myf.frames() + parity);

            std::vector<std::vector<float>> channels =
                    MathUtil::deinterleave(
                        vec,
                        (unsigned int) myf.channels(),
                        (unsigned int) myf.frames());

            return channels[0];
            break;
        }

    }



}

ProcessModel::ProcessModel(
        const TimeValue& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{duration, id, Metadata<ObjectKey_k, ProcessModel>::get(), parent}
{
    pluginModelList = new iscore::ElementPluginModelList{
                      iscore::IDocument::documentContext(*parent),
                      this};

    m_audioFile = "test.wav";
    setScript("phasor(f)   = f/48000 : (+,1.0:fmod) ~ _ ;"
              "process     = phasor(f) * 6.28318530718 : sin;");
}

ProcessModel::ProcessModel(
        const ProcessModel& source,
        const Id<Process::ProcessModel>& id,
        QObject* parent):
    Process::ProcessModel{source.duration(), id, Metadata<ObjectKey_k, ProcessModel>::get(), parent},
    m_script{source.m_script}
{
    pluginModelList = new iscore::ElementPluginModelList{
                      *source.pluginModelList,
                      this};
}

void ProcessModel::setScript(const QString& script)
{
    m_script = script;
    /*
    //auto vec = readFile(m_audioFile);
    //if(vec.size() > 0)
    {
        m_ossia_process->setAudioBlock(std::make_unique<FaustAudioBlock>(
                                           script,
        std::vector<float>{},
                                           m_ossia_process->engine()
                                           ));
    }
    */
}

ProcessModel* ProcessModel::clone(
        const Id<Process::ProcessModel>& newId,
        QObject* newParent) const
{
    return new ProcessModel{*this, newId, newParent};
}

QString ProcessModel::prettyName() const
{
    return "Audio Process";
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
}

