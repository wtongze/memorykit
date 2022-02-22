#pragma once

#include <napi.h>

class Process : public Napi::ObjectWrap<Process> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Process(const Napi::CallbackInfo& info);

  Napi::Value GetBaseAddr(const Napi::CallbackInfo& info);
  Napi::Value ReadInt(const Napi::CallbackInfo& info);
  void WriteInt(const Napi::CallbackInfo& info);

 private:
  size_t ReadMemory(void* addr, char* space, size_t len);
  size_t WriteMemory(void* addr, char* space, size_t len);

  int handle;
  pid_t pid;
};
