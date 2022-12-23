#ifndef CPR_CPR_TYPES_H
#define CPR_CPR_TYPES_H

#include <map>
#include <string>
#include <vector>

namespace curlinho {

struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const noexcept;
};

using Headers = std::map<std::string, std::string, CaseInsensitiveCompare>;
using Url = std::string;
using ProtocolVersion = enum class HTTP { v1x, v2 };

struct RetryPolicy {
  int nr_retries = 0;
  int minDelay = 0;
  int maxDelay = 0;
  int maxBackOff = 0;
  std::vector<int> recoverableCodes_{408, 425, 429, 502, 503, 504, 522, 524};
};

} // namespace curlinho

#endif
