#define DEBUG
#include <jsapi.h>
#include <iostream>
#include <string>
#include <js/Initialization.h>
#include <js/Object.h>
#include <js/Warnings.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>

#include "js/CharacterEncoding.h"
#include "js/Conversions.h"

inline JSClass global_class = { "global", JSCLASS_GLOBAL_FLAGS, &JS::DefaultGlobalClassOps };

class SpiderMonkeyEngine {
public:
  SpiderMonkeyEngine() {
    if (!JS_Init()) { throw std::runtime_error("Failed to initialize JS engine"); }

    cx = JS_NewContext(8192 * 1024); // 8MB stack
    if (!cx) { throw std::runtime_error("Failed to create JS context"); }

    JS::InitSelfHostedCode(cx);

    JS::RealmOptions options;
    auto newGlobal = JS_NewGlobalObject(cx, &global_class, nullptr, JS::FireOnNewGlobalHook, options);
    if (!newGlobal) { throw std::runtime_error("Failed to create global object"); }

    global.init(cx, newGlobal);

    JSAutoRealm ar(cx, global);

    const char* script = "function hello(name) { return 'Hello, ' + name; }";
    JS::CompileOptions opts(cx);
    JS::SourceText<mozilla::Utf8Unit> srcBuf;
    if (!srcBuf.init(cx, script, strlen(script), JS::SourceOwnership::Borrowed)) { throw std::runtime_error("Failed to initialize source buffer"); }

    JS::RootedValue val(cx);
    JS::MutableHandleValue rval(&val);

    if (!JS::Evaluate(cx, opts, srcBuf, rval)) { throw std::runtime_error("Failed to compile script"); }
  }

  ~SpiderMonkeyEngine() {
    JS_DestroyContext(cx);
    JS_ShutDown();
  }

  std::string callHello(const std::string& name) {
    JSAutoRealm ar(cx, global);

    JS::RootedString jsStr(cx, JS_NewStringCopyZ(cx, name.c_str()));
    if (!jsStr) { throw std::runtime_error("Failed to create JS string"); }

    JS::RootedValue func(cx);
    if (!JS_GetProperty(cx, global, "hello", &func) || !func.isObject() || !JS_ObjectIsFunction(&func.toObject())) {
      throw std::runtime_error("Failed to get hello function");
    }

    JS::RootedValueArray<1> args(cx);
    args[0].setString(jsStr);

    JS::RootedValue val(cx);
    JS::MutableHandleValue rval(&val);

    if (!JS_CallFunctionValue(cx, global, func, args, rval)) { throw std::runtime_error("Failed to call hello function"); }

    if (!rval.isString()) { throw std::runtime_error("Failed to get string back"); }
    //JS::RootedString resultStr(cx, rval.toString());
    JS::RootedString resultStr(cx, JS::ToString(cx, rval));
    /// UNCOMMENT THE NEXT LINE TO TRIGGER THE CRASH ON ROUTINE EXIT
    JS::UniqueChars resultCStr = JS_EncodeStringToUTF8(cx, resultStr);
    /// UNCOMMENT THE ABOVE LINE TO TRIGGER THE CRASH ON ROUTINE EXIT
    if (!resultCStr) {
      throw std::runtime_error("Failed to encode result string");
    }
    //
    std::string resultCppStr(resultCStr.get());
    return resultCppStr;
  }

protected:
  JSContext* cx;
  JS::PersistentRootedObject global;
};

int main() {
  try {
    SpiderMonkeyEngine engine;
    std::string result = engine.callHello("World");
    std::cout << result << std::endl;
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
