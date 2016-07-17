#include "InputProcessModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::Input::ProcessModel& proc)
{
    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Input::ProcessModel& proc)
{
    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::Input::ProcessModel& proc)
{
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Input::ProcessModel& proc)
{
}
