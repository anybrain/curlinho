#ifndef CPR_SESSION_H
#define CPR_SESSION_H

#include <cstdint>
#include <functional>
#include <memory>

#include "curlinho/auth.h"
#include "curlinho/body.h"
#include "curlinho/cprtypes.h"
#include "curlinho/defaults.h"
#include "curlinho/parameters.h"
#include "curlinho/response.h"
#include "curlinho/util.h"

namespace curlinho {

struct CurlHolder {
  CURL *handle;
  struct curl_slist *chunk;
  struct curl_httppost *formpost;
  char error[CURL_ERROR_SIZE];
};

class Session {
  public:
  Session();
  ~Session() = default;

  void applyDefaults();
  void AppendUrl(const Url &url);
  void SetUrl(const Url &url);
  void SetParameters(const Parameters &parameters);
  void SetParameters(Parameters &&parameters);
  void SetHeaders(const Headers &headers);
  void SetTimeout(const Timeout &timeout);
  void SetAuth(const Authentication &auth);
  void SetBody(Body &&body);
  void SetBody(const Body &body);
  void SetProtocolVersion(const ProtocolVersion &protocol_version);
  void SetRetryPolicy(const RetryPolicy &retryPolicy);
  void SetHmac(const Hmac &hmac);
  void PrepareHmac(const std::string &path, const std::string &method, const std::string &body);
  bool HasHmac() { return !hmac_.empty(); };

  // Used in templated functions
  void SetOption(const Url &url);
  void SetOption(const Parameters &parameters);
  void SetOption(Parameters &&parameters);
  void SetOption(const Headers &headers);
  void SetOption(const Timeout &timeout);
  void SetOption(const Authentication &auth);
  void SetOption(Body &&body);
  void SetOption(const Body &body);
  void SetOption(const ProtocolVersion &protocol_version);
  void SetOption(const RetryPolicy &retryPolicy);
  void SetOption(const Hmac &hmac);

  Response Get();
  Response Post();

  private:
  std::unique_ptr<CurlHolder, std::function<void(CurlHolder *)>> curl_;
  Url url_;
  Parameters parameters_;
  Headers headers_;
  RetryPolicy retryPolicy_;
  Hmac hmac_;

  Response makeRequest(CURL *curl);
  static void freeHolder(CurlHolder *holder);
  static CurlHolder *newHolder();
  static CurlHolder *cloneHolder(CurlHolder *other);
};

} // namespace curlinho

#endif
