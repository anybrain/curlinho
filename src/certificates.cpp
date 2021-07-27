#include "curlinho/certificates.h"

namespace curlinho {

Certificates::Certificates() {
  certString_ = "";
  pkp_ = "";
}

Certificates::Certificates(char* certString, std::string pkp) {
  certString_ = certString;
  pkp_ = pkp;
}

} // namespace curlinho
