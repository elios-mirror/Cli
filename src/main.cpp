/*
 * Copyright - 2019 GASTALDI Rémi
 * Licensed under the GPLv2 License. See License.txt in the project root for license information.
 */

#include "EliosCli.hpp"

#include <cstring>
#include <iostream>

void printHelp(void) {
  std::cout << "Usage: elios [option] [argument]\n\nOptions:\n";
  std::cout << "run:        run an application in containerised development mode.\n arg:        path of the app\n";
  std::cout << "clean:      all container linked to the application. \n arg:        path of the app\n";
  std::cout << "image:      list all elios applications images. \n";
  std::cout << "publish:    publish the application to the elios store. \n";
}

int main(int ac, char *av[]) {
  std::string _pwd{"."};

  if (ac < 2) {
    printHelp();
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
  } else if (std::strcmp(av[1], "images") == 0) {
    elCli.images();
  } else if (std::strcmp(av[1], "publish") == 0) {
    if (ac == 3) {
      _pwd = av[2];
    }
    if (elCli.loadJson(_pwd)) {
      elCli.publish();
    }
  } else {
    printHelp();
    return 1;
  }

  std::cout << "Exiting cli" << '\n';
  return 0;
}