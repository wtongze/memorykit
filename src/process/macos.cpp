#include <mach/mach.h>
#include <stdio.h>

#include "process.hpp"

Napi::Value Process::GetBaseAddr(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  kern_return_t kern_return = 0;
  mach_port_t task = MACH_PORT_NULL;

  kern_return = task_for_pid(mach_task_self(), pid, &task);

  if (kern_return != KERN_SUCCESS) {
    Napi::TypeError::New(env, "task_for_pid() failed")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  vm_address_t addr = 0;
  vm_size_t size = 0;
  vm_region_basic_info_data_64_t basic_info;
  mach_msg_type_number_t count = VM_REGION_BASIC_INFO_COUNT_64;
  mach_port_t object = MACH_PORT_NULL;

  kern_return = vm_region_64(task, &addr, &size, VM_REGION_BASIC_INFO_64,
                             (vm_region_info_t)&basic_info, &count, &object);

  if (kern_return != KERN_SUCCESS) {
    Napi::TypeError::New(env, "vm_region_64() failed")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  return Napi::BigInt::New(env, static_cast<uint64_t>(addr));
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

  size_t readLen = vm_read(this->pid, local, 1, remote, 1, 0);

  if (len != readLen) {
    Napi::Error::New(env, "Read length mismatch").ThrowAsJavaScriptException();
    return;
  }

  T result;
  memcpy(&result, &output, sizeof(result));

  return result;
}
