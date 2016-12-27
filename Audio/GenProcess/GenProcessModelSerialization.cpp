#include "GenProcessModel.hpp"

template <>
void DataStreamReader::read(const Audio::Gen::ProcessModel& proc)
{
    insertDelimiter();
}

template <>
void DataStreamWriter::writeTo(Audio::Gen::ProcessModel& proc)
{
    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Gen::ProcessModel& proc)
{
}

template <>
void JSONObjectWriter::writeTo(Audio::Gen::ProcessModel& proc)
{
}
