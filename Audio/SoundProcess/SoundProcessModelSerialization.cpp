#include "SoundProcessModel.hpp"

template <>
void DataStreamReader::read(const Audio::Sound::ProcessModel& proc)
{
    m_stream << proc.m_file;

    insertDelimiter();
}

template <>
void DataStreamWriter::writeTo(Audio::Sound::ProcessModel& proc)
{
    m_stream >> proc.m_file;

    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Sound::ProcessModel& proc)
{
    obj["File"] = toJsonObject(proc.file());
}

template <>
void JSONObjectWriter::writeTo(Audio::Sound::ProcessModel& proc)
{
    proc.m_file = fromJsonObject<Audio::MediaFileHandle>(obj["File"]);
}
