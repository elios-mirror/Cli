/*
 * Copyright - 2019 GASTALDI Rémi
 * Licensed under the GPLv2 License. See License.txt in the project root for license information.
 */

#include "EliosCli.hpp"

#include <iostream>

int main(int ac, char *av[]) {
  (void)ac;
  (void)av;

  if (ac < 2) {
    // TODO: print help
    std::cout << "Help" << '\n';
    return 1;
  }

  EliosCli elCli{av[1]};

  elCli.runDev();

  std::cout << "Exiting cli" << '\n';

  return 0;
}