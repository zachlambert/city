#pragma one

#include "world/builder.h"
#include "asset/mesh.h"
#include "maths/grid.h"


struct CityBounds {
    glm::vec2 centre;
    glm::vec2 size;
    float max_height;
};

class CityBuilder: public AreaBuilder {
private:
    struct Cell {
        bool road;
        std::array<bool, 4> conns;
        Cell():
            road(false)
        {
            std::fill(conns.begin(), conns.end(), false);
        }
        bool& conn(const GridIndex& index)
        {
            // [ (1, 0), (0, 1), (-1, 0), (0, -1) ]
            if (index.x != 0) {
                return conns[1 - index.x];
            } else if (index.y != 0) {
                return conns[2 - index.y];
            } else {
                assert(false);
                return conns[0];
            }
        }
        const bool& conn(const GridIndex& index)const
        {
            return const_cast<Cell*>(this)->conn(index);
        }
        static size_t num_offsets() { return 4; }
        static GridIndex get_offset(size_t i)
        {
            // [ (1, 0), (0, 1), (-1, 0), (0, -1) ]
            if (i%2 == 0) {
                return GridIndex(1 - 2 * ((int)i / 2), 0);
            } else {
                return GridIndex(0, 1 - 2 * ((int)i / 2));
            }
        }
    };

public:
    CityBuilder(const YAML::Node& config_):
        config(config_["city"])
    {
        args.centre = glm::vec2(0, 0);

        auto size_arr = config["size"];
        assert(size_arr.size() == 2);
        for (size_t i = 0; i < size_arr.size(); i++) {
            args.size[i] = size_arr[i].as<float>();
        }

        args.max_height = config["max_height"].as<float>();
        args.cell_width = config["cell_width"].as<float>();
        args.road_width = config["road_width"].as<float>();

        auto road_color_arr = config["road_color"];
        assert(road_color_arr.size() == 4);
        for (size_t i = 0; i < road_color_arr.size(); i++) {
            args.road_color[i] = road_color_arr[i].as<float>();
        }

        args.num_inner_roads = config["num_inner_roads"].as<size_t>();
    }
    
    void generate(World& world, TerrainRenderer& terrain_renderer)override
    {
        GridIndex index;
        cells.resize(GridIndex(args.size / args.cell_width));

        generate_outer_road();
        int num_inner_roads = 0;
        int iter = 0;
        while (num_inner_roads < args.num_inner_roads && iter < args.num_inner_roads * 5) {
            num_inner_roads += generate_inner_road();
            iter++;
        }

        write_world(world, terrain_renderer);

        // TODO: Add world connections too
    }

private:
    void generate_outer_road()
    {
        GridIndex pos(0, 0);
        GridIndex dir(1, 0);
        do {
            while (cells.valid(pos)) {
                cells[pos].road = true;
                if (cells.valid(pos - dir)) {
                    cells[pos].conn(-dir) = true;
                }
                if (cells.valid(pos + dir)) {
                    cells[pos].conn(dir) = true;
                }
                pos += dir;
            }
            pos -= dir;
            dir = dir.rotate(1);
        } while (dir != GridIndex(1, 0));
    }

    int generate_inner_road()
    {
        int num_roads = 0;

        GridIndex dir(1, 0);
        {
            int n = rand() % 4;
            for (int i = 0; i < n; i++) {
                dir = dir.rotate(1);
            }
        }

        GridIndex pos;
        if (dir.x == 0) {
            pos.x = 1 + rand() % (cells.shape().x - 2);
            pos.y = dir.y == 1 ? 0 : cells.shape().y - 1;
        } else {
            pos.x = dir.x == 1 ? 0 : cells.shape().x - 1;
            pos.y = 1 + rand() % (cells.shape().y - 2);
        }

        // TODO: Arg
        float chance_cross_roads = 0.3;

        bool started = false;
        while (cells.valid(pos)) {
            if (!started) {
                if (!cells[pos].road) {
                    pos += dir;
                    continue;
                }
                // Stop if at the edge
                if (!cells.valid(pos + dir)) break;
                // Stop if there is already road connection here
                if (cells[pos + dir].conn(-dir)) break;

                num_roads++;
                started = true;
                cells[pos].conn(dir) = true;
                pos += dir;
            }
            cells[pos].conn(-dir) = true;
            if (cells[pos].road) {
                started = false;
                if (rand_float() < chance_cross_roads) {
                    continue;
                }
                GridIndex original_dir = dir;
                dir = dir.rotate(-1 + 2 * rand()%2);
                if (!cells[pos].conn(dir)) {
                    dir = -dir;
                    if (!cells[pos].conn(dir)) {
                        break;
                    }
                }
                while (cells[pos].conn(dir)) {
                    bool terminate = rand_float() < 0.2;
                    if (!terminate) {
                        pos += dir;
                        continue;
                    }
                    dir = original_dir;
                    continue;
                }
                // At the end without branching, continue in this direction
                continue;
            }
            cells[pos].road = true;
            if (cells.valid(pos + dir)) {
                cells[pos].conn(dir) = true;
            }
            pos += dir;
        }
        return num_roads;
    }

    void write_world(World& world, TerrainRenderer& terrain_renderer)
    {
        std::vector<MeshVertex> vertices;
        std::vector<unsigned short> indices;
        GridIndex pos;
        for (pos.x = 0; pos.x < cells.shape().x; pos.x++) {
            for (pos.y = 0; pos.y < cells.shape().y; pos.y++) {
                if (!cells[pos].road) continue;

                glm::vec2 centre_xy = args.centre + args.cell_width * (
                    (glm::vec2)pos - glm::vec2(cells.shape())/2.f - glm::vec2(0.5, 0.5));

                Region region;
                region.centre = glm::vec3(centre_xy.x, centre_xy.y, 0);
                region.visible = true;
                int index = world.regions.size();
                world.regions.push_back(region);

                generate_road_mesh(vertices, indices, region.centre, cells[pos]);

                terrain_renderer.set_region(index);
                terrain_renderer.load_mesh(vertices, indices);
            }
        }
    }

    void generate_road_mesh(
        std::vector<MeshVertex>& vertices,
        std::vector<unsigned short>& indices,
        const glm::vec3& centre,
        const Cell& cell)
    {
        vertices.clear();indices.clear();
        append_plane(vertices, indices,
                args.road_color,
                glm::vec3(0, 0, 1),
                glm::vec3(1, 0, 0),
                args.road_width,
                args.road_width,
                centre);

        float conn_length = args.cell_width - args.road_width/2;
        for (size_t i = 0; i < Cell::num_offsets(); i++) {
            auto offset = Cell::get_offset(i);
            if (!cell.conn(offset)) continue;

            glm::vec3 u1(offset.x, offset.y, 0);
            append_plane(vertices, indices,
                args.road_color,
                glm::vec3(0, 0, 1),
                u1,
                conn_length,
                args.road_width,
                centre + u1 * (args.road_width/2 + conn_length/2));
        }
    }

    YAML::Node config;

    struct Args {
        glm::vec2 centre;
        glm::vec2 size;
        float max_height;
        float cell_width;
        float road_width;
        glm::vec4 road_color;
        size_t num_inner_roads;
    } args;

    Grid<Cell> cells;
};
