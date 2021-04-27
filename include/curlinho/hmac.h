/*
 * Created by Jose Silva on 2019-11-29.
 * Copyright (c) 2019 Anybrain. All rights reserved.
 */
#ifndef CURLINHO_HMAC_H
#define CURLINHO_HMAC_H

#include "plog/Log.h"
#include "curlinho/encryption/base64.h"

#include <utility>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <string>
#include <time.h>

namespace curlinho {

class Hmac {
  public:
  Hmac() = default;
  Hmac(std::string username, std::string secret, std::string host, bool validation)
      : username_{std::move(username)}, secret_{std::move(secret)}, host_{std::move(host)}, bodyValidation_{validation} {};

  bool empty() { return username_.empty() || secret_.empty() || host_.empty(); };
  void setBodyValidation(bool validation) { bodyValidation_ = validation; }

  void prepareSignature(const std::string &path, const std::string &method,
                        const std::string &body) {
    struct tm time_info;
    time_t rawtime;
    time(&rawtime);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    gmtime_s(&time_info, &rawtime);
#else
    gmtime_r(&rawtime, &time_info);
#endif
    char date[50];
    strftime(date, 50, "%a, %d %b %Y %H:%M:%S GMT", &time_info);
    date_ = std::string(date);
    std::string requestLine = method + " " + path + " HTTP/2.0";
    std::string headersList = "date host request-line";
    std::string stringToSign = "date: " + date_ + "\nhost: " + host_ + "\n" + requestLine;
    if (bodyValidation_) {
      digest_ = "SHA-256=" + sha256_base64(body);
      stringToSign.append("\ndigest: " + digest_);
      headersList.append(" digest");
    }

    // Using sha256 hash engine here.
    unsigned char *hmac_sha256 =
        HMAC(EVP_sha256(), (void *)secret_.c_str(), (int)secret_.length(),
             (unsigned char *)stringToSign.c_str(), stringToSign.length(), NULL, NULL);

    std::string signature = encryption::base64_encode(hmac_sha256, SHA256_DIGEST_LENGTH);

    authorization_ = "hmac username=\"" + username_ + "\"," + "algorithm=\"hmac-sha256\"," +
                     "headers=\"" + headersList + "\"," + "signature=\"" + signature +
                     "\"";
  }

  Headers getHmacHeaders() {
    if (bodyValidation_) {
		return Headers{
			{"Date", date_}, {"Authorization", authorization_}, {"Digest", digest_}, {"Host", host_}};
    } else {
      return Headers{ {"Date", date_}, {"Authorization", authorization_}, {"Host", host_} };
    }
  }

  static inline std::string sha256_base64(const std::string &text) {
    char *string = new char[text.length() + 1];
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    strcpy_s(string, text.length() + 1, text.c_str());
#else
    strcpy(string, text.c_str());
#endif
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(hash, &sha256);
	return encryption::base64_encode(hash, SHA256_DIGEST_LENGTH);
  }

  private:
  std::string digest_;
  std::string authorization_;
  std::string username_;
  std::string secret_;
  std::string date_;
  std::string host_;
  bool bodyValidation_;
};

} // namespace curlinho

#endif // CURLINHO_HMAC_H
