#ifndef CPR_CERTIFICATES_H
#define CPR_CERTIFICATES_H

#include "cprtypes.h"
#include "curl/curl.h"
#include <openssl/ssl.h>

namespace curlinho {

class SslCert {
  public:
  SslCert() = default;
  SslCert(const std::string &certString, const std::string &pkp)
      : certString_(certString), pkp_(pkp) {}

  std::string certString_;
  std::string pkp_;
};

} // namespace curlinho

#endif
