#pragma once

#include <Process/ProcessFactory.hpp>
#include <Audio/ReturnProcess/ReturnProcessModel.hpp>
#include <DummyProcess/DummyLayerPresenter.hpp>
#include <DummyProcess/DummyLayerView.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <Audio/ReturnProcess/ReturnProcessMetadata.hpp>

namespace Audio
{
namespace Return
{
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
                const Process::ProcessPresenterContext& ctx,
                QObject* parent) override;

        Process::LayerView* makeLayerView(
                const Process::LayerModel&,
                QGraphicsItem* parent) override;
};
}
}
