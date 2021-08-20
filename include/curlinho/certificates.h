#ifndef CPR_CERTIFICATES_H
#define CPR_CERTIFICATES_H

#include "curl/curl.h"
#include "cprtypes.h"
#include <openssl/ssl.h>

namespace curlinho {

class Certificates {
  public:
    Certificates();
    Certificates(char* certString, std::string pkp);

    char* certString_;
    std::string pkp_;
};

} // namespace cpr

#endif
