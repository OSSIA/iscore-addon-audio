#include "InputProcessModel.hpp"

template <>
void DataStreamReader::read(const Audio::Input::ProcessModel& proc)
{
    insertDelimiter();
}

template <>
void DataStreamWriter::write(Audio::Input::ProcessModel& proc)
{
    checkDelimiter();
}

template <>
void JSONObjectReader::read(const Audio::Input::ProcessModel& proc)
{
}

template <>
void JSONObjectWriter::write(Audio::Input::ProcessModel& proc)
{
}
