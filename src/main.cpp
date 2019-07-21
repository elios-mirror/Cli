/*
 * Copyright - 2019 GASTALDI Rémi
 * Licensed under the GPLv2 License. See License.txt in the project root for license information.
 */

#include "clara.hpp"

#include <iostream>

int main(int ac, char *av[]) {
  (void)ac;
  (void)av;

  int width = 0;
  int height = 0;
  bool help = false;
  auto cli = clara::Opt(width, "width")["-w"]["--width"]("How wide should it be?") |
             clara::Opt(height, "height")["-h"]["--height"]("How wide should it be?") |
             clara::Opt([](const std::string &val){
               std::cout << "======================= " << val  << std::endl;
             }, "height")["-g"]["--greed"]("How wide should it be?") |
             clara::Help(help);

  auto result = cli.parse({"", "--width", "23", "--height", "22", "--greed", "asd"});
  if (!result) {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    return 1;
  }

  if (help) {
    std::cout << cli << std::endl;
    return 0;
  }

  std::cout << "Width " << width << "\n";
  std::cout << "Height " << height << "\n";
  std::cout << "Help " << help << "\n";

  return 0;
}