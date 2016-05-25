#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <QJsonObject>
#include <QJsonValue>

#include <Audio/Panel/Track.hpp>

template <typename T> class Reader;
template <typename T> class Writer;

template<>
void Visitor<Reader<DataStream>>::readFrom(
        const Audio::Panel::Track& t)
{
    m_stream << t.vol();
    m_stream << t.pan();
    m_stream << t.out();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Panel::Track& t)
{
    double dbuf;
    m_stream >> dbuf;
    t.setVol(dbuf);
    m_stream >> dbuf;
    t.setPan(dbuf);
    int32_t lbuf;
    m_stream >> lbuf;
    t.setOut(lbuf);
}

template<>
void Visitor<Reader<JSONObject>>::readFrom(
        const Audio::Panel::Track& t)
{
    m_obj["Volume"] = t.vol();
    m_obj["Pan"] = t.pan();
    m_obj["Output"] = t.out();
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Panel::Track& t)
{
    t.setVol(m_obj["Volume"].toDouble());
    t.setPan(m_obj["Pan"].toDouble());
    t.setOut(m_obj["Output"].toInt());
}
