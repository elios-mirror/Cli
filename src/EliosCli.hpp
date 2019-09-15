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
  explicit EliosCli(std::string pwd) : _pwd{std::move(pwd)} {
    std::array<char, 256> absolutePath{0};

    realpath(_pwd.data(), absolutePath.data());
    _pwd = absolutePath.data();
    _pwd.append("/");
    _config.loadConfigFile(_pwd + "elios.yml");
  }

  void runDev() noexcept {
    auto appName{_config.get("name")};

    if (appName.empty()) {
      std::cerr << "Application name is not set, check your elios.yml" << '\n';
      return;
    }

    // Check existing containers
    std::string cmd{"docker ps -a -f name=dev-"};
    cmd.append(appName);
    cmd.append(" | grep -w dev-");
    cmd.append(appName);

    std::string container{_exec(cmd)};

    // if (container.empty()) {
    //   return;
    // }

    if (!container.empty()) {
      std::cout << "Stop and delete container" << '\n';
      cmd = "docker stop dev-";
      cmd.append(appName);
      _exec(cmd);

      cmd = "docker rm dev-";
      cmd.append(appName);
      _exec(cmd);
    }

    // Check and save old image ID
    std::string imageIdCmd{"docker images -q dev/"};
    imageIdCmd.append(appName);

    std::string oldImagedId{_exec(imageIdCmd)};

    if (!oldImagedId.empty()) {
      oldImagedId.pop_back();
    }

    std::cout << "Old image ID: " << oldImagedId << '\n';

    // Build new app image
    cmd = "docker build --tag dev/";
    cmd.append(appName);
    cmd.append(":latest -f ");
    cmd.append(_pwd);
    cmd.append("/Dockerfile_dev ");
    cmd.append(_pwd);

    _exec(cmd, true);

    // Removing old image
    std::string newImageId{_exec(imageIdCmd)};
    if (!newImageId.empty()) {
      newImageId.pop_back();
    }
    std::cout << "-> " << oldImagedId << " : " << newImageId << '\n';
    if (!oldImagedId.empty() && oldImagedId != newImageId) {
      std::cout << "Removing old " << appName.data() << " image: " << oldImagedId << '\n';
      cmd = "docker rmi ";
      cmd.append(oldImagedId);

      _exec(cmd);
    }

    // Run new container
    cmd = "docker run -it --mount type=bind,source=";
    cmd.append(_pwd);
    cmd.append("src,target=/opt/app/src/ --mount "
               "type=bind,source=/tmp/elios_mirror,target=/tmp/elios_mirror --name dev-");
    cmd.append(appName);
    cmd.append(" dev/");
    cmd.append(appName);
    cmd.append(":latest");

    _exec(cmd, true);
  }

  void buildProd(std::string_view path) noexcept { std::cout << path << '\n'; }

private:
  std::string _exec(std::string_view cmd, bool standardOutput = false) noexcept {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.data(), "r"), pclose);

    if (!pipe) {
      std::cerr << "Popen failed'\n";
      return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      result += buffer.data();
      if (standardOutput) {
        std::cout << buffer.data();
      }
    }
    return result;
  }

  Config _config;
  std::string _pwd;
};