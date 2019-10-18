#ifndef CPR_SESSION_H
#define CPR_SESSION_H

#include <cstdint>
#include <memory>
#include <functional>

#include "curlinho/auth.h"
#include "curlinho/body.h"
#include "curlinho/cprtypes.h"
#include "curlinho/parameters.h"
#include "curlinho/response.h"
#include "curlinho/util.h"
#include "defaults.h"

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

    void applyDefaults(Defaults &defaults);
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

    Response Get();
    Response Post();

  private:
    std::unique_ptr<CurlHolder, std::function<void(CurlHolder *)>> curl_;
    Url url_;
    Parameters parameters_;
    Headers headers_;
    RetryPolicy retryPolicy_;

    Response makeRequest(CURL *curl);
    Response makeRequestRetries(CURL *curl);
    static void freeHolder(CurlHolder *holder);
    static CurlHolder *newHolder();
};

} // namespace curlinho

#endif
