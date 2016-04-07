#include <Audio/SoundProcess/SoundProcessLayer.hpp>
#include <iscore_plugin_audio_export.h>

template<>
void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::Sound::LayerModel& lm)
{
}

template<>
void Visitor<Writer<DataStream>>::writeTo(
        Audio::Sound::LayerModel& lm)
{
}



template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::Sound::LayerModel& lm)
{
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(
        Audio::Sound::LayerModel& lm)
{
}
