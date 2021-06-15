#include "curlinho/certificates.h"

namespace curlinho {
Certificates::Certificates() {
  certType_ = curlinho::CertType::NATIVE;
  certString_ = "";
}

Certificates::Certificates(curlinho::CertType type) {
  certType_ = type;
  certString_ = "";
}

Certificates::Certificates(curlinho::CertType type, char* certString) {
  certType_ = type;
  certString_ = certString;
}

Certificates::Certificates(curlinho::CertType type, char* certString, std::string hpkp) {
  certType_ = type;
  certString_ = certString;
  hpkp_ = hpkp;
}

} // namespace curlinho
