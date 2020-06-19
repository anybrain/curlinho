/*
 * Created by Jose Silva on 2019-09-05.
 * Copyright (c) 2019 MIT. All rights reserved.
 */
#ifndef CURLINHO_CURLINHO_H
#define CURLINHO_CURLINHO_H

#include "curl/curl.h"
#include "curlinho/defaults.h"
#include "curlinho/hmac.h"
#include "curlinho/session.h"

#include <functional>
#include <future>
#include <string>

namespace curlinho {

using AsyncResponse = std::future<Response>;

namespace priv {

inline void set_option(Session &session) {}
template <typename Option>
void set_option(Session &session, Option &&t) {
  session.SetOption(CRL_FWD(t));
}
template <typename Option, typename... Options>
inline void set_option(Session &session, Option &&t, Options &&... ts) {
  set_option(session, CRL_FWD(t));
  set_option(session, CRL_FWD(ts)...);
}

inline void set_default() {}
template <typename Option>
inline void set_default(Option &&t) {
  Defaults::Instance().SetOption(CRL_FWD(t));
}
template <typename Option, typename... Options>
inline void set_default(Option &&t, Options &&... ts) {
  set_default(CRL_FWD(t));
  set_default(CRL_FWD(ts)...);
}

} // namespace priv

template <typename... Options>
void SetDefaults(Options &&... ts) {
  priv::set_default(CRL_FWD(ts)...);
}

// Get methods
template <typename... Options>
Response GetIntern(const std::string &path, Options &&... ts) {
  Session session;
  session.applyDefaults();
  priv::set_option(session, CRL_FWD(ts)...);
  if (!path.empty()) {
    session.AppendUrl(path);
  }
  if (session.HasHmac()) {
    session.PrepareHmac(path, "GET", "");
  }
  return session.Get();
}

template <typename... Options>
Response Get(const std::string &path, Options &&... ts) {
  Response res = GetIntern(path, std::move(ts)...);
  handleGetRetries(res, path, std::move(ts)...);
  return res;
}

// Get async methods
template <typename... Options>
AsyncResponse GetAsync(const std::string &path, Options... ts) {
  return std::async(
      std::launch::async, [path](Options... ts) { return Get(path, std::move(ts)...); },
      std::move(ts)...);
}

// Post methods
template <typename... Options>
Response Post(const std::string &path, const Body &body, Options &&... ts) {
  Response res = PostIntern(path, body, CRL_FWD(ts)...);
  handlePostRetries(res, path, body, CRL_FWD(ts)...);
  return res;
}

// Post methods
template <typename... Options>
Response PostIntern(const std::string &path, const Body &body, Options &&... ts) {
  Session session;
  session.applyDefaults();
  priv::set_option(session, CRL_FWD(ts)...);
  if (!path.empty()) {
    session.AppendUrl(path);
  }
  if (session.HasHmac()) {
    session.PrepareHmac(path, "POST", body);
  }
  session.SetBody(body);
  return session.Post();
}

// Post async methods
template <typename... Options>
AsyncResponse PostAsync(const std::string &path, const Body &body, Options... ts) {
  return std::async(
      std::launch::async,
      [path, body](Options... ts) { return Post(path, body, std::move(ts)...); }, std::move(ts)...);
}

// Post detach
template <typename... Options>
void PostDetach(const std::string &path, const Body &body, Options... ts) {
  std::thread([path, body](Options... ts) { Post(path, body, std::move(ts)...); }, std::move(ts)...)
      .detach();
}

template <typename... Options>
void handlePostRetries(Response res, const std::string &path, const Body &body, Options... ts) {
  std::thread([res, path, body](Options... ts) {
        curlinho::Response r = res;
        auto retries = Defaults::Instance().retryPolicy_;
        for (auto timer : retries.delays_) {
          if (r.status_code != 200 || r.error.code != CURLE_OK) {
            if (r.error.code == CURLE_OPERATION_TIMEOUTED) {
              CRL_LOG << "Request timed out, Another retry!" << std::endl;
              CRL_SLEEP(timer);
            } else if (std::find(retries.recoverableCodes_.begin(), retries.recoverableCodes_.end(),
                                 r.status_code) == retries.recoverableCodes_.end()) {
              CRL_LOG << "Error was not recoverable, no need to retry! - " << r.status_code
                      << std::endl;
              break;
            } else {
              CRL_LOG << "Request failed, retry in " << timer << " seconds. - " << r.status_code
                      << std::endl;
              CRL_SLEEP(timer);
            }
            r = PostIntern(path, body, std::move(ts)...);
          }
        }
      },
      std::move(ts)...)
      .detach();
}

template <typename... Options>
void handleGetRetries(Response res, const std::string &path, Options... ts) {
  std::thread([res, path](Options... ts) {
        curlinho::Response r = res;
        auto retries = Defaults::Instance().retryPolicy_;
        for (auto timer : retries.delays_) {
          if (r.status_code != 200 || r.error.code != CURLE_OK) {
            if (r.error.code == CURLE_OPERATION_TIMEOUTED) {
              CRL_LOG << "Request timed out, Another retry!" << std::endl;
              CRL_SLEEP(timer);
            } else if (std::find(retries.recoverableCodes_.begin(), retries.recoverableCodes_.end(),
                                 r.status_code) == retries.recoverableCodes_.end()) {
              CRL_LOG << "Error was not recoverable, no need to retry! - " << r.status_code
                      << std::endl;
              break;
            } else {
              CRL_LOG << "Request failed, retry in " << timer << " seconds. - " << r.status_code
                      << std::endl;
              CRL_SLEEP(timer);
            }
            r = GetIntern(path, std::move(ts)...);
          }
        }
      },
      std::move(ts)...)
      .detach();
}
} // namespace curlinho
#endif // CURLINHO_CURLINHO_H
