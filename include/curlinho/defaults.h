/*
* Created by Jose Silva on 2019-09-06.
* Copyright (c) 2019 Anybrain. All rights reserved.
*/
#ifndef CURLINHO_DEFAULTS_H
#define CURLINHO_DEFAULTS_H

#include "auth.h"
#include "certificates.h"
#include "cprtypes.h"
#include "parameters.h"
#include "response.h"
#include "util.h"

namespace curlinho {

class Defaults {

public:
  static Defaults &Instance() {
    static Defaults instance;
    return instance;
  }

  void SetUrl(const Url &url);
  void SetHeaders(const Headers &headers);
  void SetTimeout(const Timeout &timeout);
  void SetAuth(const Authentication &auth);
  void SetProtocolVersion(const ProtocolVersion &protocol_version);
  void SetRetryPolicy(const RetryPolicy &retryPolicy);
  void SetCertificate(const Certificates &certificates);

  bool HasUrl();
  bool HasHeaders();
  bool HasTimeout();
  bool HasAuth();

  // Used in templated functions
  void SetOption(const Url &url);
  void SetOption(const Headers &headers);
  void SetOption(const Timeout &timeout);
  void SetOption(const Authentication &auth);
  void SetOption(const ProtocolVersion &protocolVersion);
  void SetOption(const RetryPolicy &retryPolicy);
  void SetOption(const Certificates &certificates);
  Headers headers_;
  Authentication auth_;
  ProtocolVersion protocolVersion_;
  RetryPolicy retryPolicy_;
  Certificates certificates_;

private:
  Defaults() : timeout_(0), auth_("", ""), protocolVersion_(HTTP::v1x) {};
  ~Defaults() = default;
  Defaults(Defaults const &) = delete;
  Defaults &operator=(Defaults const &) = delete;
};
}
#endif // CURLINHO_DEFAULTS_H
