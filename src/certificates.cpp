#include "curlinho/certificates.h"

namespace curlinho {

Certificates::Certificates() {
  certType_ = curlinho::CertType::NATIVE;
  certString_ = "";
  hpkp_ = "";
}

Certificates::Certificates(curlinho::CertType type, std::string hpkp) {
  certType_ = type;
  hpkp_ = hpkp;
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
