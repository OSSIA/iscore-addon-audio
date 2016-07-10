#pragma once
#include <OSSIA/LocalTree/Scenario/ProcessComponent.hpp>
#include <OSSIA/LocalTree/NameProperty.hpp>
#include <Audio/EffectProcess/EffectModel.hpp>
#include <Audio/EffectProcess/FaustEffectModel.hpp>

#include <Audio/AudioStreamEngine/Audio/EffectComponent.hpp>
namespace Audio
{
namespace Effect
{
class ProcessModel;

namespace LocalTree
{
class EffectComponent;
class EffectProcessComponent;
class EffectFactoryList;

class EffectComponent : public iscore::Component
{
    public:
        EffectComponent(
                OSSIA::Node& node,
                Effect::EffectModel& proc,
                const Id<iscore::Component>& id,
                const QString& name,
                QObject* parent);

        virtual ~EffectComponent();
        auto& node() const
        { return m_thisNode; }

    protected:
        std::shared_ptr<OSSIA::Node> m_parametersNode;

    private:
        OSSIA::Node& thisNode() const
        { return *node(); }

        std::shared_ptr<OSSIA::Node> m_thisNode;
        OSSIA::CallbackContainer<OSSIA::NodeChangeCallback>::iterator m_callbackIt;
};

class EffectComponentFactory :
        public iscore::GenericComponentFactory<
            Effect::EffectModel,
            Ossia::LocalTree::DocumentPlugin,
            Effect::LocalTree::EffectComponentFactory>
{
        ISCORE_ABSTRACT_FACTORY("c9e1f9bc-b974-4695-a3a8-f797c34858ee")
    public:
        virtual ~EffectComponentFactory();
        virtual EffectComponent* make(
                const Id<iscore::Component>&,
                OSSIA::Node& parent,
                Effect::EffectModel& proc,
                const Ossia::LocalTree::DocumentPlugin& doc,
                QObject* paren_objt) const = 0;
};


class FaustComponent : public EffectComponent
{
        COMPONENT_METADATA("4309e626-e125-4c5c-9e4a-86f2a85cd0a1")

        const Effect::FaustEffectModel& m_proc;
    public:
        FaustComponent(
                OSSIA::Node& node,
                Effect::FaustEffectModel& proc,
                const Id<iscore::Component>& id,
                QObject* parent):
            EffectComponent{node, proc, id, proc.title(), parent},
            m_proc{proc}
        {
            // Create a fake effect on each change,
            // and check its number / range of parameters.

            // When execution starts, the audio execution sets its pointer
            // here.

            // When a parameter gets modified, the audio effect component is modified.

            con(proc, &FaustEffectModel::textChanged,
                this, &FaustComponent::recreate);
            recreate();
        }

        void recreate()
        {
            while(!m_parametersNode->children().empty())
            {
                m_parametersNode->erase(m_parametersNode->children().begin());
            }

            auto fx = m_proc.effect();
            if(!fx)
                return;

            auto count = GetControlCountEffect(fx);
            for(int i = 0; i < count; i++)
            {
                char label[64];
                float min, max, init;

                GetControlParamEffect(fx, i, label, &min, &max, &init);

                std::shared_ptr<OSSIA::Node> param_node = *m_parametersNode->emplace(
                            m_parametersNode->children().end(),
                            label,
                            OSSIA::Value::Type::FLOAT,
                            OSSIA::AccessMode::BI,
                            OSSIA::Domain::create(new OSSIA::Float{min}, new OSSIA::Float{max}));

                // Set value to current value of fx
                auto addr = param_node->getAddress();
                addr->addCallback([=] (const OSSIA::Value& val) {
                    if(val.getType() != OSSIA::Value::Type::FLOAT)
                        return;
                    if(!m_audio_effect)
                        return;

                    auto current_val = static_cast<const OSSIA::Float&>(val).value;
                    SetControlValueEffect(m_audio_effect, i, current_val);
                });
                addr->pushValue(OSSIA::Float{init});
            }
        }

        AudioEffect m_audio_effect{};
};


class FaustComponentFactory :
        public EffectComponentFactory
{
        ISCORE_CONCRETE_FACTORY("4e00eaab-d3f4-46ec-b4dc-bc53bf104064")

    public:
        EffectComponent* make(
                const Id<iscore::Component>&,
                OSSIA::Node& parent,
                Effect::EffectModel& proc,
                const Ossia::LocalTree::DocumentPlugin& doc,
                QObject* paren_objt) const override;

        bool matches(
                EffectModel & m,
                const Ossia::LocalTree::DocumentPlugin &) const override
        {
            return dynamic_cast<FaustEffectModel*>(&m);
        }
};

using EffectComponentFactoryList =
    iscore::GenericComponentFactoryList<
            Effect::EffectModel,
            Ossia::LocalTree::DocumentPlugin,
            Effect::LocalTree::EffectComponentFactory>;


class EffectComponentHierarchyManager : public Nano::Observer
{
    public:
        struct EffectPair {
                Effect::EffectModel& model;
                EffectComponent& component;
        };

        Effect::ProcessModel& process;

        EffectComponentHierarchyManager(
                EffectProcessComponent& component,
                Effect::ProcessModel& cst,
                const Ossia::LocalTree::DocumentPlugin& doc,
                QObject* component_as_parent);

        const std::list<EffectPair>& effects() const
        { return m_children; }

        void add(Effect::EffectModel& process);

        void remove(const Effect::EffectModel& process);

        void remove(const EffectPair& pair);

        void clear();
        ~EffectComponentHierarchyManager();


    private:
        EffectProcessComponent& m_component;
        const LocalTree::EffectComponentFactoryList& m_componentFactory;
        const Ossia::LocalTree::DocumentPlugin& m_system;

        QObject* m_parentObject{};

        std::list<EffectPair> m_children; // todo map ? multi_index with both index of the component and of the process ?
};

class EffectProcessComponent final :
        public Ossia::LocalTree::ProcessComponent_T<ProcessModel>
{
        COMPONENT_METADATA("0313e6df-1a18-4349-a4fb-8bc3461cc6b5")

    public:
       EffectProcessComponent(
               const Id<iscore::Component>& id,
               OSSIA::Node& parent,
               Effect::ProcessModel& scenario,
               Ossia::LocalTree::DocumentPlugin& doc,
               QObject* parent_obj);


       EffectComponent* make_effectComponent(
               const Id<iscore::Component> & id,
               EffectComponentFactory& factory,
               Effect::EffectModel &process,
               const Ossia::LocalTree::DocumentPlugin &system,
               QObject *parent_component);

       void removing(const Effect::EffectModel& cst, const EffectComponent& comp);

       ~EffectProcessComponent();

    private:
        std::shared_ptr<OSSIA::Node> m_effectsNode;

        std::vector<std::unique_ptr<Ossia::LocalTree::BaseProperty>> m_properties;

        EffectComponentHierarchyManager m_hierarchy;
};

using EffectProcessComponentFactory = Ossia::LocalTree::ProcessComponentFactory_T<EffectProcessComponent>;
}
}
}
