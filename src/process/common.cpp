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
    return Napi::BigInt::New(env, static_cast<int64_t>(Read<long>(addr)));
  } else if (type == "FLOAT") {
    return Napi::Number::New(env, Read<float>(addr));
  } else if (type == "DOUBLE") {
    return Napi::Number::New(env, Read<double>(addr));
  } else if (type == "POINTER") {
    return Napi::BigInt::New(env, static_cast<uint64_t>(Read<uintptr_t>(addr)));
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
