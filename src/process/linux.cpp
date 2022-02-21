#include <sys/uio.h>

#include <bit>
#include <fstream>
#include <iostream>

#include "process.hpp"

Napi::Value Process::GetBaseAddr(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  std::string path = "/proc/" + std::to_string(this->pid) + "/maps";
  std::ifstream maps{path};
  std::string content;
  maps >> content;
  std::string baseAddrStr = content.substr(0, content.find('-'));
  int64_t baseAddr = std::stol(baseAddrStr, nullptr, 16);
  return Napi::BigInt::New(env, baseAddr);
}

Napi::Value Process::ReadMemory(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  int length = info.Length();

  if (length < 2) {
    Napi::TypeError::New(env, "Arguments are wrong")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsBigInt()) {
    Napi::TypeError::New(env, "Address in BigInt expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[1].IsString()) {
    Napi::TypeError::New(env, "Type in string expected")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  bool lossless;
  long addr = info[0].As<Napi::BigInt>().Int64Value(&lossless);
  std::string type = info[1].As<Napi::String>().Utf8Value();

  int len = 0;

  if (type == "INT") {
    len = sizeof(int);
  }

  struct iovec local[1];
  struct iovec remote[1];
  char space[len];

  local[0].iov_base = space;
  local[0].iov_len = len;
  remote[0].iov_base = (void*)addr;
  remote[0].iov_len = len;

  size_t readLen = process_vm_readv(this->pid, local, 1, remote, 1, 0);

  if (std::endian::native == std::endian::little) {
    for (size_t i = 0; i < readLen / 2; i++) {
      size_t j = readLen - 1 - i;
      char val = space[i];
      space[i] = space[j];
      space[j] = val;
    }
  }

  if (type == "INT") {
    int result = 0;
    for (size_t i = 0; i < readLen; i++) {
      result |= space[i] << (sizeof(int) - 1 - i) * 8;
    }
    return Napi::Number::New(env, result);
  }
  return Napi::Number::New(env, 0);
}
