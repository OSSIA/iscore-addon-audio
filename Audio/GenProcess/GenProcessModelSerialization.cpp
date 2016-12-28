#include "GenProcessModel.hpp"

template <>
void DataStreamReader::read(const Audio::Gen::ProcessModel& proc)
{
    insertDelimiter();
}

template <>
void DataStreamWriter::write(Audio::Gen::ProcessModel& proc)
{
    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Gen::ProcessModel& proc)
{
}

template <>
void JSONObjectWriter::write(Audio::Gen::ProcessModel& proc)
{
}
