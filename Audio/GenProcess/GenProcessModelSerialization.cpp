#include "GenProcessModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::Gen::ProcessModel& proc)
{
    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Gen::ProcessModel& proc)
{
    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::Gen::ProcessModel& proc)
{
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Gen::ProcessModel& proc)
{
}
