/*
* Created by Miguel on 2019-08-20.
* Copyright (c) 2019 Deeep. All rights reserved.
*/

#include "catch2/catch.hpp"

#include "nlohmann/json.hpp"
#include "deeep/DeeepApi.h"
#include "cpr/cpr.h"

#include <chrono>

TEST_CASE("Success Response Test") {
    nlohmann::json j;
    std::string url = "https://httpstat.us/200";
    std::thread(&deeep::DeeepApi::postThreadFunction, &deeep::DeeepApi::getInstance(), j, url ).join();
    std::vector<cpr::Response> responseHistory = deeep::DeeepApi::getInstance().getResponseHistory();
    REQUIRE(responseHistory.size() == 1);
    REQUIRE(responseHistory.at(0).status_code == 200);
}

TEST_CASE("Multiple Success Response Test") {
  nlohmann::json j;
  std::string url = "https://httpstat.us/200";
  for(int i = 0; i < 4; ++i){
    std::thread(&deeep::DeeepApi::postThreadFunction, &deeep::DeeepApi::getInstance(), j, url ).join();
    std::vector<cpr::Response> responseHistory = deeep::DeeepApi::getInstance().getResponseHistory();
    REQUIRE(responseHistory.size() == 1);
    REQUIRE(responseHistory.at(0).status_code == 200);
  }
}

TEST_CASE("Non Recoverable Response Test") {
  nlohmann::json j;
  std::string url = "https://httpstat.us/404";
  std::thread(&deeep::DeeepApi::postThreadFunction, &deeep::DeeepApi::getInstance(), j, url ).join();
  std::vector<cpr::Response> responseHistory = deeep::DeeepApi::getInstance().getResponseHistory();
  REQUIRE(responseHistory.size() == 1);
  REQUIRE(responseHistory.at(0).status_code == 404);
}

TEST_CASE("Multiple Non Recoverable Response Test") {
  nlohmann::json j;
  std::string url = "https://httpstat.us/404";
  for(int i = 0; i < 4; ++i) {
    std::thread(&deeep::DeeepApi::postThreadFunction, &deeep::DeeepApi::getInstance(), j, url).join();
    std::vector<cpr::Response> responseHistory = deeep::DeeepApi::getInstance().getResponseHistory();
    REQUIRE(responseHistory.size() == 1);
    REQUIRE(responseHistory.at(0).status_code == 404);
  }
}


TEST_CASE("Recoverable Response Test") {
  nlohmann::json j;
  std::string url = "https://httpstat.us/503";
  auto start = std::chrono::steady_clock::now();
  std::thread(&deeep::DeeepApi::postThreadFunction, &deeep::DeeepApi::getInstance(), j, url ).join();
  auto end = std::chrono::steady_clock::now();
  std::vector<cpr::Response> responseHistory = deeep::DeeepApi::getInstance().getResponseHistory();
  REQUIRE(responseHistory.size() == 4);
  for(auto r : responseHistory){
    REQUIRE(r.status_code == 503);
  }
  REQUIRE(std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 70);
}

TEST_CASE("Multiple Recoverable Response Test") {
  nlohmann::json j;
  std::string url = "https://httpstat.us/503";
  for(int i = 0; i < 4; ++i) {
    auto start = std::chrono::steady_clock::now();
    std::thread(&deeep::DeeepApi::postThreadFunction, &deeep::DeeepApi::getInstance(), j, url ).join();
    auto end = std::chrono::steady_clock::now();
    std::vector<cpr::Response> responseHistory = deeep::DeeepApi::getInstance().getResponseHistory();
    REQUIRE(responseHistory.size() == 4);
    for(auto r : responseHistory){
      REQUIRE(r.status_code == 503);
    }
    REQUIRE(std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 70);
  }
}

TEST_CASE("Timeout Response Test") {
  nlohmann::json j;
  std::string url = "https://httpstat.us/503?sleep=50000";
  auto start = std::chrono::steady_clock::now();
  std::thread(&deeep::DeeepApi::postThreadFunction, &deeep::DeeepApi::getInstance(), j, url ).join();
  auto end = std::chrono::steady_clock::now();
  std::vector<cpr::Response> responseHistory = deeep::DeeepApi::getInstance().getResponseHistory();
  REQUIRE(responseHistory.size() == 4);
  for(auto r : responseHistory){
    REQUIRE(r.error.code == cpr::ErrorCode::OPERATION_TIMEDOUT);
  }
  REQUIRE(std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 70);
}

TEST_CASE("Multiple Timeout Response Test") {
  nlohmann::json j;
  std::string url = "https://httpstat.us/503?sleep=50000";
  for(int i = 0; i < 4; ++i) {
    auto start = std::chrono::steady_clock::now();
    std::thread(&deeep::DeeepApi::postThreadFunction, &deeep::DeeepApi::getInstance(), j, url ).join();
    auto end = std::chrono::steady_clock::now();
    std::vector<cpr::Response> responseHistory = deeep::DeeepApi::getInstance().getResponseHistory();
    REQUIRE(responseHistory.size() == 4);
    for(auto r : responseHistory){
      REQUIRE(r.error.code == cpr::ErrorCode::OPERATION_TIMEDOUT);
    }
    REQUIRE(std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 70);
  }
}