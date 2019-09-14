/*
 * Copyright - 2019 GASTALDI Rémi
 * Licensed under the GPLv2 License. See License.txt in the project root for license information.
 */

#pragma once

#include "Config.hpp"

#include <cstdio>

#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

class EliosCli {
public:
  EliosCli() : _config{"/home/gastal_r/Projects/Elios/elios-cli/calendar/elios.yml"} {}

  void runDev(std::string_view path) noexcept {}

  void buildProd(std::string_view path) noexcept { std::cout << path << '\n'; }

private:
  std::string _exec(const char *cmd) noexcept {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

    if (!pipe) {
      // throw std::runtime_error("popen() failed!");
      std::cerr << "Popen failed'\n";
      return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      result += buffer.data();
    }
    return result;
  }

  Config _config;
};