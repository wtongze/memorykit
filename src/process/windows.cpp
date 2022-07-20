#include <windows.h>
#include <TlHelp32.h>
#include <comdef.h>
#include "process.hpp"

void Process::AquireProcess()
{
  this->handle = (uint64_t)OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->pid);
}

Napi::Value Process::GetBaseAddr(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  PROCESSENTRY32 pEntry;
  pEntry.dwSize = sizeof(pEntry);

  Process32First(hProcessSnapshot, &pEntry);
  CHAR processName[260];

  do
  {
    if (static_cast<int>(pEntry.th32ProcessID) == pid)
    {
      strcpy(processName, (CHAR *)pEntry.szExeFile);
      break;
    }
  } while (Process32Next(hProcessSnapshot, &pEntry));

  CloseHandle(hProcessSnapshot);

  HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
  MODULEENTRY32 mEntry;

  if (hModuleSnapshot == INVALID_HANDLE_VALUE)
  {
    Napi::TypeError::New(env, "module snapshot failed")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  mEntry.dwSize = sizeof(mEntry);

  if (!Module32First(hModuleSnapshot, &mEntry))
  {
    CloseHandle(hModuleSnapshot);
    Napi::TypeError::New(env, "failed to retrieve the first module")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  std::vector<MODULEENTRY32> modules;

  do
  {
    if (!strcmp((CHAR *)mEntry.szModule, processName))
    {
      return Napi::BigInt::New(env, (uint64_t)mEntry.modBaseAddr);
    }
  } while (Module32Next(hModuleSnapshot, &mEntry));

  CloseHandle(hModuleSnapshot);
  Napi::TypeError::New(env, "module not found")
      .ThrowAsJavaScriptException();
  return env.Undefined();
}

void Process::Read(uint64_t addr, void *target, size_t len) {
  size_t readLen;
  ReadProcessMemory((HANDLE)handle, (LPVOID)addr, target, len, &readLen);
  if (len != readLen) {
    throw std::runtime_error("Read length mismatch");
  }
}

void Process::Write(uint64_t addr, void *source, size_t len) {
  size_t writeLen;
  WriteProcessMemory((HANDLE)handle, (LPVOID)addr, source, len, &writeLen);
  if (len != writeLen) {
    throw std::runtime_error("Write length mismatch");
  }
}
