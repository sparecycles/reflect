#include <reflect/Reflection.h>
#include <reflect/execute/ApplicationClass.hpp>
#include <v8.h>
#include <readline/readline.h>

DEFINE_APPLICATION("v8r::Shell") {
  v8::HandleScope handle_scope;
  v8::Persistent<v8::Context> context = v8::Context::New();
  v8::Context::Scope scope(context);
  char *line;
  while(line = readline("> "), line) {
    v8::HandleScope execution_handle_scope;
    v8::TryCatch try_catch;
    v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(line), v8::String::New("repl"));
    v8::Handle<v8::Value> result = script->Run();
    if(try_catch.HasCaught()) {
      printf("\033[41mError\033[0m\n");
    } else {
      v8::String::Utf8Value str(result); 
      const char* cstr = *str; 
      printf("%s\n", cstr); 
    }
  }
  return 0;
}

namespace reflect { namespace v8r {


} } // ::v8 ::reflect
