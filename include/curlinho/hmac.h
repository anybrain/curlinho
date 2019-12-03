#include <utility>

#include <utility>

/*
 * Created by José Luís  Silva on 2019-11-29.
 * Copyright (c) 2019 Deeep. All rights reserved.
 */
#ifndef DEEEP_SDK_CPP_HMAC_H
#define DEEEP_SDK_CPP_HMAC_H

#include "plog/Log.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <string>
#include <time.h>

namespace curlinho {

class Hmac {
  public:
  Hmac() = default;
  Hmac(std::string username, std::string secret, std::string host)
      : username_{std::move(username)}, secret_{std::move(secret)}, host_{std::move(host)} {};

  bool empty() { return username_.empty() || secret_.empty() || host_.empty(); };

  void prepareSignature(const std::string &path, const std::string &method,
                        const std::string &body) {
    time_t rawtime;
    time(&rawtime);
    struct tm *time_info = gmtime(&rawtime);
    char date[50];
    strftime(date, 50, "%a, %d %b %Y %H:%M:%S GMT", time_info);
    date_ = std::string(date);
    std::string requestLine = method + " " + path + " HTTP/2.0";
    digest_ = "SHA-256=" + base64Encode(sha256_string(body));
    std::string stringToSign =
        "date: " + date_ + "\nhost: " + host_ + "\n" + requestLine + "\ndigest: " + digest_;

    // Using sha256 hash engine here.
    unsigned char *hmac_sha256 =
        HMAC(EVP_sha256(), (void *)secret_.c_str(), secret_.length(),
             (unsigned char *)stringToSign.c_str(), stringToSign.length(), NULL, NULL);
    char sign[32];
    for (int i = 0; i < 32; i++)
      sprintf(&sign[i * 2], "%02x", (unsigned int)hmac_sha256[i]);
    std::string signature = base64Encode(std::string(sign));

    authorization_ = "hmac username=\"" + username_ + "\"," + "algorithm=\"hmac-sha256\"," +
                     "headers=\"date host request-line digest\"," + "signature=\"" + signature +
                     "\"";
    PLOG_DEBUG << "date: " << date_;
    PLOG_DEBUG << "requestLine: " << requestLine;
    PLOG_DEBUG << "digest: " << digest_;
    PLOG_DEBUG << "Signature: " << signature;
    PLOG_DEBUG << "Authorization: " << authorization_;

    PLOG_DEBUG << "digest decoded: " << base64Decode(base64Encode(sha256_string(body)));
    PLOG_DEBUG << "sha256 string: " << sha256_string(body);
    PLOG_DEBUG << "body string: " << body;
  }

  Headers getHmacHeaders() {
    return Headers{
        {"Date", date_}, {"Authorization", authorization_}, {"Digest", digest_}, {"Host", host_}};
  }

  static inline std::string sha256_string(const std::string &text) {
    char *string = new char[text.length() + 1];
    strcpy(string, text.c_str());
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char outputBuffer[65];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(hash, &sha256);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
    return std::string(outputBuffer);
  }

  static inline std::string base64Encode(const std::string &in) {
    std::string out;
    int val = 0, valb = -6;
    for (char c : in) {
      val = (val << 8) + c;
      valb += 8;
      while (valb >= 0) {
        out.push_back(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) &
                                                                               0x3F]);
        valb -= 6;
      }
    }
    if (valb > -6) {
      out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
                        [((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (out.size() % 4) {
      out.push_back('=');
    }
    return out;
  }

  static inline std::string base64Decode(const std::string &in) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; ++i) {
      T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    }
    int val = 0, valb = -8;
    for (char c : in) {
      if (T[c] == -1) {
        break;
      }
      val = (val << 6) + T[c];
      valb += 6;
      if (valb >= 0) {
        out.push_back(char((val >> valb) & 0xFF));
        valb -= 8;
      }
    }
    return out;
  }

  private:
  std::string digest_;
  std::string authorization_;
  std::string username_;
  std::string secret_;
  std::string date_;
  std::string host_;
};

/*
static inline std::string hmacAuthString(const std::string &host, const std::string &path,
                                         const std::string &method, const std::string &username,
                                         const std::string &secret, const std::string &body) {
  time_t rawtime;
  time(&rawtime);
  struct tm *time_info = localtime(&rawtime);
  char date[50];
  strftime(date, 50, "%a, %d %b %Y %H:%M:%S %Z", time_info);
  std::string requestLine = method + " " + path + " HTTP/2.0";
  std::string digest = "SHA-256=" + base64Decode(sha256_string(body));
  std::stringstream stringToSign;
  stringToSign << "date: " << date
               << "\nhost: " + host + "\n" + requestLine + "\ndigest: " + digest;

  // Using sha256 hash engine here.
  std::string signature =
      base64Encode(sha256_string(std::string(reinterpret_cast<const char *>(HMAC(
          EVP_sha256(), (void *)secret.c_str(), secret.length(),
          (unsigned char *)stringToSign.str().c_str(), stringToSign.str().length(), NULL, NULL)))));
  PLOG_DEBUG << "date: " << date;
  PLOG_DEBUG << "requestLine: " << requestLine;
  PLOG_DEBUG << "digest: " << digest;
  PLOG_DEBUG << "Signature: " << signature;
  std::string authorization = "hmac username=\"" + username + "\"," + "algorithm=\"hmac-sha256\"," +
                              "headers=\"date host request-line digest\"," + "signature=\"" +
                              signature + "\"";
  return authorization;
}


static void hmacAuth() {
  var username = "2de92c66-3750-40aa-acf6-ebae6d885ebd";
  var secret = "43v6DLYKBhnE64hbEfv7o6zDajSnh9Am";
  var apiUrl = "https://deeep-storage.performetric.net";
  var host = "deeep-storage.performetric.net";
  var path = "/storage/swagger-ui.html";
  var method = "GET";
  Date currentDate = new Date();
  SimpleDateFormat format = new SimpleDateFormat("E, dd MMM yyyy HH:mm:ss z");
  format.setTimeZone(TimeZone.getTimeZone("UTC"));
  var date = format.format(currentDate);
  var requestLine = method + " " + path + " HTTP/1.1";
  // String body = "A small body";
  std::string body = "";
  // Create the Authorization header
  std::string digest = "SHA-256=" + base64(sha256(body.getBytes()));
  std::string stringToSign =
      "date: " + date + "\nhost: " + host + "\n" + requestLine + "\ndigest: " + digest;
  var signature = base64(hmacSha256(stringToSign, secret));
  var authorization = "hmac " + "username=\"" + username + "\"," + "algorithm=\"hmac-sha256\"," +
                      "headers=\"date host request-line digest\"," + "signature=\"" + signature +
                      "\"";
  System.out.println(date);
  System.out.println(requestLine);
  System.out.println(digest);
  System.out.println(signature);
  var headers = new HttpHeaders();
  headers.set("Date", date);
  headers.add("Host", host);
  headers.add("Digest", digest);
  headers.set("Authorization", authorization);
  var entity = new HttpEntity<>(headers);
  RestTemplate rest = new RestTemplate();
  // var response = rest.postForEntity(apiUrl + path, entity, String.class); // POST
  var response = rest.exchange(apiUrl + path, HttpMethod.GET, entity, String.class); // GET
  System.out.println(response.getStatusCodeValue());
}
 */

} // namespace curlinho

#endif // DEEEP_SDK_CPP_HMAC_H
