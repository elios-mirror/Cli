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
  Config() = default;

  explicit Config(const std::string &configPath) {
    loadConfigFile(configPath);
  }

  bool loadConfigFile(const std::string &configPath) {
    YAML::Node config;

    try {
      config = YAML::LoadFile(configPath);
    } catch (YAML::BadFile &) {
      std::cerr << "elios.yml not found or bad file";
      return false;
    }

    for (const auto &it : config) {
      _config[it.first.as<std::string>()] = it.second.as<std::string>();
    }

    return true;
  }

  std::string_view get(const std::string &id) const noexcept {
    auto elem = _config.find(id);

    if (elem != _config.cend()) {
      return elem->second;
    }
    return {};
  }

  const std::unordered_map<std::string, std::string> &get() const noexcept {
    return _config;
  }

private:
  std::unordered_map<std::string, std::string> _config;
};