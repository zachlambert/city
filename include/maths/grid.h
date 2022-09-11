#pragma once

#include <vector>
#include <glm/glm.hpp>


struct GridIndex {
    int x;
    int y;
    GridIndex(): x(0), y(0) {}
    GridIndex(int x, int y): x(x), y(y) {}

    GridIndex rotate(int sign)
    {
        GridIndex result;
        result.x= -y * sign;
        result.y = x * sign;
        return result;
    }

    explicit GridIndex(const glm::vec2 vec)
    {
        x = floor(vec.x);
        y = floor(vec.y);
    }
    explicit operator glm::vec2()
    {
        return glm::vec2(x, y);
    }
};
inline GridIndex& operator+=(GridIndex& lhs, const GridIndex& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
inline GridIndex operator+(GridIndex lhs, const GridIndex& rhs)
{
    lhs += rhs;
    return lhs;
}
inline GridIndex& operator-=(GridIndex& lhs, const GridIndex& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}
inline GridIndex operator-(GridIndex lhs, const GridIndex& rhs)
{
    lhs -= rhs;
    return lhs;
}
inline GridIndex operator-(GridIndex op)
{
    op.x = -op.x;
    op.y = -op.y;
    return op;
}
inline bool operator==(const GridIndex& lhs, const GridIndex& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T>
class Grid {
public:
    Grid():
        shape_(0, 0)
    {}
    Grid(GridIndex shape):
        data(shape.x * shape.y),
        shape_(shape)
    {}
    void resize(GridIndex shape)
    {
        this->shape_ = shape;
        data.resize(shape.x * shape.y);
    }

    T& operator[](GridIndex index)
    {
        return data[index.y * shape_.x + index.x];
    }
    const T& operator[](GridIndex index)const
    {
        return data[index.y * shape_.x + index.x];
    }

    GridIndex shape()const
    {
        return shape_;
    }

    bool valid(const GridIndex& index)
    {
        return
            index.x >= 0 && index.x < shape_.x
            && index.y >= 0 && index.y < shape_.y;
    }

private:
    std::vector<T> data;
    GridIndex shape_;
};

