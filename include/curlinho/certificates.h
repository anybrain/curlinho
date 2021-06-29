#ifndef CPR_CERTIFICATES_H
#define CPR_CERTIFICATES_H

#include "curl/curl.h"
#include "cprtypes.h"
#include <openssl/ssl.h>

namespace curlinho {

class Certificates {
  public:
    Certificates();
    Certificates(curlinho::CertType type, std::string hpkp);
    Certificates(CertType type, char* certString);
    Certificates(CertType type, char* certString, std::string hpkp);

    CertType certType_;
    char* certString_;
    std::string hpkp_;
};

} // namespace cpr

#endif
