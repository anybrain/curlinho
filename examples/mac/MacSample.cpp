/*
 * Created by Serafim Pinto on 2019-06-05.
 * Copyright (c) 2019 Deeep. All rights reserved.
 */
#include "curlinho/curlinho.h"

#include <iostream>

int main() {

  using namespace curlinho;

  RetryPolicy retries;
  retries.nr_retries = 3;
  retries.delays_= {4,4,2};
  curlinho::SetDefaults(
          Url{"https://www.google.pt/"},
          Headers{
              {"Content-type", "application/json"},
              {"User-agent", "JJ"}
              },
          Timeout{100},
          ProtocolVersion{curlinho::HTTP::v2},
          retries
      );

  Response res = curlinho::Get("");

  std::cout << res.status_code;

  curlinho::PostDetach("", curlinho::Body{""});

  std::thread([]() {
      Response res = Post("maps/", Body{""});
      std::cout << res.status_code << std::endl << res.text << std::endl;
      for (auto item : res.headers) {
          std::cout << item.first << " - " << item.second << std::endl;
      }
  }).detach();
  CRL_SLEEP(2);
  return 0;
}
