/*
 * Copyright - 2019 GASTALDI Rémi
 * Licensed under the GPLv2 License. See License.txt in the project root for license information.
 */

#pragma once

#include "yaml-cpp/yaml.h"

#include <iostream>
#include <string>
#include <unordered_map>

class Config {
public:
  explicit Config(const std::string &configPath) {
    YAML::Node config;
    try {
      config = YAML::LoadFile(configPath);
    } catch (YAML::BadFile &e) {
      std::cerr << "File not found or bad format" << '\n';
      return;
    }
    const auto appName = config["name"];
    std::cout << appName << '\n';

  }

private:
  std::unordered_map<std::string, std::string> _config;
};