#include "pyutils.h"
#include <chrono>
#include <thread>

namespace includecpp {

// ============================================================================
// PyString Implementation
// ============================================================================

std::vector<std::string> PyString::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    result.push_back(str.substr(start));
    return result;
}

std::string PyString::join(const std::vector<std::string>& parts, const std::string& separator) {
    if (parts.empty()) return "";

    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += separator + parts[i];
    }
    return result;
}

std::string PyString::strip(const std::string& str, const std::string& chars) {
    return lstrip(rstrip(str, chars), chars);
}

std::string PyString::lstrip(const std::string& str, const std::string& chars) {
    size_t start = str.find_first_not_of(chars);
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string PyString::rstrip(const std::string& str, const std::string& chars) {
    size_t end = str.find_last_not_of(chars);
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string PyString::upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string PyString::lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string PyString::replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;

    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }

    return result;
}

bool PyString::startswith(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) return false;
    return str.compare(0, prefix.length(), prefix) == 0;
}

bool PyString::endswith(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

int PyString::count(const std::string& str, const std::string& sub) {
    int count = 0;
    size_t pos = 0;

    while ((pos = str.find(sub, pos)) != std::string::npos) {
        ++count;
        pos += sub.length();
    }

    return count;
}

int PyString::find(const std::string& str, const std::string& sub, int start) {
    size_t pos = str.find(sub, start);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
}

std::string PyString::slice(const std::string& str, int start, int end) {
    int size = static_cast<int>(str.length());
    if (start < 0) start += size;
    if (end < 0) end += size;
    if (end == -1) end = size;

    start = std::max(0, std::min(start, size));
    end = std::max(0, std::min(end, size));

    if (start >= end) return "";
    return str.substr(start, end - start);
}

bool PyString::isdigit(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool PyString::isalpha(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), ::isalpha);
}

std::string PyString::zfill(const std::string& str, int width) {
    if (static_cast<int>(str.length()) >= width) return str;
    return std::string(width - str.length(), '0') + str;
}

// ============================================================================
// PyPath Implementation
// ============================================================================

std::string PyPath::join(const std::vector<std::string>& parts) {
    if (parts.empty()) return "";

    std::filesystem::path result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result /= parts[i];
    }
    return result.string();
}

std::string PyPath::dirname(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

std::string PyPath::basename(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

std::string PyPath::abspath(const std::string& path) {
    return std::filesystem::absolute(path).string();
}

bool PyPath::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool PyPath::isfile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

bool PyPath::isdir(const std::string& path) {
    return std::filesystem::is_directory(path);
}

std::vector<std::string> PyPath::listdir(const std::string& path) {
    std::vector<std::string> result;

    if (!std::filesystem::exists(path)) return result;

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        result.push_back(entry.path().filename().string());
    }

    return result;
}

bool PyPath::makedirs(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

bool PyPath::remove(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (...) {
        return false;
    }
}

std::string PyPath::extension(const std::string& path) {
    return std::filesystem::path(path).extension().string();
}

int64_t PyPath::getsize(const std::string& path) {
    try {
        return static_cast<int64_t>(std::filesystem::file_size(path));
    } catch (...) {
        return -1;
    }
}

std::string PyPath::normpath(const std::string& path) {
    return std::filesystem::path(path).lexically_normal().string();
}

// ============================================================================
// PyDict Implementation
// ============================================================================

std::vector<std::string> PyDict::keys(const std::map<std::string, std::string>& dict) {
    std::vector<std::string> result;
    for (const auto& pair : dict) {
        result.push_back(pair.first);
    }
    return result;
}

std::vector<std::string> PyDict::values(const std::map<std::string, std::string>& dict) {
    std::vector<std::string> result;
    for (const auto& pair : dict) {
        result.push_back(pair.second);
    }
    return result;
}

bool PyDict::has_key(const std::map<std::string, std::string>& dict, const std::string& key) {
    return dict.find(key) != dict.end();
}

std::string PyDict::get(const std::map<std::string, std::string>& dict, const std::string& key, const std::string& default_value) {
    auto it = dict.find(key);
    return (it != dict.end()) ? it->second : default_value;
}

std::map<std::string, std::string> PyDict::fromkeys(const std::vector<std::string>& keys, const std::string& value) {
    std::map<std::string, std::string> result;
    for (const auto& key : keys) {
        result[key] = value;
    }
    return result;
}

// ============================================================================
// PyTime Implementation
// ============================================================================

std::string PyTime::strftime(const std::string& format) {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);

    char buffer[256];
    std::strftime(buffer, sizeof(buffer), format.c_str(), &tm);
    return std::string(buffer);
}

std::string PyTime::now() {
    return strftime("%Y-%m-%d %H:%M:%S");
}

int64_t PyTime::timestamp() {
    return static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
}

void PyTime::sleep(double seconds) {
    std::this_thread::sleep_for(
        std::chrono::duration<double>(seconds)
    );
}

// ============================================================================
// PyFile Implementation
// ============================================================================

std::string PyFile::read(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> PyFile::readlines(const std::string& filepath) {
    std::vector<std::string> lines;
    std::ifstream file(filepath);

    if (!file.is_open()) return lines;

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

bool PyFile::write(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << content;
    return file.good();
}

bool PyFile::writelines(const std::string& filepath, const std::vector<std::string>& lines) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    for (const auto& line : lines) {
        file << line << "\n";
    }

    return file.good();
}

bool PyFile::append(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath, std::ios::app);
    if (!file.is_open()) return false;

    file << content;
    return file.good();
}

} // namespace includecpp
