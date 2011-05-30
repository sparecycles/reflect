#include <reflect/PersistentClass.hpp>
#include <reflect/Persistent.h>
#include <reflect/execute/ApplicationClass.hpp>
#include <v8.h>
#include <readline/readline.h>
#include "ObjectWrap.h"

namespace reflect { namespace v8r {

v8::Handle<v8::ObjectTemplate> RegisterType(Type *type) {
  v8::Persistent<v8::ObjectTemplate> type_template;
  if(PersistentClass *persistent_class = type % autocast) {
    printf("Registering %s\n", persistent_class->Name());
    type_template = v8::Persistent<v8::ObjectTemplate>::New(v8::ObjectTemplate::New());
    type_template->SetInternalFieldCount(1);
    for(PersistentClass::PropertyIterator it(persistent_class); it; it.next()) {
      struct Accessor {
        Accessor(const char *name_, const Property *property_) : name(name_), property(property_) {}
	const char *name;
	const Property *property;
        static v8::Handle<v8::Value> getter(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
	  const Accessor *accessor = static_cast<const Accessor *>(v8::External::Unwrap(info.Data()));
          if(const DataProperty *prop = accessor->property % autocast) {
            reflect::Variant variant;
            prop->ReadData(v8::External::Unwrap(info.This()->GetInternalField(0)), variant);
            if(variant.GetType()->Derives<double>() || variant.GetType()->Derives<float>()) {
	      return v8::Number::New(variant.AsValue<double>());
            } else if(variant.CanReadAs<long>()) {
              return v8::Number::New(variant.AsValue<long>());
            }
          } else {
            printf("Can't access non-data property yet: %s", accessor->name);
          }
	  return v8::Undefined();
        }
        static void setter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
	  const Accessor *accessor = static_cast<const Accessor *>(v8::External::Unwrap(info.Data()));
	  void *opaque = v8::External::Unwrap(info.This()->GetInternalField(0));
          if(const DataProperty *prop = accessor->property % autocast) {
	    if(value->IsInt32()) {
              prop->WriteData(opaque, reflect::Variant::FromConstRef(value->Int32Value()));
            } else if(value->IsNumber()) {
              prop->WriteData(opaque, reflect::Variant::FromConstRef(value->NumberValue()));
            }
          } else {
            printf("Can't access non-data property yet: %s", accessor->name);
          }
        }
	static v8::Handle<v8::Value> propertyInfo(PersistentClass::PropertyIterator it) {
	  Accessor *accessor = new Accessor(it->first.c_str(), it->second);
	  return v8::External::Wrap(static_cast<void *>(accessor));
        }
      };
      type_template->SetAccessor(
        v8::String::New(it->first.c_str()),
	&Accessor::getter,
	&Accessor::setter,
	Accessor::propertyInfo(it)
      );
    }
  }
  return type_template;
}
 
} } // ::v8 ::reflect

class Point : public reflect::Persistent {
  DECLARE_REFLECTION(reflect::Persistent)
public:
  Point(int x_ = 10, int y_ = 42) : x(x_), y(y_) {}
  int x, y;
};

DEFINE_REFLECTION(Point, "Point") {
   +Concrete;

   Properties
     ("x", &Point::x)
     ("y", &Point::y)
     ;
}

DEFINE_APPLICATION("v8r::Shell") {
  v8::HandleScope global_handle_scope;
  v8::Persistent<v8::Context> context = v8::Context::New();
  v8::Context::Scope scope(context);
  char *line;
  v8::Handle<v8::ObjectTemplate> point_class = reflect::v8r::RegisterType(TypeOf<Point>());
  v8::Local<v8::Object> point_inst = point_class->NewInstance();
  point_inst->SetInternalField(0, v8::External::New(new Point()));
  context->Global()->Set(v8::String::New("point"), point_inst);
  while(line = readline("> "), line) {
    v8::HandleScope execution_handle_scope;
    v8::TryCatch try_catch;
    v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(line), v8::String::New("repl"));
    v8::Handle<v8::Value> result = *script ? script->Run() : v8::Handle<v8::Value>();
    if(try_catch.HasCaught()) {
      printf("\033[41mError: %s\033[0m\n", *v8::String::Utf8Value(try_catch.Exception()));
      if(!try_catch.CanContinue()) exit(1);
    } else {
      printf("%s\n", *v8::String::Utf8Value(result)); 
    }
  }
  context.Dispose();
  return 0;
}
