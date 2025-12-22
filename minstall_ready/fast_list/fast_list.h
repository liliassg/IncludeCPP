#pragma once
#include <vector>
#include <algorithm>
#include <cstdint>

namespace includecpp {

class FastList {
public:
    FastList();
    std::vector<int> data;
    void add(int value);
    int get(int index);
    int size();
    void clear();
};

std::vector<int> fast_sort(const std::vector<int>& input);
std::vector<int> fast_reverse(const std::vector<int>& input);
int64_t fast_sum(const std::vector<int>& input);
int fast_max(const std::vector<int>& input);
int fast_min(const std::vector<int>& input);

}
