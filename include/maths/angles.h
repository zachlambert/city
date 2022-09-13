#pragma once

#include <vector>
#define DEFINE_MATH_CONSTANTS
#include <cmath>
#include <tuple>
#include <iostream>
#include <concepts>


float clamp_angle(float angle);
float clamp_angle_positive(float angle);

struct AngleInterval {
    float min;
    float range;
    bool full;

    AngleInterval();
    AngleInterval(float centre, float range);
    bool contains(float angle)const;
    bool intersects(const AngleInterval& other)const;
    std::tuple<int, AngleInterval> remove(const AngleInterval& cut);
    float max()const;
};

struct AngleSet {
    std::vector<AngleInterval> intervals;

    AngleSet();
    float get(float t, float padding = 0)const;
    bool contains(float angle)const;
    bool intersects(const AngleInterval& query)const;
    void remove(AngleInterval cut);
    void cull(float min_range);
    bool empty()const;
};

std::ostream& operator<<(std::ostream& os, const AngleSet& angle_set);
std::ostream& operator<<(std::ostream& os, const AngleInterval& angle_interval);
