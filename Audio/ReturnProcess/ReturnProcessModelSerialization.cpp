#include "ReturnProcessModel.hpp"
#include <iscore/model/path/PathSerialization.hpp>
#include <Audio/SendProcess/SendProcessModel.hpp>

template <>
void DataStreamReader::read(const Audio::Return::ProcessModel& proc)
{
    m_stream << proc.m_sendPath;
    insertDelimiter();
}

template <>
void DataStreamWriter::writeTo(Audio::Return::ProcessModel& proc)
{
    m_stream >> proc.m_sendPath;
    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Return::ProcessModel& proc)
{
    obj["Send"] = toJsonObject(proc.m_sendPath.unsafePath());
}

template <>
void JSONObjectWriter::writeTo(Audio::Return::ProcessModel& proc)
{
    ObjectPath p = fromJsonObject<ObjectPath>(obj["Send"]);
    using path_t = Path<Audio::Send::ProcessModel>;
    proc.m_sendPath = path_t{std::move(p), path_t::UnsafeDynamicCreation{}};
}
