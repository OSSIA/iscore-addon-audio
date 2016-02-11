#pragma once

#include <Process/ProcessFactory.hpp>
#include <Audio/AudioProcessModel.hpp>
#include <DummyProcess/DummyLayerPresenter.hpp>
#include <DummyProcess/DummyLayerView.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/AudioProcessMetadata.hpp>

namespace Audio
{
class LayerView final : public Process::LayerView
{
        Q_OBJECT
    public:
        explicit LayerView(QGraphicsItem* parent);

        void setData(AudioArray data);
        void recompute(const TimeValue& dur, ZoomRatio ratio);
    signals:
        void pressed();

    private:
        void paint_impl(QPainter*) const override;
        void mousePressEvent(QGraphicsSceneMouseEvent*) override;

        AudioArray m_data;
        QPainterPath m_path;
};
class ProcessFactory final : public Process::ProcessFactory
{
    public:
        QString prettyName() const override;

        const UuidKey<Process::ProcessFactory>& concreteFactoryKey() const override;


        Process::ProcessModel* makeModel(
                const TimeValue& duration,
                const Id<Process::ProcessModel>& id,
                QObject* parent) override;

        QByteArray makeStaticLayerConstructionData() const override;

        Process::ProcessModel* load(
                const VisitorVariant& vis,
                QObject* parent) override;

        Process::LayerPresenter* makeLayerPresenter(
                const Process::LayerModel& model,
                Process::LayerView* v,
                QObject* parent) override;

        Process::LayerView* makeLayerView(
                const Process::LayerModel&,
                QGraphicsItem* parent) override;
};
}
