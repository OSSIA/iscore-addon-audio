#include "EnvelopeEffectModel.hpp"
#include <Audio/AudioStreamEngine/AudioApplicationPlugin.hpp>
#include <Audio/AudioStreamEngine/Streams/AudioStreamIScoreExtensions.h>
#include <QUrl>
#include <QFile>

namespace Audio
{
namespace Effect
{

EnvelopeEffectModel::EnvelopeEffectModel(
        const QString& path,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
  reload();
}

EnvelopeEffectModel::EnvelopeEffectModel(
        const EnvelopeEffectModel& source,
        const Id<EffectModel>& id,
        QObject* parent):
    EffectModel{id, parent}
{
  reload();
}

void EnvelopeEffectModel::reload()
{
  m_effect = MakeEnvelopeEffect();
  metadata().setLabel(tr("Envelope"));
  restoreParams();

}
}
}

template <>
void DataStreamReader::read(
    const Audio::Effect::EnvelopeEffectModel& eff)
{
  insertDelimiter();
}

template <>
void DataStreamWriter::write(
    Audio::Effect::EnvelopeEffectModel& eff)
{
  checkDelimiter();
}

template <>
void JSONObjectReader::read(
    const Audio::Effect::EnvelopeEffectModel& eff)
{
}

template <>
void JSONObjectWriter::write(
    Audio::Effect::EnvelopeEffectModel& eff)
{
}
