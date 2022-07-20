#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <stdio.h>
#include <iostream>

int main() {
  kern_return_t kern_return = 0;
  mach_port_name_t task;
  kern_return = task_for_pid(mach_task_self(), 19809, &task);
  if (kern_return != KERN_SUCCESS) {
    printf("task for pid");
    return -1;
  }

  uintptr_t buf;
  mach_msg_type_number_t readLen = 0;
  kern_return = vm_read(task, 0x16f10f624, 4, (vm_offset_t*)&buf, &readLen);

  std::cout << (*(int*)buf) << std::endl;
  return 0;
}