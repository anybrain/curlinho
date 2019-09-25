#ifndef CPR_RESPONSE_H
#define CPR_RESPONSE_H

#include <cstdint>
#include <string>

#include "curlinho/cprtypes.h"
#include "curlinho/defines.h"
#include "curlinho/error.h"

namespace curlinho {

class Response {
  public:
    Response() = default;

    template <typename TextType, typename HeaderType, typename UrlType,
              typename ErrorType>
    Response(const std::int32_t& p_status_code, TextType&& p_text, HeaderType&& p_header, UrlType&& p_url,
             const double& p_elapsed,
             ErrorType&& p_error = Error{})
            : status_code{p_status_code}, text{CRL_FWD(p_text)}, headers{CRL_FWD(p_header)},
              url{CRL_FWD(p_url)}, elapsed{p_elapsed},
              error{CRL_FWD(p_error)} {}

    std::int32_t status_code;
    std::string text;
    Headers headers;
    Url url;
    double elapsed;
    Error error;
};

} // namespace cpr

#endif
