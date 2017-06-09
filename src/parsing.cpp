// Copyright 2016 Etix Labs
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "server.h"
#include <iostream>
#include <string.h>

// Set config defaults or from env
void parse_env(std::shared_ptr<t_config> config) {
  // Address
  config->address = DEFAULT_ADDRESS;
  if (const char *address = std::getenv("RTSP_ADDRESS")) {
    config->address = address;
  }

  // Port
  config->port = DEFAULT_PORT;
  if (const char *port = std::getenv("RTSP_PORT")) {
    config->port = port;
  }

  // Route
  config->route = DEFAULT_ROUTE;
  if (const char *route = std::getenv("RTSP_ROUTE")) {
    config->route = route;
  }

  // Username
  config->username = DEFAULT_USERNAME;
  if (const char *username = std::getenv("RTSP_USERNAME")) {
    config->username = username;
  }

  // Password
  config->password = DEFAULT_PASSWORD;
  if (const char *password = std::getenv("RTSP_PASSWORD")) {
    config->password = password;
  }

  // Framerate
  config->framerate = DEFAULT_FRAMERATE;
  if (const char *framerate = std::getenv("RTSP_FRAMERATE")) {
    config->framerate = framerate;
  }

  // Scale
  config->scale =
      std::make_pair<std::string, std::string>(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  if (const char *scale = std::getenv("RTSP_RESOLUTION")) {
    size_t pos = 0;
    std::string scale_str(scale);

    if ((pos = scale_str.find("x")) == std::string::npos) {
      std::cerr
          << "No x token found between width and height in the scale argument: "
          << scale_str << std::endl
          << "Using default values";
    } else {
      config->scale = std::make_pair<std::string, std::string>(
          scale_str.substr(0, pos), scale_str.substr(pos + 1));
    }
  }

  // Input
  config->input = DEFAULT_INPUT;
  if (const char *input = std::getenv("INPUT")) {
    config->input = input;
  }

  config->time = DEFAULT_TIME_ENABLED;
  if (const char *time = std::getenv("ENABLE_TIME_OVERLAY")) {
    if (strcmp(time, "false") == 0) {
      config->time = false;
    } else {
      config->time = true;
    }
  }
}

// Overwrite default parameters via cmd line
bool parse_args(std::shared_ptr<t_config> config, int argc, char **argv) {
  int c;
  opterr = 0;
  while ((c = getopt(argc, argv, "r:u:l:p:b:f:s:i:ht")) != -1) {
    switch (c) {
    case 'r': // Route
      if (optarg && optarg[0] == '-') {
        break;
      }
      if (not optarg[0] == '/') {
        config->route = "/";
      }
      config->route += optarg;
      break;
    case 'u': // Username
      if (optarg && optarg[0] == '-') {
        break;
      }
      config->username = optarg;
      break;
    case 'p': // Password
      if (optarg && optarg[0] == '-') {
        break;
      }
      config->password = optarg;
      break;
    case 'i': // Input
      if (optarg && optarg[0] == '-') {
        break;
      }
      config->input = optarg;
      break;
    case 'l': // Listen address
      if (optarg && optarg[0] == '-') {
        break;
      }
      config->address = optarg;
      break;
    case 'b': // Port
      if (optarg && optarg[0] == '-') {
        break;
      }
      config->port = optarg;
      break;
    case 'f': // Framerate
      if (optarg && optarg[0] == '-') {
        break;
      }
      config->framerate = optarg;
      break;
    case 's': { // Scale
      if (optarg && optarg[0] == '-') {
        break;
      }
      size_t pos = 0;
      std::string scale = optarg;
      if ((pos = scale.find("x")) == std::string::npos) {
        std::cerr << "No x token found between width and height in the scale "
                     "argument: "
                  << scale << std::endl
                  << "Using default values instead";
        return false;
      }
      config->scale.first = scale.substr(0, pos);
      config->scale.second = scale.substr(pos + 1);
      break;
    }
    case 't': // Time Overlay
      config->time = true;
      break;
    case 'h': // help
      fprintf(stdout,
              "Usage: %s [-l address] [-b port] [-r route] [-i "
              "input] [-u username] [-p password] [-f framerate] [-s "
              "'width'x'height'] [-t] [-h]\n",
              argv[0]);
      return true;
    case '?':
      if (optopt == 'r' || optopt == 'l' || optopt == 'p' || optopt == 'u' ||
          optopt == 'i' || optopt == 'a' || optopt == 'b' || optopt == 'f' ||
          optopt == 's') {
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      } else if (isprint(optopt)) {
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      } else {
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      }
      return false;
    default:
      return false;
      ;
    }
  }
  return true;
}
