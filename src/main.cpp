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
  if (ac == 3) {
    _pwd = av[2];
  }
  
  EliosCli elCli{_pwd};

  if (std::strcmp(av[1], "run") == 0) {
    elCli.runDev();
  } else {
    std::cout << "Bad command" << '\n';
  }

  std::cout << "Exiting cli" << '\n';

  return 0;
}