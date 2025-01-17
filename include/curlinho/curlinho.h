/*
 * Created by Jose Silva on 2019-09-05.
 * Copyright (c) 2019 MIT. All rights reserved.
 */
#ifndef CURLINHO_CURLINHO_H
#define CURLINHO_CURLINHO_H

#include "curl/curl.h"
#include "curlinho/session.h"
#include "curlinho/util.h"

#include <functional>
#include <future>
#include <string>
#include <cmath>

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

} // namespace priv

// Get methods
template <typename... Options>
Response GetIntern(const std::string &url, Options &&... ts) {
  Session session;
  priv::set_option(session, CRL_FWD(ts)...);
  if (!url.empty()) {
    session.SetUrl(url);
  }
  return session.Get();
}

template <typename... Options>
Response Get(const std::string &url, Options &&... ts) {
  Response res = GetIntern(url, std::move(ts)...);
  handleGetRetries(res, url, std::move(ts)...);
  return res;
}

// Get async methods
template <typename... Options>
AsyncResponse GetAsync(const std::string &url, Options... ts) {
  return std::async(
      std::launch::async, [url](Options... ts) { return Get(url, std::move(ts)...); },
      std::move(ts)...);
}

// Post methods
template <typename... Options>
Response Post(const std::string &url, const Body &body, Options &&... ts) {
  Response res = PostIntern(url, body, CRL_FWD(ts)...);
  handlePostRetries(res, url, body, CRL_FWD(ts)...);
  return res;
}

// Post methods
template <typename... Options>
Response PostIntern(const std::string &url, const Body &body, Options &&... ts) {
  Session session;
  priv::set_option(session, CRL_FWD(ts)...);
  if (!url.empty()) {
    session.SetUrl(url);
  }
  session.SetBody(body);
  return session.Post();
}

// Post async methods
template <typename... Options>
AsyncResponse PostAsync(const std::string &url, const Body &body, Options... ts) {
  return std::async(
      std::launch::async,
      [url, body](Options... ts) { return Post(url, body, std::move(ts)...); }, std::move(ts)...);
}

// Post detach
template <typename... Options>
void PostDetach(const std::string &url, const Body &body, Options... ts) {
  std::thread([url, body](Options... ts) { Post(url, body, std::move(ts)...); }, std::move(ts)...)
      .detach();
}

template <typename... Options>
void handlePostRetries(Response res, const std::string &url, const Body &body, Options... ts) {
  std::thread(
      [res, url, body](Options... ts) {
        Session session(url, body);
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
void handleGetRetries(Response res, const std::string &url, Options... ts) {
  std::thread(
      [res, url](Options... ts) {
        Session session(url, curlinho::Body{});
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

static inline std::string GetHttpLastVersion() {
  Session session;
  return session.GetHttpLastProtocolVersion();
}

} // namespace curlinho
#endif // CURLINHO_CURLINHO_H
