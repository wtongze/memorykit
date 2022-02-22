#include <sys/uio.h>

#include <bit>
#include <fstream>
#include <iostream>
#include <type_traits>

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

template <typename T>
T Process::Read(uint64_t addr) {
  size_t len = sizeof(T);
  char output[len];

  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = output;
  local[0].iov_len = len;
  remote[0].iov_base = (void*)addr;
  remote[0].iov_len = len;

  size_t readLen = process_vm_readv(this->pid, local, 1, remote, 1, 0);

  if (std::endian::native == std::endian::little) {
    for (size_t i = 0; i < readLen / 2; i++) {
      size_t j = readLen - 1 - i;
      char temp = output[i];
      output[i] = output[j];
      output[j] = temp;
    }
  }

  if (len != readLen) {
    throw std::runtime_error("Read length mismatch");
  }

  T result;
  if constexpr (std::is_same_v<T, int>) {
    result = 0;
  } else {
    result = -1;
  }

  for (size_t i = 0; i < len; i++) {
    result |= output[i] << (len - 1 - i) * 8;
  }

  return result;
}

Napi::Value Process::ReadMemory(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  bool lossless;
  uint64_t addr = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
  std::string type = info[1].As<Napi::String>().Utf8Value();

  if (!lossless) {
    Napi::RangeError::New(env, "Address out of range")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (type == "INT") {
    return Napi::Number::New(env, Read<int>(addr));
  }
  return env.Undefined();
}

template <typename T>
void Process::Write(uint64_t addr, T val) {
  size_t len = sizeof(T);
  char output[len];

  for (size_t i = 0; i < len; i++) {
    output[i] = val >> (len - 1 - i) * 8;
  }

  if (std::endian::native == std::endian::little) {
    for (size_t i = 0; i < len / 2; i++) {
      size_t j = len - 1 - i;
      char temp = output[i];
      output[i] = output[j];
      output[j] = temp;
    }
  }

  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = output;
  local[0].iov_len = len;
  remote[0].iov_base = (void*)addr;
  remote[0].iov_len = len;

  size_t writeLen = process_vm_writev(this->pid, local, 1, remote, 1, 0);

  if (len != writeLen) {
    throw std::runtime_error("Write length mismatch");
  }
}

void Process::WriteMemory(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  bool lossless;
  uint64_t addr = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
  std::string type = info[1].As<Napi::String>().Utf8Value();

  if (!lossless) {
    Napi::RangeError::New(env, "Address out of range")
        .ThrowAsJavaScriptException();
    return;
  }

  if (type == "INT") {
    int val = info[2].As<Napi::Number>().Int32Value();
    Write<int>(addr, val);
  }
}
