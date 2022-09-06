#pragma one

#include "component/component.h"
#include <concepts>
#include <vector>

#include "component/agent.h"
#include "component/camera.h"
#include "component/circle.h"

#include "component/window.h"
#include "component/circle_renderer.h"
#include "component/clock.h"
#include "component/game.h"


template <typename T>
concept component_type = requires(T t)
{
    { std::derived_from<T, Component> };
};

template <component_type ComponentT>
class ComponentList {
public:
    int create()
    {
        int index;
        if (!free.empty()) {
            index = free.back();
            free.pop_back();
        } else {
            index = components.size();
            components.emplace_back();
        }
        components[index].valid = true;
        return index;
    }
    void remove(int index)
    {
        components[index].valid = false;
        free.push_back(index);
    }

    ComponentT& operator[](size_t index) {
        return components[index];
    }
    const ComponentT& operator[](size_t index)const {
        return components[index];
    }
    size_t size()const { return components.size(); }

private:
    std::vector<ComponentT> components;
    std::vector<int> free;
};

#define ADD_LIST(Type, name, plural, inputs)\
ComponentList<Type> plural;\
int create_##name(inputs);\
void remove_##name(int index);\
void update_##plural();

#define ADD_SINGLE(Type, name, inputs)\
Type name;\
void create_##name(inputs);\
void remove_##name();\
void update_##name();

struct State {
    ADD_LIST(Agent, agent, agents, Agent::Args)
    ADD_LIST(Camera, camera, cameras, Camera::Args args)
    ADD_LIST(Circle, circle, circles, Circle::Args)
    ADD_LIST(Hand, hand, hands, Hand::Args)
    ADD_LIST(RigidBody, rigid_body, rigid_bodies, RigidBody::Args)

    ADD_SINGLE(Window, window, void)
    ADD_SINGLE(CircleRenderer, circle_renderer, void)
    ADD_SINGLE(Clock, clock, void)
    ADD_SINGLE(Game, game, void)

    void init();
    void deinit();
    void update();
};

#undef ADD_LIST
#undef ADD_SINGLE
