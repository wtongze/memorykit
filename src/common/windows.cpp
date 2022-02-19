#include <napi.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <comdef.h>
#include "common.hpp"

Napi::Array getProcesses(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::Array processes = Napi::Array::New(env);
  uint32_t index = 0;

  HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  PROCESSENTRY32 pEntry;
  pEntry.dwSize = sizeof(pEntry);

  Process32First(hProcessSnapshot, &pEntry);

  do
  {
    _bstr_t name(pEntry.szExeFile);
    std::string a {name};
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("pid", Napi::Number::New(env, static_cast<int>(pEntry.th32ProcessID)));
    obj.Set("ppid", Napi::Number::New(env, static_cast<int>(pEntry.th32ParentProcessID)));
    obj.Set("name", Napi::String::New(env, a));
    processes.Set(index, obj);
    index++;
  } while (Process32Next(hProcessSnapshot, &pEntry));

  CloseHandle(hProcessSnapshot);

  return processes;
}
