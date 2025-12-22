#include "fast_list.h"
#include <numeric>
#include <limits>

namespace includecpp {

FastList::FastList() {
    data.clear();
}

void FastList::add(int value) {
    data.push_back(value);
}

int FastList::get(int index) {
    if (index < 0 || index >= static_cast<int>(data.size())) {
        return 0;
    }
    return data[index];
}

int FastList::size() {
    return static_cast<int>(data.size());
}

void FastList::clear() {
    data.clear();
}

std::vector<int> fast_sort(const std::vector<int>& input) {
    std::vector<int> result = input;
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<int> fast_reverse(const std::vector<int>& input) {
    std::vector<int> result = input;
    std::reverse(result.begin(), result.end());
    return result;
}

int64_t fast_sum(const std::vector<int>& input) {
    return std::accumulate(input.begin(), input.end(), static_cast<int64_t>(0));
}

int fast_max(const std::vector<int>& input) {
    if (input.empty()) {
        return 0;
    }
    return *std::max_element(input.begin(), input.end());
}

int fast_min(const std::vector<int>& input) {
    if (input.empty()) {
        return 0;
    }
    return *std::min_element(input.begin(), input.end());
}

}
