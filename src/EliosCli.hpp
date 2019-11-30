/*
 * Copyright - 2019 GASTALDI Rémi
 * Licensed under the GPLv2 License. See License.txt in the project root for license information.
 */

#pragma once

#include "Config.hpp"

#include <array>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <fstream>

class EliosCli {
public:
  EliosCli() = default;

  void loadConfig(std::string_view path) {
    std::array<char, 256> absolutePath{0};

    realpath(path.data(), absolutePath.data());
    _pwd = absolutePath.data();
    _pwd.append("/");
    _config.loadConfigFile(_pwd + "elios.yml");

    _appName = _config.get("name");
  }
  
  bool loadJson(std::string_view path) {
    std::array<char, 256> absolutePath{0};

    realpath(path.data(), absolutePath.data());
    _pwd = absolutePath.data();
    _pwd.append("/config.json");

    std::ifstream ifs(_pwd.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    if (!ifs.is_open()) {
      std::cout << "Config not found" << '\n';
      return false;
    }
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);

    ifs.read(bytes.data(), fileSize);
    _json = std::string(bytes.data(), fileSize);

    return true;
  }

  void setName(std::string appName) noexcept { _appName = std::move(appName); }

  void runDev() noexcept {
    if (_appName.empty()) {
      std::cerr << "Error: Application name is not set, check your elios.yml" << '\n';
      return;
    }
    if (_pwd.empty()) {
      std::cerr << "Error: path unknow" << '\n';
      return;
    }

    // Check existing containers
    bool existing{_isContainerExist()};

    if (existing) {
      _stopAndDeleteContainer();
    }

    // Check and save old image ID
    std::string oldImagedId{_imageId()};

    if (!oldImagedId.empty()) {
      oldImagedId.pop_back();
    }

    // Build new app image
    _buildDevImage();

    // Removing old image
    std::string newImageId{_imageId()};
    if (!newImageId.empty()) {
      newImageId.pop_back();
    }
    if (!oldImagedId.empty() && oldImagedId != newImageId) {
      std::cout << "Removing old " << _appName.data() << " image: " << oldImagedId << '\n';
      _deleteImage(oldImagedId);
    }

    // Run new container
    _runContainer();
  }

  void cleanApp() {
    if (_appName.empty()) {
      std::cerr << "Error: Application name is not set, check your elios.yml" << '\n';
      return;
    }

    if (_isContainerExist()) {
      _stopAndDeleteContainer();
    }
    _deleteImage();
  }

  void images() { _exec(" docker images | grep \"dev/\" | cut -d \" \" -f1", true); }

  void publish() { 
    _exec("xdg-open  https://dev.elios-mirror.com/modules/import?json=" + _urlEncode(_json));
  }
  // void buildProd() noexcept { std::cout << path << '\n'; }

private:
  std::string _urlEncode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
      std::string::value_type c = (*i);
      // Keep alphanumeric and other accepted characters intact
      if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
        escaped << c;
        continue;
      }
      // Any other characters are percent-encoded
      escaped << std::uppercase;
      escaped << '%' << std::setw(2) << int((unsigned char)c);
      escaped << std::nouppercase;
    }

    return escaped.str();
  }

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

  void _stopAndDeleteContainer() noexcept {
    std::cout << "Stop and deleting container" << '\n';
    _exec("docker stop dev-" + _appName);

    _exec("docker rm dev-" + _appName);
  }

  bool _isContainerExist() noexcept {
    std::string container{
        _exec("docker ps -a -f name=dev-" + _appName + " | grep -w dev-" + _appName)};
    return !container.empty();
  }

  void _buildDevImage() noexcept {
    _exec("docker build --tag dev/" + _appName + ":latest -f " + _pwd + "/Dockerfile_dev " + _pwd,
          true);
  }

  std::string _imageId() noexcept { return _exec("docker images -q dev/" + _appName); }

  void _deleteImage(const std::string &appName) noexcept {
    std::cout << "Deleting image" << '\n';
    _exec("docker rmi dev/" + appName + ":latest");
  }

  void _deleteImage() noexcept { _deleteImage(_appName); }

  void _runContainer() noexcept {
    _exec("docker run -it --mount type=bind,source=" + _pwd +
              "src,target=/opt/app/src/ --mount "
              "type=bind,source=/tmp/elios_mirror,target=/tmp/elios_mirror --name dev-" +
              _appName + " dev/" + _appName + ":latest",
          true);
  }

private:
  Config _config;
  std::string _pwd;
  std::string _appName;
  std::string _json;
};