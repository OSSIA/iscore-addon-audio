#include "SoundProcessModel.hpp"

template <>
void DataStreamReader::read(const Audio::Sound::ProcessModel& proc)
{
    m_stream << proc.m_file;

    insertDelimiter();
}

template <>
void DataStreamWriter::write(Audio::Sound::ProcessModel& proc)
{
    m_stream >> proc.m_file;

    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Sound::ProcessModel& proc)
{
    obj["File"] = proc.file().name();
}

template <>
void JSONObjectWriter::write(Audio::Sound::ProcessModel& proc)
{
    proc.m_file.load(obj["File"].toString());
}
