#include "curlinho/session.h"
#include "curl/curl.h"
#include "curlinho/util.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

namespace curlinho {

Session::Session() {
  curl_ = std::unique_ptr<CurlHolder, std::function<void(CurlHolder *)>>(newHolder(), &freeHolder);
  auto curl = curl_->handle;
  if (curl) {
    // Set up some sensible defaults
    auto version_info = curl_version_info(CURLVERSION_NOW);
    auto version = std::string{"curl/"} + std::string{version_info->version};
    curl_easy_setopt(curl, CURLOPT_USERAGENT, version.data());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_->error);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

#if defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER)
    curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, sslCert);
#endif
#ifdef CPR_CURL_NOSIGNAL
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
#endif
#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 25
#if LIBCURL_VERSION_PATCH >= 0
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
#endif
#endif
#endif
  }
}

void Session::freeHolder(CurlHolder *holder) {
  curl_easy_cleanup(holder->handle);
  curl_slist_free_all(holder->chunk);
  curl_formfree(holder->formpost);
  delete holder;
}

CurlHolder *Session::newHolder() {
  CurlHolder *holder = new CurlHolder();
  holder->handle = curl_easy_init();
  holder->chunk = NULL;
  holder->formpost = NULL;
  return holder;
}

CurlHolder *Session::cloneHolder(CurlHolder *other) {
  CurlHolder *holder = new CurlHolder();
  holder->handle = other->handle;
  holder->chunk = other->chunk;
  holder->formpost = other->formpost;
  return holder;
}

CURLcode Session::sslCert(CURL *curl, void *sslctx, void *parm) {
  CURLcode rv = CURLE_ABORTED_BY_CALLBACK;

  static const char mypem[] =
      "-----BEGIN CERTIFICATE-----\n"
      "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\n"
      "Q+6IHdfGjjxDah2nGN59PRbxYvnKkKj9\n"
      "-----END CERTIFICATE-----\n";

  BIO *cbio = BIO_new_mem_buf(mypem, sizeof(mypem));
  X509_STORE *cts = SSL_CTX_get_cert_store((SSL_CTX *)sslctx);
  int i;
  STACK_OF(X509_INFO) * inf;
  (void)curl;
  (void)parm;

  if (!cts || !cbio) {
    return rv;
  }

  inf = PEM_X509_INFO_read_bio(cbio, NULL, NULL, NULL);

  if (!inf) {
    BIO_free(cbio);
    return rv;
  }

  for (i = 0; i < sk_X509_INFO_num(inf); i++) {
    X509_INFO *itmp = sk_X509_INFO_value(inf, i);
    if (itmp->x509) {
      X509_STORE_add_cert(cts, itmp->x509);
    }
    if (itmp->crl) {
      X509_STORE_add_crl(cts, itmp->crl);
    }
  }

  sk_X509_INFO_pop_free(inf, X509_INFO_free);
  BIO_free(cbio);

  rv = CURLE_OK;
  return rv;
}

void Session::applyDefaults() {
  Defaults &defaults = Defaults::Instance();
  if (defaults.HasUrl()) {
    SetUrl(defaults.url_);
  }
  if (defaults.HasAuth()) {
    SetAuth(defaults.auth_);
  }
  if (defaults.HasTimeout()) {
    SetTimeout(defaults.timeout_);
  }
  if (defaults.HasHmac()) {
    SetHmac(defaults.hmac_);
  }
  SetHeaders(defaults.headers_);
  SetProtocolVersion(defaults.protocolVersion_);
  SetRetryPolicy(defaults.retryPolicy_);
}

void Session::SetHmac(const Hmac &hmac) {
  hmac_ = hmac;
}

void Session::PrepareHmac(const std::string &path, const std::string &method, const std::string &body) {
  hmac_.prepareSignature(path, method, body);
  SetHeaders(hmac_.getHmacHeaders());
}

void Session::SetUrl(const Url &url) {
  url_ = url;
}

void Session::AppendUrl(const Url &url) {
  url_ = url_ + url;
}

void Session::SetParameters(const Parameters &parameters) {
  parameters_ = parameters;
}

void Session::SetParameters(Parameters &&parameters) {
  parameters_ = std::move(parameters);
}

void Session::SetHeaders(const Headers &headers) {
  auto curl = curl_->handle;
  if (curl) {
    Headers newHeaders;
    newHeaders.insert(headers.begin(), headers.end());
    newHeaders.insert(headers_.begin(), headers_.end());
    headers_ = std::move(newHeaders);
    struct curl_slist *chunk = nullptr;
    for (const auto &header : headers_) {
      auto header_string = std::string{header.first};
       if (header.second.empty()) {
        header_string += ";";
      } else {
        header_string += ": " + header.second;
      }

      auto temp = curl_slist_append(chunk, header_string.data());
      if (temp) {
        chunk = temp;
      }
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    curl_slist_free_all(curl_->chunk);
    curl_->chunk = chunk;
  }
}

void Session::SetTimeout(const Timeout &timeout) {
  auto curl = curl_->handle;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout.Milliseconds());
  }
}

void Session::SetAuth(const Authentication &auth) {
  auto curl = curl_->handle;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, auth.GetAuthString());
  }
}

void Session::SetBody(Body &&body) {
  auto curl = curl_->handle;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, body.data());
  }
}

void Session::SetBody(const Body &body) {
  auto curl = curl_->handle;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.data());
  }
}

void Session::SetProtocolVersion(const ProtocolVersion &protocol_version) {
#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 33
#if LIBCURL_VERSION_PATCH >= 0
  auto curl = curl_->handle;
  if (curl) {
    if (protocol_version == curlinho::HTTP::v2) {
      if (curl_version_info(CURLVERSION_NOW)->features & CURL_VERSION_HTTP2) {
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
      } else {
        std::cerr << "No HTTP/2 support" << std::endl;
      }
    }
  }
#endif
#endif
#endif
}

void Session::SetRetryPolicy(const RetryPolicy &retryPolicy) {
  retryPolicy_ = retryPolicy;
}

Response Session::Get() {
  auto curl = curl_->handle;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
  }

  return makeRequest(curl);
}

Response Session::Post() {
  auto curl = curl_->handle;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
  }

  return makeRequest(curl);
}

Response Session::makeRequest(CURL *curl) {
  if (!parameters_.content.empty()) {
    Url new_url{url_ + "?" + parameters_.content};
    curl_easy_setopt(curl, CURLOPT_URL, new_url.data());
  } else {
    curl_easy_setopt(curl, CURLOPT_URL, url_.data());
  }

  curl_->error[0] = '\0';

  std::string response_string;
  std::string header_string;
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlinho::util::writeFunction);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

  auto curl_error = curl_easy_perform(curl);

  char *raw_url;
  long response_code;
  double elapsed;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
  curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
  curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &raw_url);

  return Response{static_cast<std::int32_t>(response_code),
                  std::move(response_string),
                  curlinho::util::parseHeaders(header_string),
                  raw_url,
                  elapsed,
                  Error(curl_error, curl_->error)};
}

void Session::SetOption(const Url &url) {
  SetUrl(url);
}
void Session::SetOption(const Parameters &parameters) {
  SetParameters(parameters);
}
void Session::SetOption(Parameters &&parameters) {
  SetParameters(std::move(parameters));
}
void Session::SetOption(const Headers &header) {
  SetHeaders(header);
}
void Session::SetOption(const Timeout &timeout) {
  SetTimeout(timeout);
}
void Session::SetOption(const Authentication &auth) {
  SetAuth(auth);
}
void Session::SetOption(const Body &body) {
  SetBody(body);
}
void Session::SetOption(Body &&body) {
  SetBody(std::move(body));
}
void Session::SetOption(const ProtocolVersion &protocolVersion) {
  SetProtocolVersion(protocolVersion);
}
void Session::SetOption(const RetryPolicy &retryPolicy) {
  SetRetryPolicy(retryPolicy);
}
void Session::SetOption(const Hmac &hmac) {
  SetHmac(hmac);
}

} // namespace curlinho
