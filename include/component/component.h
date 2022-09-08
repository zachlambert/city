#pragma once

#include <vector>
#include <concepts>

template <typename T>
concept component_type = requires(T t)
{
    { t.valid } -> std::convertible_to<bool>;
};

template <component_type T>
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

    T& operator[](size_t index) {
        return components[index];
    }
    const T& operator[](size_t index)const {
        return components[index];
    }
    size_t size()const { return components.size(); }

private:
    std::vector<T> components;
    std::vector<int> free;
};

