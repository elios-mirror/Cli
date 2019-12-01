/*
 * Copyright - 2019 GASTALDI Rémi
 * Licensed under the GPLv2 License. See License.txt in the project root for license information.
 */

#pragma once

#include "Config.hpp"

#include <array>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include <curl/curl.h>

class EliosCli {
public:
  EliosCli() = default;

  bool loadConfig(std::string_view path) {
    std::array<char, 256> absolutePath{0};

    realpath(path.data(), absolutePath.data());
    _pwd = absolutePath.data();
    _pwd.append("/");

    if (!_config.loadConfigFile(_pwd + "elios.yml"))
      return false;

    _appName = _config.get("name");
    if (_appName.empty()) {
      std::cout << "Application name not found in elios.yml" << '\n';
      return false;
    }
    _appVersion = _config.get("version");
    if (_appVersion.empty()) {
      std::cout << "Application version not found in elios.yml" << '\n';
      return false;
    }
    return true;
  }

  bool loadJson(std::string_view path) {
    std::array<char, 256> absolutePath{0};

    realpath(path.data(), absolutePath.data());
    _pwd = absolutePath.data();
    _pwd.append("/variables.json");

    std::ifstream ifs(_pwd.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    if (!ifs.is_open()) {
      std::cout << "variables.json not found" << '\n';
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

  void images() { _exec("docker images | grep \"dev/\" | cut -d \" \" -f1", true); }

  static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
  }

  void publish() {
    _exec("xdg-open  https://dev.elios-mirror.com/modules/import?json=" + _urlEncode(_json) +
          "&name=" + _urlEncode(_appName) + "&version=" + _urlEncode(_appVersion));

    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
      std::string url{"https://dev.elios-mirror.com/api/checker/modules/" + _appName + "/" +
                      _appVersion + "/"};
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

      constexpr size_t minutes{10 * 60};
      for (size_t time = 0; time < minutes;) {
        res = curl_easy_perform(curl);
        std::this_thread::sleep_for(std::chrono::seconds(5));
        if (res != CURLE_OK) {
          std::cout << curl_easy_strerror(res) << '\n';
        } else {
          std::cout << readBuffer << std::endl;
          if (readBuffer == "true") {
            _publishDocker();
            break;
          }
          readBuffer.clear();
        }
        time += 5;
      }
      curl_easy_cleanup(curl);
    }
    // curl_global_cleanup();
  }

private:
  Config _config;
  std::string _pwd;
  std::string _appName;
  std::string _appVersion;
  std::string _json;

private:
  std::string _urlEncode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
      std::string::value_type c = (*i);
      if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
        escaped << c;
        continue;
      }
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

  void _publishDocker() noexcept {
    _exec("docker login -u eliosmirror -p Upy5zNkTnXhm8RDr", true);
    _exec("docker push eliosmirror/" + _appName + ":latest", true);
  }
};