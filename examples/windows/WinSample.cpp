/*
 * Created by José Luis Silva on 2019-07-05.
 * Copyright (c) 2019 Deeep. All rights reserved.
 */
#include "deeep/DeeepAnalytics.h"

#include <Windows.h> // sleep()

int main() {
  deeep::DeeepAnalytics &deeep = deeep::DeeepAnalytics::getInstance();
  deeep.configureUsername("serafim");

  deeep.initialize("c2e3daa7-2759-41b6-89f4-63bf26ed6099", "gcsvVYpgtx46k07XY8UbUqgd1kzlihxl");

  deeep.start();
  deeep.start(); //second call to test if it is catch and ignored

  Sleep(5 * 1000);

  deeep.stop();

  Sleep(5 * 1000);

  deeep.start();

  Sleep(60 * 60 * 1000);

  deeep.stop();

  return 0;
}
