#include "SoundProcessModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::Sound::ProcessModel& proc)
{
    m_stream << proc.m_file;

    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Sound::ProcessModel& proc)
{
    m_stream >> proc.m_file;

    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::Sound::ProcessModel& proc)
{
    m_obj["File"] = toJsonObject(proc.file());
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Sound::ProcessModel& proc)
{
    proc.m_file = fromJsonObject<Audio::MediaFileHandle>(m_obj["File"]);
}
