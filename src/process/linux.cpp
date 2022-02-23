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
  uint8_t output[len];

  struct iovec local[1];
  struct iovec remote[1];

  local[0].iov_base = output;
  local[0].iov_len = len;
  remote[0].iov_base = (void*)addr;
  remote[0].iov_len = len;

  size_t readLen = process_vm_readv(this->pid, local, 1, remote, 1, 0);

  if (len != readLen) {
    throw std::runtime_error("Read length mismatch");
  }

  T result;
  memcpy(&result, &output, sizeof(result));

  return result;
}

Napi::Value Process::ReadMemory(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  uint64_t addr;
  if (info[0].IsBigInt()) {
    bool lossless;
    addr = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    if (!lossless) {
      Napi::RangeError::New(env, "Address out of range")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  } else {
    addr = info[0].As<Napi::Number>().Int64Value();
  }

  std::string type = info[1].As<Napi::String>().Utf8Value();

  if (type == "BYTE") {
    return Napi::Number::New(env, Read<uint8_t>(addr));
  } else if (type == "INT") {
    return Napi::Number::New(env, Read<int>(addr));
  } else if (type == "INT32") {
    return Napi::Number::New(env, Read<int32_t>(addr));
  } else if (type == "UINT32") {
    return Napi::Number::New(env, Read<uint32_t>(addr));
  } else if (type == "INT64") {
    return Napi::BigInt::New(env, Read<int64_t>(addr));
  } else if (type == "UINT64") {
    return Napi::BigInt::New(env, Read<uint64_t>(addr));
  } else if (type == "SHORT") {
    return Napi::Number::New(env, Read<short>(addr));
  } else if (type == "LONG") {
    return Napi::BigInt::New(env, Read<long>(addr));
  } else if (type == "FLOAT") {
    return Napi::Number::New(env, Read<float>(addr));
  } else if (type == "DOUBLE") {
    return Napi::Number::New(env, Read<double>(addr));
  } else if (type == "POINTER") {
    return Napi::BigInt::New(env, Read<uintptr_t>(addr));
  } else if (type == "BOOL") {
    return Napi::Boolean::New(env, Read<bool>(addr));
  } else if (type == "DWORD") {
    Napi::TypeError::New(env, "Can't read DWORD on linux.")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  } else if (type == "STRING") {
    std::string result;
    while (1) {
      char l = Read<char>(addr);
      if (l == '\0') {
        break;
      } else {
        result.push_back(l);
        addr++;
      }
    }
    return Napi::String::New(env, result);
  } else {
    Napi::TypeError::New(env, "Can't read type " + type + " from memory")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
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
