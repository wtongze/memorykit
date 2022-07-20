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

void Process::AquireProcess() {}

void Process::Read(uint64_t addr, void* target, size_t len) {
  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = target;
  local[0].iov_len = len;
  remote[0].iov_base = (void*)addr;
  remote[0].iov_len = len;

  size_t readLen = process_vm_readv(pid, local, 1, remote, 1, 0);

  if (len != readLen) {
    throw std::runtime_error("Read length mismatch");
  }
}

void Process::Write(uint64_t addr, void* source, size_t len) {
  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = source;
  local[0].iov_len = len;
  remote[0].iov_base = (void*)addr;
  remote[0].iov_len = len;

  size_t writeLen = process_vm_writev(pid, local, 1, remote, 1, 0);

  if (len != writeLen) {
    throw std::runtime_error("Write length mismatch");
  }
}
