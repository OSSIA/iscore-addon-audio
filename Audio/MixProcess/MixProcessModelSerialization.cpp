#include "MixProcessModel.hpp"


template <>
void DataStreamReader::read(const Audio::Mix::Routing& proc)
{
    m_stream << proc.in << proc.out << proc.mix << proc.enabled;

    insertDelimiter();
}

template <>
void DataStreamWriter::write(Audio::Mix::Routing& proc)
{
    m_stream >> proc.in >> proc.out >> proc.mix >> proc.enabled;

    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Mix::Routing& proc)
{
    obj["In"] = toJsonValue(proc.in);
    obj["Out"] = toJsonValue(proc.out);
    obj["Mix"] = proc.mix;
    obj["Enabled"] = proc.enabled;
}

template <>
void JSONObjectWriter::write(Audio::Mix::Routing& proc)
{
    proc.in = fromJsonValue<Id<Process::ProcessModel>>(obj["In"]);
    proc.out = fromJsonValue<Id<Process::ProcessModel>>(obj["Out"]);
    proc.mix = obj["Mix"].toDouble();
    proc.enabled = obj["Enabled"].toBool();
}

template <>
void DataStreamReader::read(const Audio::Mix::DirectMix& proc)
{
    m_stream << proc.process << proc.mix;

    insertDelimiter();
}

template <>
void DataStreamWriter::write(Audio::Mix::DirectMix& proc)
{
    m_stream >> proc.process >> proc.mix;

    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Mix::DirectMix& proc)
{
    obj["Process"] = toJsonValue(proc.process);
    obj["Mix"] = proc.mix;
}

template <>
void JSONObjectWriter::write(Audio::Mix::DirectMix& proc)
{
    proc.process = fromJsonValue<Id<Process::ProcessModel>>(obj["Process"]);
    proc.mix = obj["Mix"].toDouble();
}



// MOVEME <!>
template<typename T, typename U>
struct TSerializer<DataStream, std::list<T, U>>
{
        static void readFrom(
                DataStream::Serializer& s,
                const std::list<T, U>& obj)
        {
            auto& st = s.stream();
            st << (int32_t) obj.size();
            for(const auto& e : obj)
            {
                st << e;
            }
        }

        static void writeTo(
                DataStream::Deserializer& s,
                std::list<T, U>& obj)
        {
            auto& st = s.stream();
            int32_t n;
            st >> n;
            for(int i = 0; i < n; i++)
            {
                T val;
                st >> val;
                obj.push_back(val);
            }
        }
};

template <>
void DataStreamReader::read(const Audio::Mix::ProcessModel& proc)
{
    m_stream << (int32_t) proc.m_routings.size();
    for(auto& elt : proc.m_routings)
        m_stream << elt;
    m_stream << proc.m_dataProcesses << proc.m_fxProcesses << proc.m_sendProcesses;

    insertDelimiter();
}

template <>
void DataStreamWriter::write(Audio::Mix::ProcessModel& proc)
{
    int32_t nroutings = 0;
    m_stream >> nroutings;
    for(int i = 0; i < nroutings; i++)
    {
        Audio::Mix::Routing r;
        m_stream >> r;
        proc.m_routings.insert(r);
    }
    m_stream >> proc.m_dataProcesses >> proc.m_fxProcesses >> proc.m_sendProcesses;

    checkDelimiter();
}

template<typename T>
QJsonArray toJsonIdArray(const T& array)
{
    QJsonArray arr;

    for(const auto& elt : array)
    {
        arr.append(toJsonValue(elt));
    }

    return arr;
}


static QJsonArray toJsonRoutingArray(const Audio::Mix::RoutingMap& orig)
{
    QJsonArray arr;
    for(const Audio::Mix::Routing& obj : orig)
    {
        arr.push_back(toJsonObject(obj));
    }
    return arr;
}

template <>
void JSONObjectReader::read(const Audio::Mix::ProcessModel& proc)
{
    obj["Routings"] = toJsonRoutingArray(proc.m_routings);
    obj["DataProcesses"] = toJsonArray(proc.m_dataProcesses);
    obj["FxProcesses"] = toJsonArray(proc.m_fxProcesses);
    obj["SendProcesses"] = toJsonIdArray(proc.m_sendProcesses);
}

// MOVEME
template<typename List_T>
void fromJsonToList(QJsonArray&& json_arr, List_T& arr)
{
    Foreach(json_arr, [&] (auto elt)
    {
        typename List_T::value_type obj;
        fromJsonObject(elt.toObject(), obj);
        arr.push_back(obj);
    });
}
template<typename List_T>
void fromJsonToIdList(QJsonArray&& json_arr, List_T& arr)
{
    using val_t = typename List_T::value_type;
    Foreach(json_arr, [&] (auto elt)
    {
        arr.push_back(fromJsonValue<val_t>(elt));
    });
}

static void fromJsonRoutingArray(QJsonArray&& json_arr, Audio::Mix::RoutingMap& arr)
{
    arr.reserve(json_arr.size());
    Foreach(json_arr, [&] (auto elt)
    {
        Audio::Mix::Routing obj;
        fromJsonObject(elt.toObject(), obj);
        arr.insert(obj);
    });
}

template <>
void JSONObjectWriter::write(Audio::Mix::ProcessModel& proc)
{
    fromJsonRoutingArray(obj["Routings"].toArray(), proc.m_routings);
    fromJsonToList(obj["DataProcesses"].toArray(), proc.m_dataProcesses);
    fromJsonToList(obj["FxProcesses"].toArray(), proc.m_fxProcesses);
    fromJsonToIdList(obj["SendProcesses"].toArray(), proc.m_sendProcesses);
}
