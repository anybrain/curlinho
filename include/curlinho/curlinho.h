/*
* Created by José Luis Silva on 2019-09-05.
* Copyright (c) 2019 MIT. All rights reserved.
*/
#ifndef CURLINHO_CURLINHO_H
#define CURLINHO_CURLINHO_H

#include "curl/curl.h"
#include "curlinho/session.h"
#include "curlinho/defaults.h"

#include <functional>
#include <future>
#include <string>

namespace curlinho {

using AsyncResponse = std::future<Response>;

namespace priv {

void set_option(Session &session) {}
template <typename Option>
void set_option(Session &session, Option &&t) {
  session.SetOption(CRL_FWD(t));
}
template <typename Option, typename... Options>
void set_option(Session &session, Option &&t, Options &&... ts) {
  set_option(session, CRL_FWD(t));
  set_option(session, CRL_FWD(ts)...);
}

void set_default() {}
template <typename Option>
void set_default(Option &&t) {
  Defaults::Instance().SetOption(CRL_FWD(t));
}
template <typename Option, typename... Options>
void set_default(Option &&t, Options &&... ts) {
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
Response Get(const std::string &path, Options &&... ts) {
  Session session;
  priv::set_option(session, CRL_FWD(ts)...);
  if (!path.empty()) {
    session.AppendUrl(path);
  }
  return session.Get();
}

// Get async methods
template <typename... Options>
AsyncResponse GetAsync(const std::string &path, Options... ts) {
  return std::async(std::launch::async,
                    [path](Options... ts) { return Get(path, std::move(ts)...); },
                    std::move(ts)...);
}

// Post methods
template <typename... Options>
Response Post(const std::string &path, const Body &body,
              Options &&... ts) {
  Session session;
  priv::set_option(session, CRL_FWD(ts)...);
  if (!path.empty()) {
    session.AppendUrl(path);
  }
  session.SetBody(body);
  return session.Post();
}

// Post async methods
template <typename... Options>
AsyncResponse PostAsync(const std::string &path, const Body &body,
                        Options... ts) {
  return std::async(
      std::launch::async,
      [path, body](Options... ts) { return Post(path, body, std::move(ts)...); },
      std::move(ts)...);
}

// Post detach
template <typename... Options>
void PostDetach(const std::string &path, const Body &body,
                Options... ts) {
  std::thread([path, body](Options... ts) { Post(path, body, std::move(ts)...); },
              std::move(ts)...)
      .detach();
}

}
#endif // CURLINHO_CURLINHO_H
