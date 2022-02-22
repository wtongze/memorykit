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

size_t Process::ReadMemory(void* addr, char* space, size_t len) {
  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = space;
  local[0].iov_len = len;
  remote[0].iov_base = addr;
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
  return readLen;
}

size_t Process::WriteMemory(void* addr, char* space, size_t len) {
  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = space;
  local[0].iov_len = len;
  remote[0].iov_base = addr;
  remote[0].iov_len = len;

  if (std::endian::native == std::endian::little) {
    for (size_t i = 0; i < len / 2; i++) {
      size_t j = len - 1 - i;
      char val = space[i];
      space[i] = space[j];
      space[j] = val;
    }
  }

  size_t writeLen = process_vm_writev(this->pid, local, 1, remote, 1, 0);
  return writeLen;
}

Napi::Value Process::ReadInt(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  bool lossless;
  uint64_t addr = info[0].As<Napi::BigInt>().Uint64Value(&lossless);

  if (!lossless) {
    Napi::RangeError::New(env, "Address out of range")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  size_t len = sizeof(int);
  char output[len];
  size_t readLen = ReadMemory((void*)addr, output, len);

  if (len != readLen) {
    Napi::RangeError::New(env, "Can't read int").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  int result = 0;
  for (size_t i = 0; i < readLen; i++) {
    result |= output[i] << (len - 1 - i) * 8;
  }
  return Napi::Number::New(env, result);
}

void Process::WriteInt(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  bool lossless;
  uint64_t addr = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
  int value = info[1].As<Napi::Number>().Int32Value();

  if (!lossless) {
    Napi::RangeError::New(env, "Address out of range")
        .ThrowAsJavaScriptException();
    return;
  }

  size_t len = sizeof(int);
  char input[len];
  for (size_t i = 0; i < len; i++) {
    input[i] = value >> (len - 1 - i) * 8;
  }

  size_t writeLen = WriteMemory((void*)addr, input, len);

  if (len != writeLen) {
    Napi::RangeError::New(env, "Can't write int").ThrowAsJavaScriptException();
  }
}
