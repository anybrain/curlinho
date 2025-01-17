/*
 * Created by Serafim Pinto on 2019-06-05.
 * Copyright (c) 2019 Anybrain. All rights reserved.
 */
#include "curlinho/curlinho.h"

#include <iostream>

int main() {

  using namespace curlinho;

  RetryPolicy retries;
  retries.numRetries = 3;
  retries.minDelay = 5;
  retries.maxDelay = 10;
  retries.maxBackOff = 40;

  curlinho::SetDefaults(
          Url{"https://httpstat.us/"},
          Headers{
              {"Content-type", "application/json"},
              {"Accept", "application/json"}
              },
          Timeout{30000},
          ProtocolVersion{curlinho::HTTP::v2},
          retries
      );
  Response res = curlinho::Get("425");
  std::cout << res.text << std::endl;
  for (auto item : res.headers) {
    std::cout << item.first << " - " << item.second << std::endl;
  }

  curlinho::PostDetach("", curlinho::Body{""});

  std::thread([]() {
      Response res = Post("200", Body{""});
      std::cout << res.text << std::endl;
      for (auto item : res.headers) {
          std::cout << item.first << " - " << item.second << std::endl;
      }
  }).detach();
  CRL_SLEEP(2);
  return 0;
}
