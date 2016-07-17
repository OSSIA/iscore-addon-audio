#include "SendProcessModel.hpp"

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Audio::Send::ProcessModel& proc)
{
    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Audio::Send::ProcessModel& proc)
{
    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Audio::Send::ProcessModel& proc)
{
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Audio::Send::ProcessModel& proc)
{
}
