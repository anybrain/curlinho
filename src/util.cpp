#include "curlinho/util.h"
#include "curlinho/cprtypes.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <random>

namespace curlinho {

bool CaseInsensitiveCompare::operator()(const std::string& a, const std::string& b) const noexcept {
    return std::lexicographical_compare(
            a.begin(), a.end(), b.begin(), b.end(),
            [](unsigned char ac, unsigned char bc) { return std::tolower(ac) < std::tolower(bc); });
}

long Timeout::Milliseconds() const {
    static_assert(std::is_same<std::chrono::milliseconds, decltype(ms)>::value,
                  "Following casting expects milliseconds.");
    return static_cast<long>(ms.count());
}

namespace util {

Headers parseHeaders(const std::string& headers) {
    Headers header;
    std::vector<std::string> lines;
    std::istringstream stream(headers);
    {
        std::string line;
        while (std::getline(stream, line, '\n')) {
            lines.push_back(line);
        }
    }

    for (auto& line : lines) {
        if (line.substr(0, 5) == "HTTP/") {
            header.clear();
        }

        if (line.length() > 0) {
            auto found = line.find(":");
            if (found != std::string::npos) {
                auto value = line.substr(found + 1);
                value.erase(0, value.find_first_not_of("\t "));
                value.resize((std::min)(value.size(), value.find_last_not_of("\t\n\r ") + 1));
                header[line.substr(0, found)] = value;
            }
        }
    }

    return header;
}

std::vector<std::string> split(const std::string& to_split, char delimiter) {
    std::vector<std::string> tokens;

    std::stringstream stream(to_split);
    std::string item;
    while (std::getline(stream, item, delimiter)) {
        tokens.push_back(item);
    }

    return tokens;
}

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

std::string urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (auto i = value.cbegin(), n = value.cend(); i != n; ++i) {
        std::string::value_type c = (*i);
        // Keep alphanumeric and other accepted characters intact
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }
        // Any other characters are percent-encoded
        escaped << '%' << std::setw(2) << std::int32_t(static_cast<unsigned char>(c));
    }

    return escaped.str();
}

int randomNumberRange(int low, int high) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(low, high);
  return dist(gen);
}

} // namespace util
} // namespace cpr
