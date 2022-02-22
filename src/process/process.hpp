#pragma once

#include <napi.h>

class Process : public Napi::ObjectWrap<Process> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Process(const Napi::CallbackInfo& info);

  Napi::Value GetBaseAddr(const Napi::CallbackInfo& info);
  Napi::Value ReadMemory(const Napi::CallbackInfo& info);
  void WriteMemory(const Napi::CallbackInfo& info);

 private:
  template <typename T>
  T Read(uint64_t addr);
  template <typename T>
  void Write(uint64_t addr, T val);

  int handle;
  pid_t pid;
};
