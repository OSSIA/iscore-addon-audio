#include "AudioFactory.hpp"
#include <Process/LayerModel.hpp>
#include <DummyProcess/DummyLayerPanelProxy.hpp>
#include <Audio/WavBlock.hpp>
namespace Audio
{
class LayerModel;
}

template<>
ISCORE_LIB_DUMMYPROCESS_EXPORT void Visitor<Reader<DataStream>>::readFrom_impl(
        const Audio::LayerModel& lm)
{
}

template<>
ISCORE_LIB_DUMMYPROCESS_EXPORT void Visitor<Writer<DataStream>>::writeTo(
        Audio::LayerModel& lm)
{
}



template<>
ISCORE_LIB_DUMMYPROCESS_EXPORT void Visitor<Reader<JSONObject>>::readFrom_impl(
        const Audio::LayerModel& lm)
{
}

template<>
ISCORE_LIB_DUMMYPROCESS_EXPORT void Visitor<Writer<JSONObject>>::writeTo(
        Audio::LayerModel& lm)
{
}

namespace Audio
{

LayerModel::LayerModel(
        ProcessModel& model,
        const Id<Process::LayerModel>& id,
        QObject* parent):
    Process::LayerModel{id, "LayerModel", model, parent}
{
}

LayerModel::LayerModel(
        const LayerModel&,
        ProcessModel& model,
        const Id<Process::LayerModel>& id,
        QObject* parent):
    Process::LayerModel{id, "LayerModel", model, parent}
{

}

void LayerModel::serialize(const VisitorVariant& vis) const
{
    serialize_dyn(vis, *this);
}

Process::LayerModelPanelProxy* LayerModel::make_panelProxy(QObject* parent) const
{
    return new Dummy::DummyLayerPanelProxy{*this, parent};
}

LayerView::LayerView(QGraphicsItem* parent):
    Process::LayerView{parent}
{
}

void LayerView::setData(AudioArray data)
{
    m_data = data;
}

void LayerView::recompute(const TimeValue& dur, ZoomRatio ratio)
{
    m_path = QPainterPath{};

    if(m_data.size() == 0)
        return;

    auto& chan = m_data[0];
    if(chan.size() < 1000)
        return;

    std::vector<float> points;
    // Compute 1 point every 10 pixels

    // Get how much data point is 10 pixels :
    // ratio is milliseconds / pixels
    // dur is milliseconds corresponding to width()
    // rate is 44100
    // rate.rate.

    // Each "density" pixels we want to take a new data point
    int density = 10;

    // The duration of 10 pixels is 10 * ratio
    double interval_duration = density * ratio;

    int samples_in_interval = interval_duration * 44100 / 1000;

    int n_samples = std::max(int(width() / density), int(chan.size() / density));
    points.resize(n_samples);
    for(int i = 0; i < n_samples; i ++)
    {
        if(i > points.size() || i * samples_in_interval > chan.size())
            break;
        points[i] = chan[i * samples_in_interval];
    }

    int n_mult = 6;
    m_path.moveTo(0, points[0] * n_mult * height() + height() / 2.);
    for(int i = 1; i < n_samples - 1; i ++)
    {
        m_path.lineTo(i * density, points[i] * n_mult * height() + height() / 2.);
    }

    update();
}

void LayerView::paint_impl(QPainter* painter) const
{
    painter->setBrush(Qt::darkCyan);
    painter->setPen(Qt::darkBlue);

    painter->drawPath(m_path);
    QMatrix m;
    m.scale(-1.0, -1.0);
    painter->setMatrix(m);
    painter->drawPath(m_path);
}

void LayerView::mousePressEvent(QGraphicsSceneMouseEvent*)
{
    emit pressed();
}

class LayerPresenter final :
        public Process::LayerPresenter
{
    public:
        explicit LayerPresenter(
                const LayerModel& model,
                LayerView* view,
                QObject* parent);

        void setWidth(qreal width) override;
        void setHeight(qreal height) override;

        void putToFront() override;
        void putBehind() override;

        void on_zoomRatioChanged(ZoomRatio) override;

        void parentGeometryChanged() override;

        const LayerModel& layerModel() const override;
        const ProcessModel& processModel() const
        { return static_cast<ProcessModel&>(m_layer.processModel()); }
        const Id<Process::ProcessModel>& modelId() const override;

        void fillContextMenu(
                QMenu*,
                const QPoint& pos,
                const QPointF& scenepos) const override;

    private:
        const LayerModel& m_layer;
        LayerView* m_view{};
        FocusDispatcher m_focusDispatcher;
        ZoomRatio m_ratio{1};
};

LayerPresenter::LayerPresenter(
        const LayerModel& model,
        LayerView* view,
        QObject* parent):
    Process::LayerPresenter{"AudioLayerPresenter", parent},
    m_layer{model},
    m_view{view},
    m_focusDispatcher{iscore::IDocument::documentContext(model).document}
{
    connect(view, &LayerView::pressed,
            this, [&] () {
        m_focusDispatcher.focus(this);
    });

    auto& pm = processModel();
    con(pm, &ProcessModel::fileChanged,
        this, [&] () {
        if(auto b = pm.block())
        {
            if(auto wb = dynamic_cast<WavBlock*>(b))
            {
                m_view->setData(wb->m_audio);
                m_view->recompute(pm.duration(), m_ratio);
            }
        }
    });

    if(auto b = pm.block())
    {
        if(auto wb = dynamic_cast<WavBlock*>(b))
        {
            m_view->setData(wb->m_audio);
            m_view->recompute(pm.duration(), m_ratio);
        }
    }
}

void LayerPresenter::setWidth(qreal val)
{
    m_view->setWidth(val);
    m_view->recompute(processModel().duration(), m_ratio);
}

void LayerPresenter::setHeight(qreal val)
{
    m_view->setHeight(val);
    m_view->recompute(processModel().duration(), m_ratio);
}

void LayerPresenter::putToFront()
{
    m_view->recompute(processModel().duration(), m_ratio);
    m_view->show();
}

void LayerPresenter::putBehind()
{
    m_view->hide();
}

void LayerPresenter::on_zoomRatioChanged(ZoomRatio r)
{
    m_ratio = r;
    m_view->recompute(processModel().duration(), m_ratio);
}

void LayerPresenter::parentGeometryChanged()
{
    m_view->recompute(processModel().duration(), m_ratio);
}

const LayerModel& LayerPresenter::layerModel() const
{
    return m_layer;
}

const Id<Process::ProcessModel>& LayerPresenter::modelId() const
{
    return processModel().id();
}

void LayerPresenter::fillContextMenu(
        QMenu*,
        const QPoint&,
        const QPointF&) const
{
}


QString ProcessFactory::prettyName() const
{ // In factory list
    return Metadata<PrettyName_k, ProcessModel>::get();
}

const UuidKey<Process::ProcessFactory>&ProcessFactory::concreteFactoryKey() const
{
    return Metadata<ConcreteFactoryKey_k, ProcessModel>::get();
}

Process::ProcessModel *ProcessFactory::makeModel(
        const TimeValue& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent)
{
    return new ProcessModel{duration, id, parent};
}

QByteArray ProcessFactory::makeStaticLayerConstructionData() const
{
    return {};
}

Process::ProcessModel* ProcessFactory::load(
        const VisitorVariant& vis,
        QObject* parent)
{
    return deserialize_dyn(vis, [&] (auto&& deserializer)
    { return new ProcessModel{deserializer, parent};});
}

Process::LayerPresenter *ProcessFactory::makeLayerPresenter(
        const Process::LayerModel& model,
        Process::LayerView* v,
        QObject* parent)
{
    return new LayerPresenter{
        dynamic_cast<const LayerModel&>(model),
                dynamic_cast<LayerView*>(v),
                parent};
}

Process::LayerView* ProcessFactory::makeLayerView(
        const Process::LayerModel&,
        QGraphicsItem* parent)
{
    return new LayerView{parent};
}

}
