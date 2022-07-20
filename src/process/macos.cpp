#include <mach/mach.h>
#include <stdio.h>

#include "process.hpp"

Napi::Value Process::GetBaseAddr(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  kern_return_t kern_return = 0;
  mach_port_t task = MACH_PORT_NULL;

  kern_return = task_for_pid(mach_task_self(), this->pid, &task);

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

Napi::Value Process::ReadMemory(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return env.Undefined();
}

void Process::WriteMemory(const Napi::CallbackInfo& info) {}
