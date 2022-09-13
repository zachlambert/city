
#include "maths/angles.h"

AngleInterval::AngleInterval():
    min(0),
    range(2 * M_PI),
    full(true)
{}

AngleInterval::AngleInterval(float centre, float range):
    min(clamp_angle(centre - range/2)),
    range(range),
    full(false)
{}

float clamp_angle(float angle)
{
    if (angle < 0) angle += 2 * M_PI;
    angle = std::fmod(angle, 2 * M_PI);
    if (angle > M_PI) angle -= 2 * M_PI;
    return angle;
}

float clamp_angle_positive(float angle)
{
    if (angle < 0) angle += 2 * M_PI;
    return angle;
}

bool AngleInterval::contains(float angle)const
{
    if (full) return true;
    float dist = clamp_angle(angle) - clamp_angle(min);
    return clamp_angle_positive(dist) < range;
}

bool AngleInterval::intersects(const AngleInterval& other)const
{
    if (full) return true;
    return contains(other.min) || contains(other.max()) || other.contains(min) || other.contains(max());
}

std::tuple<int, AngleInterval> AngleInterval::remove(const AngleInterval& cut)
{
    if (full) {
        min = cut.max();
        range = 2 * M_PI - cut.range;
        full = false;
        return { 1, {} };
    }

    if (!intersects(cut)) return { 1, {} };

    bool min_inside = contains(cut.min);
    bool max_inside = contains(cut.max());
    bool inside = cut.range < range && min_inside && max_inside;
    if (inside) {
        AngleInterval extra;
        extra.min = cut.max();
        extra.range = clamp_angle_positive(max() - cut.max());
        range = clamp_angle_positive(cut.min - min);
        return { 2, extra };
    }
    bool fully_remove = cut.range > range && !min_inside && !max_inside;
    if (fully_remove) {
        return { 0, {} };
    }

    if (max_inside) {
        range = clamp_angle_positive(min + range - cut.max());
        min = cut.max();
    }
    if (min_inside) {
        range = clamp_angle_positive(cut.min - min);
    }
    return { 1, {} };
}

float AngleInterval::max()const
{
    return clamp_angle(min + range);
}

AngleSet::AngleSet()
{
    intervals.emplace_back();
}

float AngleSet::get(float t, float padding)
{
    float total_angle = 0;
    for (const auto& interval: intervals) {
        if (interval.range > padding) {
            total_angle += (interval.range - padding);
        }
    }
    float angle = t * total_angle;
    float current = 0;
    for (const auto& interval: intervals) {
        float increase = interval.range - padding;
        if (increase > 0) {
            if (angle < current + increase) {
                return interval.min + padding/2 + (angle - current);
            }
            current += increase;
        }
    }
    return 0;
}

bool AngleSet::intersects(const AngleInterval& query)
{
    for (const auto& interval: intervals) {
        if (query.intersects(interval)) return true;
    }
    return false;
}

void AngleSet::remove(AngleInterval cut)
{
    size_t size = intervals.size();
    for (size_t i = 0; i < size; i++) {
        AngleInterval& interval = intervals[i];
        auto [num, extra_interval] = interval.remove(cut);
        if (num == 1) continue;
        if (num == 0) {
            intervals.erase(intervals.begin() + i);
            size--;
            continue;
        }
        intervals.push_back(extra_interval);
    }
}

void AngleSet::cull(float min_range)
{
    size_t i = 0;
    while (i < intervals.size()) {
        if (intervals[i].range < min_range) {
            intervals.erase(intervals.begin() + i);
            continue;
        }
        i++;
    }
}

bool AngleSet::empty()const
{
    return intervals.empty();
}

std::ostream& operator<<(std::ostream& os, const AngleSet& angle_set)
{
    os << "[";
    for (const auto& interval: angle_set.intervals) {
        os << interval << ", ";
    }
    os << " ]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const AngleInterval& angle_interval)
{
    os << "(" << angle_interval.min << ", " << angle_interval.range << ")";
    return os;
}
