/*
 * Created by Jose Silva on 2019-09-05.
 * Copyright (c) 2019 MIT. All rights reserved.
 */
#ifndef CURLINHO_CURLINHO_H
#define CURLINHO_CURLINHO_H

#include "curl/curl.h"
#include "curlinho/defaults.h"
#include "curlinho/session.h"
#include "curlinho/util.h"

#include <functional>
#include <future>
#include <string>

namespace curlinho {

using AsyncResponse = std::future<Response>;

namespace priv {

inline void set_option(Session &session) {
  session.SetOption();
}
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
  std::thread(
      [res, path, body](Options... ts) {
        Session session(path, body);
        priv::set_option(session, CRL_FWD(ts)...);
        auto retries = session.GetRetryPolicy();
        curlinho::Response r = res;
        for (int retry = 0; retry < retries.numRetries; retry++) {
          int timer = (int)(std::pow(2, retry) *
                            util::randomNumberRange(retries.minDelay, retries.maxDelay));
          if (timer > retries.maxBackOff) {
            timer = retries.maxBackOff;
          }
          if (r.status_code != 200 || r.error.code != CURLE_OK) {
            if (r.error.code == CURLE_OPERATION_TIMEOUTED) {
              CRL_LOG << "Request timed out, Another retry!" << std::endl;
              CRL_SLEEP(timer);
            } else if (std::find(retries.recoverableCodes_.begin(), retries.recoverableCodes_.end(),
                                 r.status_code) == retries.recoverableCodes_.end()) {
              CRL_LOG << "Error was not recoverable, no need to retry! - " << r.status_code;
              break;
            } else {
              CRL_LOG << "Request failed, retry in " << timer << " seconds. - " << r.status_code;
              CRL_SLEEP(timer);
            }
            r = session.Post();
          }
        }
      },
      std::move(ts)...)
      .detach();
}

template <typename... Options>
void handleGetRetries(Response res, const std::string &path, Options... ts) {
  std::thread(
      [res, path](Options... ts) {
        Session session(path, "");
        priv::set_option(session, CRL_FWD(ts)...);
        auto retries = session.GetRetryPolicy();
        curlinho::Response r = res;
        for (int retry = 0; retry < retries.numRetries; retry++) {
          int timer = (int)(std::pow(2, retry) *
                            util::randomNumberRange(retries.minDelay, retries.maxDelay));
          if (timer > retries.maxBackOff) {
            timer = retries.maxBackOff;
          }
          if (r.status_code != 200 || r.error.code != CURLE_OK) {
            if (r.error.code == CURLE_OPERATION_TIMEOUTED) {
              CRL_LOG << "Request timed out, Another retry!" << std::endl;
              CRL_SLEEP(timer);
            } else if (std::find(retries.recoverableCodes_.begin(), retries.recoverableCodes_.end(),
                                 r.status_code) == retries.recoverableCodes_.end()) {
              CRL_LOG << "Error was not recoverable, no need to retry! - " << r.status_code;
              break;
            } else {
              CRL_LOG << "Request failed, retry in " << timer << " seconds. - " << r.status_code;
              CRL_SLEEP(timer);
            }
            r = session.Get();
          }
        }
      },
      std::move(ts)...)
      .detach();
}

static inline std::string GetHttpVersion() {
  Session session;
  session.applyDefaults();
  return session.GetProtocolVersion();
}

} // namespace curlinho
#endif // CURLINHO_CURLINHO_H
