#pragma once
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <iomanip>

namespace includecpp {

class PyString {
public:
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter = " ");
    static std::string join(const std::vector<std::string>& parts, const std::string& separator = "");
    static std::string strip(const std::string& str, const std::string& chars = " \t\n\r");
    static std::string lstrip(const std::string& str, const std::string& chars = " \t\n\r");
    static std::string rstrip(const std::string& str, const std::string& chars = " \t\n\r");
    static std::string upper(const std::string& str);
    static std::string lower(const std::string& str);
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static bool startswith(const std::string& str, const std::string& prefix);
    static bool endswith(const std::string& str, const std::string& suffix);
    static int count(const std::string& str, const std::string& sub);
    static int find(const std::string& str, const std::string& sub, int start = 0);
    static std::string slice(const std::string& str, int start, int end = -1);
    static bool isdigit(const std::string& str);
    static bool isalpha(const std::string& str);
    static std::string zfill(const std::string& str, int width);
};

class PyPath {
public:
    static std::string join(const std::vector<std::string>& parts);
    static std::string dirname(const std::string& path);
    static std::string basename(const std::string& path);
    static std::string abspath(const std::string& path);
    static bool exists(const std::string& path);
    static bool isfile(const std::string& path);
    static bool isdir(const std::string& path);
    static std::vector<std::string> listdir(const std::string& path);
    static bool makedirs(const std::string& path);
    static bool remove(const std::string& path);
    static std::string extension(const std::string& path);
    static int64_t getsize(const std::string& path);
    static std::string normpath(const std::string& path);
};

class PyList {
public:
    template<typename T>
    static std::vector<T> slice(const std::vector<T>& vec, int start, int end = -1, int step = 1);

    template<typename T>
    static std::vector<T> filter(const std::vector<T>& vec, bool(*predicate)(const T&));

    template<typename T>
    static void reverse(std::vector<T>& vec);

    template<typename T>
    static std::vector<T> sorted(const std::vector<T>& vec);

    template<typename T>
    static int count(const std::vector<T>& vec, const T& value);

    template<typename T>
    static int index(const std::vector<T>& vec, const T& value);

    template<typename T>
    static std::vector<T> unique(const std::vector<T>& vec);
};

class PyDict {
public:
    static std::vector<std::string> keys(const std::map<std::string, std::string>& dict);
    static std::vector<std::string> values(const std::map<std::string, std::string>& dict);
    static bool has_key(const std::map<std::string, std::string>& dict, const std::string& key);
    static std::string get(const std::map<std::string, std::string>& dict, const std::string& key, const std::string& default_value = "");
    static std::map<std::string, std::string> fromkeys(const std::vector<std::string>& keys, const std::string& value = "");
};

class PyTime {
public:
    static std::string strftime(const std::string& format);
    static std::string now();
    static int64_t timestamp();
    static void sleep(double seconds);
};

class PyFile {
public:
    static std::string read(const std::string& filepath);
    static std::vector<std::string> readlines(const std::string& filepath);
    static bool write(const std::string& filepath, const std::string& content);
    static bool writelines(const std::string& filepath, const std::vector<std::string>& lines);
    static bool append(const std::string& filepath, const std::string& content);
};

// Template implementations
template<typename T>
std::vector<T> PyList::slice(const std::vector<T>& vec, int start, int end, int step) {
    int size = static_cast<int>(vec.size());
    if (start < 0) start += size;
    if (end < 0) end += size;
    if (end == -1) end = size;

    start = std::max(0, std::min(start, size));
    end = std::max(0, std::min(end, size));

    std::vector<T> result;
    for (int i = start; i < end; i += step) {
        result.push_back(vec[i]);
    }
    return result;
}

template<typename T>
std::vector<T> PyList::filter(const std::vector<T>& vec, bool(*predicate)(const T&)) {
    std::vector<T> result;
    for (const auto& item : vec) {
        if (predicate(item)) {
            result.push_back(item);
        }
    }
    return result;
}

template<typename T>
void PyList::reverse(std::vector<T>& vec) {
    std::reverse(vec.begin(), vec.end());
}

template<typename T>
std::vector<T> PyList::sorted(const std::vector<T>& vec) {
    std::vector<T> result = vec;
    std::sort(result.begin(), result.end());
    return result;
}

template<typename T>
int PyList::count(const std::vector<T>& vec, const T& value) {
    return static_cast<int>(std::count(vec.begin(), vec.end(), value));
}

template<typename T>
int PyList::index(const std::vector<T>& vec, const T& value) {
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end()) {
        return static_cast<int>(std::distance(vec.begin(), it));
    }
    return -1;
}

template<typename T>
std::vector<T> PyList::unique(const std::vector<T>& vec) {
    std::vector<T> result;
    for (const auto& item : vec) {
        if (std::find(result.begin(), result.end(), item) == result.end()) {
            result.push_back(item);
        }
    }
    return result;
}

} // namespace includecpp
