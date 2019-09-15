/*
 * Copyright - 2019 GASTALDI Rémi
 * Licensed under the GPLv2 License. See License.txt in the project root for license information.
 */

#include "EliosCli.hpp"

#include <cstring>
#include <iostream>

int main(int ac, char *av[]) {
  std::string _pwd{"."};

  if (ac < 2) {
    std::cout << "TODO: Help" << '\n';
    return 1;
  }

  EliosCli elCli;

  if (std::strcmp(av[1], "run") == 0) {
    if (ac == 3) {
      _pwd = av[2];
    }
    elCli.loadConfig(_pwd);
    elCli.runDev();
  } else if (std::strcmp(av[1], "clean") == 0) {
    if (ac < 3) {
      elCli.loadConfig(".");
      elCli.cleanApp();
    } else {
      elCli.setName(av[2]);
      elCli.cleanApp();
    }
  } else {
    std::cout << "Bad command" << '\n';
  }

  std::cout << "Exiting cli" << '\n';

  return 0;
}