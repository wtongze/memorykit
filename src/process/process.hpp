#pragma once

#include <napi.h>

class Process : public Napi::ObjectWrap<Process> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Process(const Napi::CallbackInfo& info);

 private:
  Napi::Value GetBaseAddr(const Napi::CallbackInfo& info);
  Napi::Value ReadMemory(const Napi::CallbackInfo& info);
  // Napi::Value WriteMemory(const Napi::CallbackInfo& info);

  int handle;
  pid_t pid;
};
