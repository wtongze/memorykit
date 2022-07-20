#include <sys/uio.h>

#include <bit>
#include <fstream>
#include <iostream>
#include <type_traits>

#include "process.hpp"

Napi::Value Process::GetBaseAddr(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  std::string path = "/proc/" + std::to_string(pid) + "/maps";
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
  uint8_t output[len];

  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = output;
  local[0].iov_len = len;
  remote[0].iov_base = (void*)addr;
  remote[0].iov_len = len;

  size_t readLen = process_vm_readv(this->pid, local, 1, remote, 1, 0);

  if (len != readLen) {
    Napi::Error::New(env, "Read length mismatch").ThrowAsJavaScriptException();
    return;
  }

  T result;
  memcpy(&result, &output, sizeof(result));

  return result;
}

template <typename T>
void Process::Write(uint64_t addr, T val) {
  size_t len = sizeof(T);
  uint8_t input[len];

  memcpy(&input, &val, sizeof(val));

  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = input;
  local[0].iov_len = len;
  remote[0].iov_base = (void*)addr;
  remote[0].iov_len = len;

  size_t writeLen = process_vm_writev(pid, local, 1, remote, 1, 0);

  if (len != writeLen) {
    Napi::Error::New(env, "Write length mismatch").ThrowAsJavaScriptException();
    return;
  }
}
