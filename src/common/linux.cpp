#include <napi.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include "common.hpp"

Napi::Array getProcesses(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Array processes = Napi::Array::New(env);
  uint32_t index = 0;

  for (auto& item : std::filesystem::directory_iterator("/proc")) {
    if (item.is_directory()) {
      std::string fileName{item.path().filename().c_str()};
      if (std::regex_match(fileName, std::regex{"^\\d+$"})) {
        pid_t pid = std::stoi(fileName);
        pid_t ppid = -1;
        std::string comm;

        std::string itemPath{item.path()};

        std::ifstream commFile{itemPath + "/comm"};
        commFile >> comm;

        std::ifstream stat{itemPath + "/stat"};

        std::string rawArgs;
        std::getline(stat, rawArgs);
        std::vector<std::string> args;

        size_t pos = 0;
        while ((pos = rawArgs.find(' ')) != std::string::npos) {
          args.push_back(rawArgs.substr(0, pos));
          rawArgs.erase(0, pos + 1);
        }
        try {
          ppid = std::stoi(args[3]);
        } catch (std::exception& e) {
        }

        Napi::Object obj = Napi::Object::New(env);
        obj.Set("pid", Napi::Number::New(env, pid));
        obj.Set("ppid", ppid == -1 ? env.Undefined() : Napi::Number::New(env, ppid));
        obj.Set("name", comm.empty() ? env.Undefined() : Napi::String::New(env, comm));
        processes.Set(index, obj);
        index++;
      }
    }
  }
  return processes;
}