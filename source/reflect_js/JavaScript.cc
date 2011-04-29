#include <reflect_js/JavaScript.h>
#include <reflect/PrimitiveTypes.h>
#include <reflect/string/String.h>
#include <reflect/Persistent.h>
#include <reflect/DataProperty.h>
#include <reflect/PropertyPath.h>
#include <reflect/function/Function.hpp>
#include <reflect/utility/Context.hpp>
#include <reflect_js/JavaScript_private.h>
#include <jsapi.h>
#include <jsdbgapi.h>
#include <map>


template class reflect::utility::Context<reflect::js::CallContextObject>;

namespace reflect { namespace js {

RuntimeData::NativeInfo::NativeInfo(const Type *t, void *ptr)
	: type(t)
	, opaque(ptr)
{}
		
RuntimeData::JavaInfo::JavaInfo(JSObject *obj)
	: object(obj)
	, native_reference_count(1)
{}

void RuntimeData::DecRef(void *opaque)
{
	JavaInfo &info = mOpaqueToObject[opaque];
	--info.native_reference_count;
	
	if(0 == info.native_reference_count && info.object)
	{
		JavaToNativeMap::iterator it = mObjectToOpaque.find(info.object);
	
		if(mObjectToOpaque.end() == it)
		{
			mOpaqueToObject.erase(opaque);
			
			delete [] static_cast<char *>(it->second.type->Destruct(opaque));

			mObjectToOpaque.erase(it);
		}
	}
}

void RuntimeData::IncRef(void *opaque)
{
	mOpaqueToObject[opaque].native_reference_count++;
}

void RuntimeData::ReleaseJavaObject(JSObject *object)
{
	JavaToNativeMap::iterator it = mObjectToOpaque.find(object);

	if(it != mObjectToOpaque.end())
	{
		NativeToJavaMap::iterator nit = mOpaqueToObject.find(it->second.opaque);
		if(0 == nit->second.native_reference_count)
		{
			delete [] static_cast<char *>(it->second.type->Destruct(nit->first));
			mOpaqueToObject.erase(nit);
		}
		else
		{
			nit->second.object = 0;
		}

		mObjectToOpaque.erase(it);
	}
}

JSObject *RuntimeData::GetJavaObject(JSContext *cx, void *opaque, const Type *type, JSClass *repclass)
{
	JavaInfo &info = mOpaqueToObject[opaque];

	if(info.object == 0)
	{
		JSObject *proto = FindPrototype(type);
		
		info.object = JS_NewObject(cx, repclass, proto, 0);
		
		if(!info.object)
		{
			JS_ReportError(cx, "Error: Out of memory");
			return 0;
		}
		JS_SetPrivate(cx, info.object, opaque);
		JS_SetReservedSlot(cx, info.object, 0, PRIVATE_TO_JSVAL(type));
		NativeInfo &native = mObjectToOpaque[info.object];
	
		if(native.type == 0)
		{
			native.type = type;
			native.opaque = opaque;
		}
	}
	
	return info.object;
}

static const uint32 gc_heap_size = 20 << 10;
static const uint32 stack_chunk_size = 2048;

JavaScriptRuntime JavaScriptRuntime::sSharedRuntime(0);

JavaScriptRuntime JavaScriptRuntime::SharedRuntime()
{
	return sSharedRuntime;
}

static int sJSRuntimeCount = 0;

RuntimeData *GetRuntimeData(JSRuntime *rt)
{
	RuntimeData *data = static_cast<RuntimeData *>(JS_GetRuntimePrivate(rt));
	
	return data;
}

static JSRuntime *AllocateRuntime()
{
	JSRuntime *rt = JS_NewRuntime(gc_heap_size);
	
	JS_SetRuntimePrivate(rt, new RuntimeData(rt));
	
	sJSRuntimeCount++;
	
	return rt;
}

static void ReleaseRuntime(JSRuntime *rt)
{
	delete GetRuntimeData(rt);

	JSContext *cx = JS_NewContext(rt, 4096);
	JS_DestroyContext(cx);
		
	JS_DestroyRuntime(rt);
	
	if(0 == --sJSRuntimeCount)
	{
		JS_ShutDown();
	}
}

JavaScriptRuntime::JavaScriptRuntime()
	: mRuntime()
{
}

JavaScriptRuntime::JavaScriptRuntime(JSRuntime *runtime)
	: mRuntime(runtime)
{
}

JavaScriptRuntime::JavaScriptRuntime(const JavaScriptRuntime &other)
{
	if(0 == other.mRuntime)
	{
		if(&other != &sSharedRuntime)
		{
			// error!!!
		}
		
		sSharedRuntime.mRuntime = AllocateRuntime();
	}
	
	mRuntime = other.mRuntime;
	
	SharedLink(other);
}

JavaScriptRuntime::~JavaScriptRuntime()
{
	if(this != &sSharedRuntime && SharedRelease())
	{
		ReleaseRuntime(mRuntime);
		mRuntime = 0;
	}
	else if(sSharedRuntime.mRuntime && sSharedRuntime.NextShared() == &sSharedRuntime)
	{
		ReleaseRuntime(mRuntime);
		sSharedRuntime.mRuntime = 0;
	}
}

void JavaScriptContext::IncOpaqueRef(void *opaque) const
{
	RuntimeData *data = GetRuntimeData(JS_GetRuntime(mContext));
	data->IncRef(opaque);
}

void JavaScriptContext::DecOpaqueRef(void *opaque) const
{
	RuntimeData *data = GetRuntimeData(JS_GetRuntime(mContext));
	data->DecRef(opaque);
}

static JSBool GlobalClass_Resolve(JSContext *cx, JSObject *obj, jsval id)
{
	return JS_TRUE;
}

static JSClass jsclass_GlobalClass = {
	"Global",
	JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, // add
	JS_PropertyStub, // del
	JS_PropertyStub, // get
	JS_PropertyStub, // set
	JS_EnumerateStub,
	GlobalClass_Resolve,
	JS_ConvertStub,
	JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS	
};

bool JavaScriptContext::RegisterFunction(string::ConstString name, const function::Function *native, bool take_ownership)
{
	if(take_ownership)
		mRegisteredFunctions.push_back(native);	
	
	if(native->IsMethod())
		return false;

	JSObject *func = DefineNativeFunction(mContext, JS_GetGlobalObject(mContext), native);

	return func != 0;
}

void JavaScriptContext::ThrowError(const char *format, ...) const
{
	reflect::string::String str;
	
	va_list args;
	va_start(args, format);
	str.vformat(format, args);
	va_end(args);
	
	JS_ReportError(mContext, "%s", str.c_str());
}

JavaScriptContext::JavaScriptContext(const JavaScriptRuntime &runtime)
	: mRuntime(runtime)
	, mContext(JS_NewContext(runtime.GetRuntime(), stack_chunk_size))
{
	JS_SetContextPrivate(mContext, translucent_cast<void *>(this));
	JS_InitStandardClasses(mContext, JS_NewObject(mContext, &jsclass_GlobalClass, 0, 0));

	if(JSObject *native_function_prototype = JS_DefineObject(mContext, JS_GetGlobalObject(mContext), "NativeFunctionPrototype", 0, 0, JSPROP_PERMANENT))
	{
		for(ObjectType::FunctionIterator it(TypeOf<function::Function>()); it; it.next())
			DefineNativeFunction(mContext, native_function_prototype, it->second);

#define LINE "\n"

		Eval("NativeFunctionPrototype.toString = function() {         "
		LINE "   function typename(type) {                            "
		LINE "     var name = type.Name();                            "
		LINE "     if(name.length) return name;                       "
		LINE "     else return '<unnamed>';                           "
		LINE "   }                                                    "
		LINE "                                                        "
		LINE "   var text = 'function() {\\n  ';                      "
		LINE "   text += typename(this.ResultType()) + ' ';           "
		LINE "                                                        "
		LINE "   if(this.IsMethod()) {                                "
		LINE "     text += typename(this.CallObjectType()) + '::';    "
		LINE "   }                                                    "
		LINE "                                                        "
		LINE "   text += this.Name() + '(';                           "
		LINE "   var num_params = this.NumParameters();               "
		LINE "                                                        "
		LINE "   for(var index = 0; index < num_params; index++) {    "
		LINE "     if(index > 0) text += ', ';                        "
		LINE "     text += typename(this.ParameterType(index));       "
		LINE "   }                                                    "
		LINE "                                                        "
		LINE "   text += ')\\n}';                                     "
		LINE "   return text;                                         "
		LINE "}                                                       "
		);
#undef LINE

	}

	GetRuntimeData(JS_GetRuntime(mContext))->MakeNativeObject(mContext);
	
	struct Intrinsic {
		static void print()
		{
			int argc = JavaScriptContext::CallArgumentCount();
			for(int i = 0; i < argc; i++)
			{
				printf("%s%s",
					JavaScriptContext::CallArgumentText(i),
					i < argc-1 ? " " : "\n");
			}
		}

		static void gc()
		{
			JavaScriptContext::CallContext()->GC();
		}

		static void console()
		{
			const JavaScriptContext *context = JavaScriptContext::CallContext();
			char buffer[4096];

			for(;;)
			{
				printf("> ");
				fflush(stdout);

				if(!fgets(buffer, sizeof(buffer)-1, stdin))
					break;

				buffer[sizeof(buffer)-1] = 0;
				
				int end = strlen(buffer);

				while(--end >= 0 && strchr("\n\r \t", buffer[end]))
					buffer[end] = '\0';
				
				char *text = buffer;
				while(*text && strchr(" \t", *text))
					text++;
				
				if(strcmp("exit", text) == 0)
					return;
				
				Variant result;
				if(context->Eval(text, result))
				{
					if(result.GetType() && result.GetType() != TypeOf<void>())
						printf("%s\n", result.ToString().c_str());
				}
			}

			fprintf(stdout, "\n");	
		}
		
		static Type *FindType(const char *name) { return Type::FindType(name); }
	};

	RegisterFunction("print", Intrinsic::print);
	RegisterFunction("GC", Intrinsic::gc);
	RegisterFunction("Console", Intrinsic::console);
}

JSContext *JavaScriptContext::GetJSContext() const
{
	return mContext;
}

JavaScriptContext::~JavaScriptContext()
{
	for(std::vector<const function::Function *>::iterator it = mRegisteredFunctions.begin();
		it != mRegisteredFunctions.end();
		it++)
	{
		delete *it;
	}
		
	JS_DestroyContext(mContext);
}

void JavaScriptContext::GC() const
{
	JS_GC(mContext);
}

bool JavaScriptContext::Eval(string::Fragment script) const
{
	jsval rval;

	//fprintf(stderr, "[[ %.*s ]]\n", script.size(), script.data());
	
	JSBool result = JS_EvaluateScript(mContext,
		JS_GetGlobalObject(mContext),
		script.data(),
		script.size(),
		"script", 1, &rval);
	
	if(JS_FALSE == result)
	{
		if(JS_IsExceptionPending(mContext))
		{
			jsval exception;
			JS_GetPendingException(mContext, &exception);
			JSString *text = JS_ValueToString(mContext, exception);
			fprintf(stderr, "\nException: %s\n", JS_GetStringBytes(text));
			JS_ClearPendingException(mContext);
		}
		return false;
	}
	
	return true;
}

bool JavaScriptContext::Eval(string::Fragment script, Variant &result) const
{
	jsval rval;
	
	if(JS_IsExceptionPending(mContext))
		JS_ClearPendingException(mContext);
	
	JSBool jsresult = JS_EvaluateScript(mContext, JS_GetGlobalObject(GetJSContext()), script.data(), script.size(), "script", 1, &rval);

	if(jsresult) switch(JS_TypeOfValue(mContext, rval))
	{
	case JSTYPE_VOID:
    case JSTYPE_OBJECT:
		{
			JSObject *object;
			if(JSVAL_IS_OBJECT(rval)
				&& (object = JSVAL_TO_OBJECT(rval), object)
				&& (JS_GET_CLASS(GetJSContext(), object) == JavaStructClass()
				|| JS_GET_CLASS(GetJSContext(), object) == JavaDynamicClass()))
			{
				void *opaque = JS_GetPrivate(GetJSContext(), object);
				const Type *type = GetNativeType(GetJSContext(), object);
				return result.Ref(Variant::FromOpaque(type, opaque));
			}
		}
		break;
    case JSTYPE_FUNCTION:
    case JSTYPE_NULL:
    case JSTYPE_XML:
		break;
    case JSTYPE_STRING:
		{
			JSString *string = JS_ValueToString(mContext, rval);
			char *string_data = JS_GetStringBytes(string);
			return result.SetValue(string_data);
		}
		break;
    case JSTYPE_NUMBER:
		if(JSVAL_IS_INT(rval))
		{
			return result.SetValue(JSVAL_TO_INT(rval));
		}
		else
		{
			return result.SetValue(*JSVAL_TO_DOUBLE(rval));
		}
		break;
    case JSTYPE_BOOLEAN:
		{
			return result.SetValue(bool(JSVAL_TO_BOOLEAN(rval) != JS_FALSE));
		}
		break;
	default:
		break;
	}
	
	if(JS_FALSE == jsresult)
	{
		jsval exval;
		
		if(!JS_GetPendingException(mContext, &exval))
		{
			fprintf(stderr, "Error: Unknown Exception\n");
			return false;
		}
		
		JSString *str = JS_ValueToString(mContext, exval);
		fprintf(stderr, "%s\n", JS_GetStringBytes(str));
		JS_ClearPendingException(mContext);
		return false;
	}
	
	return true;
}



const JavaScriptContext *JavaScriptContext::CallContext()
{
	if(CallContextObject *cco = CallContextObject::GetContext())
	{
		return cco->CallContext();
	}
	
	return 0;
}

int JavaScriptContext::CallArgumentCount()
{
	if(CallContextObject *cco = CallContextObject::GetContext())
	{
		return cco->ArgumentCount();
	}
	
	return 0;
}

bool JavaScriptContext::CallArgument(int index, Variant &variant)
{
	if(CallContextObject *cco = CallContextObject::GetContext())
	{
		return cco->GetArgument(index, variant);
	}
	
	return false;
}

const char *JavaScriptContext::CallArgumentText(int index)
{
	if(CallContextObject *cco = CallContextObject::GetContext())
	{
		return cco->GetArgumentText(index);
	}
	
	return false;	
}

bool JavaScriptContext::CallReturn(Variant &value)
{
	if(CallContextObject *cco = CallContextObject::GetContext())
	{
		return cco->SetReturnValue(value);
	}
	
	return false;	
}


void RuntimeData::SetPrototype(const Type *type, JSObject *proto)
{
	mTypePrototypes[type] = proto;
}

void RuntimeData::MakeNativeObject(JSContext *cx)
{
	if(mNative)
	{
		JS_DefineProperty(cx, JS_GetGlobalObject(cx), "Native", OBJECT_TO_JSVAL(mNative), 0, 0, JSPROP_PERMANENT);
	}
	else
	{
		mNative = JS_DefineObject(cx, JS_GetGlobalObject(cx), "Native", 0, 0, JSPROP_PERMANENT);
		EnumerateRootNamespaces(cx, mNative);
	}
}

RuntimeData::~RuntimeData()
{
}

JSObject *RuntimeData::FindPrototype(const Type *type) const
{
	NativePrototypeMap::const_iterator it = mTypePrototypes.find(type);

	if(it != mTypePrototypes.end())
		return it->second;

	return 0;
}

} }

#include <reflect/execute/ApplicationClass.hpp>

DEFINE_APPLICATION("reflect_js::RunFile")
{
	using namespace reflect;
	using namespace js;

	JavaScriptContext context;
	
	string::String result;
	
	string::String script = "";

	if(argc > 1)
	{
		if(FILE *input = fopen(argv[1], "r"))
		{
			char buffer[256];

			while(fgets(buffer, sizeof(buffer)-1, input))
				buffer[sizeof(buffer)-1] = '\0', script += buffer;

			fclose(input);
		}
		else perror(argv[1]);
	}
	
	context.Eval(script.c_str(), result);
			
	return 0;
}
