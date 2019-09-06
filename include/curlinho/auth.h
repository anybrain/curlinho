#ifndef CPR_AUTH_H
#define CPR_AUTH_H

#include <string>

#include "curlinho/defines.h"

namespace curlinho {

class Authentication {
  public:
    template <typename UserType, typename PassType>
    Authentication(UserType&& username, PassType&& password)
            : username_{CRL_FWD(username)}, password_{CRL_FWD(password)},
              auth_string_{username_ + ":" + password_} {}

    const char* GetAuthString() const noexcept {
        return auth_string_.data();
    };

    bool empty () {
      return username_.empty();
    }

  private:
    std::string username_;
    std::string password_;
    std::string auth_string_;
};

} // namespace curlinho

#endif
