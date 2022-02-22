#include "process.hpp"

Napi::Object Process::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
      DefineClass(env, "Process",
                  {
                      InstanceMethod("getBaseAddr", &Process::GetBaseAddr),
                      InstanceMethod("readMemory", &Process::ReadMemory),
                      InstanceMethod("writeMemory", &Process::WriteMemory),
                  });

  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);

  exports.Set("Process", func);
  return exports;
}

Process::Process(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Process>(info) {
  Napi::Env env = info.Env();

  int length = info.Length();

  if (length <= 0 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    return;
  }

  Napi::Number value = info[0].As<Napi::Number>();
  this->pid = value.Int32Value();
  this->handle = -1;
}
