#include <napi.h>
#include "common/common.hpp"

using namespace Napi;

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "getProcesses"),
              Napi::Function::New(env, getProcesses));
  return exports;
}

NODE_API_MODULE(addon, Init)
