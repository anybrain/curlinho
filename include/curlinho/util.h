#ifndef CPR_UTIL_H
#define CPR_UTIL_H

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>
#include <memory>

#include "curlinho/cprtypes.h"
#include "curl/curl.h"

namespace curlinho {

class Timeout {
public:
  Timeout(const std::chrono::milliseconds& duration) : ms{duration} {}
  Timeout(const std::int32_t& milliseconds) : Timeout{std::chrono::milliseconds(milliseconds)} {}

  long Milliseconds() const;

  std::chrono::milliseconds ms;
};

namespace util {

Headers parseHeaders(const std::string& headers);
size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data);
std::vector<std::string> split(const std::string& to_split, char delimiter);
std::string urlEncode(const std::string& response);
int randomNumberRange(int low, int high);

} // namespace util

} // namespace curlinho

#endif
