#include "process.hpp"

Napi::Object Process::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
      DefineClass(env, "Process",
                  {InstanceMethod("getBaseAddr", &Process::GetBaseAddr),
                   InstanceMethod("readMemory", &Process::ReadMemory),
                   InstanceMethod("writeMemory", &Process::WriteMemory),
                   InstanceMethod("release", &Process::ReleaseProcess),
                   InstanceMethod("hasReleased", &Process::hasReleased)});

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
  pid = value.Int32Value();

  try {
    AquireProcess();
  } catch (std::exception& e) {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    return;
  }
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

  try {
    if (type == "BYTE") {
      uint8_t result;
      Read(addr, &result, sizeof(result));
      return Napi::Number::New(env, result);
    } else if (type == "INT") {
      int result;
      Read(addr, &result, sizeof(result));
      return Napi::Number::New(env, result);
    } else if (type == "INT32") {
      int32_t result;
      Read(addr, &result, sizeof(result));
      return Napi::Number::New(env, result);
    } else if (type == "UINT32" || type == "DWORD") {
      uint32_t result;
      Read(addr, &result, sizeof(result));
      return Napi::Number::New(env, result);
    } else if (type == "INT64") {
      int64_t result;
      Read(addr, &result, sizeof(result));
      return Napi::BigInt::New(env, result);
    } else if (type == "UINT64") {
      uint64_t result;
      Read(addr, &result, sizeof(result));
      return Napi::BigInt::New(env, result);
    } else if (type == "SHORT") {
      short result;
      Read(addr, &result, sizeof(result));
      return Napi::Number::New(env, result);
    } else if (type == "LONG") {
      long result;
      Read(addr, &result, sizeof(result));
      return Napi::BigInt::New(env, static_cast<int64_t>(result));
    } else if (type == "FLOAT") {
      float result;
      Read(addr, &result, sizeof(result));
      return Napi::Number::New(env, result);
    } else if (type == "DOUBLE") {
      double result;
      Read(addr, &result, sizeof(result));
      return Napi::Number::New(env, result);
    } else if (type == "POINTER") {
      uintptr_t result;
      Read(addr, &result, sizeof(result));
      return Napi::BigInt::New(env, static_cast<uint64_t>(result));
    } else if (type == "BOOL") {
      bool result;
      Read(addr, &result, sizeof(result));
      return Napi::Number::New(env, result);
    } else if (type == "STRING") {
      std::string result;
      while (1) {
        char l;
        Read(addr, &l, sizeof(l));
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
  } catch (std::exception& e) {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
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

  try {
    if (type == "INT") {
      int value = info[2].As<Napi::Number>().Int32Value();
      Write(addr, &value, sizeof(value));
    } else if (type == "STRING") {
      std::string value = info[2].As<Napi::String>().Utf8Value();
      Write(addr, (void*)value.c_str(), value.length() + 1);
    }
  } catch (std::exception& e) {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    return;
  }
}

Napi::Value Process::hasReleased(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::Boolean::New(env, pid == -1);
}
