#include <mach/mach.h>
#include <stdio.h>

#include "process.hpp"

void Process::AquireProcess() {
  kern_return_t kern_return = 0;

  kern_return = task_for_pid(mach_task_self(), pid, &this->task);
  if (kern_return != KERN_SUCCESS) {
    throw std::runtime_error("AquireProcess failed");
  }
}

Napi::Value Process::GetBaseAddr(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  kern_return_t kern_return = 0;

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

void Process::Read(uint64_t addr, void* target, size_t len) {
  vm_offset_t output;
  kern_return_t kern_return = 0;
  mach_msg_type_number_t readLen = 0;

  kern_return = vm_read(task, addr, len, &output, &readLen);

  if (len != static_cast<size_t>(readLen)) {
    throw std::runtime_error("Read length mismatch");
  }

  if (kern_return != KERN_SUCCESS) {
    throw std::runtime_error("vm_read() failed");
  }

  memcpy(target, (void*)output, len);
}

void Process::Write(uint64_t addr, void* source, size_t len) {
  kern_return_t kern_return = 0;
  kern_return = vm_write(task, addr, (vm_offset_t)source, (mach_msg_type_number_t)len);

  if (kern_return != KERN_SUCCESS) {
    throw std::runtime_error("vm_write() failed");
  }
}
