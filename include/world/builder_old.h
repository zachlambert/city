#pragma once

#include "world/world.h"
#include "system/terrain_renderer.h"
#include <memory>
#include <concepts>


class RegionBuilder;

struct BuildNode {
    std::shared_ptr<RegionBuilder> region_builder;
    std::vector<int> children;
    int region;
    BuildNode():
        region(-1)
    {}
};

class Builder;
class RegionBuilder {
public:
    RegionBuilder(Builder& builder, int node):
        builder(builder),
        node(node)
    {}

    virtual void generate_mesh(
        std::vector<MeshVertex>& vertices,
        std::vector<unsigned short>& indices) = 0;

protected:
    Builder& builder;
    const int node;

private:
    friend class Builder;
};

template <typename T>
concept region_builder_t = requires(T v)
{
    { std::is_base_of_v<RegionBuilder, T> };
};

class Builder {
public:
    Builder();
    template <region_builder_t T>
    std::shared_ptr<T> create_root()
    {
        assert(nodes.empty());
        auto region_builder = std::make_shared<T>(*this, 0);
        nodes[0].region_builder = region_builder;
        next_node = 1;
        return region_builder;
    }
    template <region_builder_t T>
    std::shared_ptr<T> create_region(int node)
    {
        nodes.at(node).children.push_back(next_node);
        auto region_builder = std::make_shared<T>(*this, next_node);
        nodes[next_node].region_builder = region_builder;
        next_node++;
        return region_builder;
    }
    void finish(World& world, TerrainRenderer& terrain_renderer);

private:
    int next_node;
    std::unordered_map<int, BuildNode> nodes;
};

class RoadBuilder: public RegionBuilder {
public:
    RoadBuilder(Builder& builder, int node);

    glm::vec2 lower; // Two corners
    glm::vec2 upper;
    glm::vec4 color;

private:
    void generate_mesh(
        std::vector<MeshVertex>& vertices,
        std::vector<unsigned short>& indices)override;

    // TODO: Junction children
};

class CityBuilder: public RegionBuilder {
public:
    CityBuilder(Builder& builder, int node);

    glm::vec2 centre;
    glm::vec2 size;
    float road_width; // including pavement
    float outer_padding; // padding between ring road and full size
                         //
    std::vector<std::shared_ptr<RoadBuilder>> roads;

    void create_children();

private:
    void generate_mesh(
        std::vector<MeshVertex>& vertices,
        std::vector<unsigned short>& indices)override;
};

