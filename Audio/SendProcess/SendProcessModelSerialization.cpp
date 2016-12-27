#include "SendProcessModel.hpp"

template <>
void DataStreamReader::read(const Audio::Send::ProcessModel& proc)
{
    insertDelimiter();
}

template <>
void DataStreamWriter::writeTo(Audio::Send::ProcessModel& proc)
{
    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Send::ProcessModel& proc)
{
}

template <>
void JSONObjectWriter::writeTo(Audio::Send::ProcessModel& proc)
{
}
