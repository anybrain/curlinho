#ifndef CPR_ERROR_H
#define CPR_ERROR_H

#include <cstdint>
#include <string>

#include "curlinho/cprtypes.h"
#include "curlinho/defines.h"
#include "curl/curl.h"

namespace curlinho {

class Error {
  public:
    Error() : code{0} {}

    template <typename TextType>
    Error(const std::int32_t& curl_code, TextType&& p_error_message)
            : code{curl_code}, message{CRL_FWD(p_error_message)} {}

    explicit operator bool() const {
        return code != CURLE_OK;
    }

    int code;
    std::string message;
};

} // namespace cpr

#endif
