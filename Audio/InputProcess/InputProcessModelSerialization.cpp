#include "InputProcessModel.hpp"

template <>
void DataStreamReader::read(const Audio::Input::ProcessModel& proc)
{
    insertDelimiter();
}

template <>
void DataStreamWriter::writeTo(Audio::Input::ProcessModel& proc)
{
    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Input::ProcessModel& proc)
{
}

template <>
void JSONObjectWriter::writeTo(Audio::Input::ProcessModel& proc)
{
}
