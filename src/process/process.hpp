#pragma once

#include <napi.h>

class Process : public Napi::ObjectWrap<Process> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  Process(const Napi::CallbackInfo& info);

  void AquireProcess();
  Napi::Value GetBaseAddr(const Napi::CallbackInfo& info);
  Napi::Value ReadMemory(const Napi::CallbackInfo& info);
  void WriteMemory(const Napi::CallbackInfo& info);

 private:
  void Read(uint64_t addr, void* target, size_t len);
  void Write(uint64_t addr, void* source, size_t len);
  int pid;
  uint64_t handle = 0; // For Windows
  unsigned int task = 0; // For MacOS
};
