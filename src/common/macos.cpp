#include "common.hpp"
#include "libproc.h"

Napi::Array getProcesses(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Array processes = Napi::Array::New(env);
  uint32_t index = 0;

  pid_t pids[2088];
  int procSize = proc_listallpids(pids, sizeof(pids));
  for (int i = 0; i < procSize; i++) {
    pid_t pid = pids[i];
    struct proc_bsdinfo procInfo;
    int resultSize = proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &procInfo,
                                  PROC_PIDTBSDINFO_SIZE);
    if (resultSize == PROC_PIDTBSDINFO_SIZE) {
      pid_t ppid = procInfo.pbi_ppid;
      std::string comm{procInfo.pbi_comm};
      std::string name{procInfo.pbi_name};

      Napi::Object obj = Napi::Object::New(env);
      obj.Set("pid", Napi::Number::New(env, pid));
      obj.Set("ppid", Napi::Number::New(env, ppid));
      obj.Set("name", Napi::String::New(env, name.empty() ? comm : name));
      processes.Set(index, obj);
      index++;
    }
  }

  return processes;
}
