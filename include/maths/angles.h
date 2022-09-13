#pragma once

#include <vector>
#define DEFINE_MATH_CONSTANTS
#include <cmath>
#include <tuple>

float clamp_angle(float angle);
float clamp_angle_positive(float angle);

class AngleInterval {
public:
    AngleInterval();
    AngleInterval(float centre, float range);
    bool contains(float angle)const;
    bool intersects(const AngleInterval& other)const;
    std::tuple<int, AngleInterval> remove(const AngleInterval& cut);
    float max()const;
private:
    float min;
    float range;
    bool full;
    friend class AngleSet;
};

class AngleSet {
public:
    AngleSet();
    float get(float t, float padding = 0);
    bool intersects(const AngleInterval& query);
    void remove(AngleInterval cut);
    void cull(float min_range);
    bool empty()const;
private:
    std::vector<AngleInterval> intervals;
};

