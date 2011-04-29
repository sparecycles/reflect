#include <reflect_js/JavaScript_private.h>
#include <reflect/function/Function.h>
#include <reflect/autocast.h>
#include <reflect/string/String.h>

namespace reflect { namespace js {

static JSBool FunctionClassCall(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool FunctionClassConvert(JSContext *cx, JSObject *obj, JSType type, jsval *vp);
static JSBool FunctionClassResolve(JSContext *cx, JSObject *obj, jsval id);

static JSClass sFunctionClass = {
	"NativeFunction",
	JSCLASS_HAS_PRIVATE | JSCLASS_HAS_RESERVED_SLOTS(1),

	JS_PropertyStub, // add
	JS_PropertyStub, // del
	JS_PropertyStub, // get
	JS_PropertyStub, // set
	JS_EnumerateStub,
	FunctionClassResolve,
	JS_ConvertStub, // todo: override
	JS_FinalizeStub,

    /* Optionally non-null members start here. */
    0, //JSGetObjectOps      getObjectOps;
    0, //JSCheckAccessOp     checkAccess;
    FunctionClassCall, //JSNative            call;
    0, //JSNative            construct;
    0, //JSXDRObjectOp       xdrObject;
    0, //JSHasInstanceOp     hasInstance;
    0, //JSMarkOp            mark;
    0, //JSReserveSlotsOp    reserveSlots;
};

JSClass *FunctionClass()
{
	return &sFunctionClass;
}

JSObject *DefineNativeFunction(JSContext *cx, JSObject *obj, const function::Function *native)
{
	JSObject *native_function_prototype;
	jsval nfp_val;
	JS_GetProperty(cx, JS_GetGlobalObject(cx), "NativeFunctionPrototype", &nfp_val);
	JS_ValueToObject(cx, nfp_val, &native_function_prototype);
	
	if(JSObject *function_object = JS_DefineObject(cx, obj, native->Name(), FunctionClass(), native_function_prototype, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE))
	{
		JS_SetPrivate(cx, function_object, const_cast<void *>(static_cast<const void *>(native)));
		JS_SetReservedSlot(cx, function_object, 0, PRIVATE_TO_JSVAL(TypeOf<function::Function>()));
		return function_object;
	}
	
	return 0;
}

JSBool InvokeNative(JSContext *cx, uintN argc, jsval *argv, jsval *rval,
	const function::Function *native_function, const Variant &native_object)
{
	CallContextObject cco(static_cast<const JavaScriptContext *>(JS_GetContextPrivate(cx)), argc, argv, rval);

	function::Parameters params(native_function);
	
	if(int(argc) < native_function->NumParameters())
	{
		JS_ReportError(cx, "native: %s: too few parameters %d: expected %d",
			native_function->Name(), argc, native_function->NumParameters());

		return JS_FALSE;
	}
	
	for(int i = 0; i < native_function->NumParameters(); i++)
	{
		if(!JavaToVariant(cx, params[i], argv[i]))
		{
			JS_ReportError(cx, "native: %s: can't convert arg %d to a %s",
				native_function->Name(), i, params[i].GetType()->Name());

			return JS_FALSE;
		}
	}
	
	Variant result;
		
	bool call_result = native_function->Call(native_object, params, result);
	
	if(JS_IsExceptionPending(cx))
		return JS_FALSE;
		
	if(!call_result)
	{
		if(native_function->IsMethod())
		{
			JS_ReportError(cx, "native: %s::%s: call failed",
				native_function->CallObjectType()->Name(), native_function->Name());
		}
		else
		{
			JS_ReportError(cx, "native: %s: call failed",
				native_function->Name());
		}
		
		return JS_FALSE;
	}
	
	if(!cco.IsReturnValueSet() && false == VariantToJava(cx, *rval, result))
	{
		JS_ReportError(cx, "native: %s: Failed to convert return value of type %s",
			native_function->Name(), result.GetType()->Name()); 
		return JS_FALSE;
	}
	
	return call_result;
}

/*
JSBool CallNativeFunction(JSContext *cx, JSObject *, uintN argc, jsval *argv, jsval *rval)
{
// should match spidermonkey 1.8
#define JS_ARGV_CALLEE(argv__) ((argv__)[-2])

	JSFunction *java_function = JS_ValueToFunction(cx, JS_ARGV_CALLEE(argv));
	
	if(!java_function)
		return JS_FALSE;

	JSObject *function_object = JS_GetFunctionObject(java_function);	
	
	jsval native_function_value;

	if(JS_FALSE == JS_GetReservedSlot(cx, function_object, 0, &native_function_value))
	{
		JS_ReportError(cx, "Failed to get reserved slot of %s", JS_GetFunctionName(java_function));
		
		return JS_FALSE;
	}
	
	const function::Function *native_function = static_cast<const function::Function *>(
		JSVAL_TO_PRIVATE(native_function_value));

	if(!native_function)
		return JS_FALSE;

	return InvokeNative(cx, argc, argv, rval, native_function, Variant::Void());
}

JSBool CallNativeMethod(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
// should match spidermonkey 1.8
#define JS_ARGV_CALLEE(argv__) ((argv__)[-2])
	JSFunction *java_function = JS_ValueToFunction(cx, JS_ARGV_CALLEE(argv));
	
	if(!java_function)
		return JS_FALSE;

	JSObject *function_object = JS_GetFunctionObject(java_function);	
	
	jsval native_function_value;

	if(JS_FALSE == JS_GetReservedSlot(cx, function_object, 0, &native_function_value))
	{
		JS_ReportError(cx, "Failed to get reserved slot of %s", JS_GetFunctionName(java_function));
		
		return JS_FALSE;
	}
	
	const function::Function *native_function = static_cast<const function::Function *>(
		JSVAL_TO_PRIVATE(native_function_value));

	if(!native_function)
		return JS_FALSE;

	//////////
	void *native_object = JS_GetPrivate(cx, obj);
	
	if(!native_object)
		return JS_FALSE;
	
	const ObjectType *object_type = GetNativeType(cx, obj) % autocast;
	///////////
	
	return InvokeNative(cx, argc, argv, rval, native_function, Variant::FromOpaque(object_type, native_object));
}

JSBool CallFunctionMethod(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
// should match spidermonkey 1.8
#define JS_ARGV_CALLEE(argv__) ((argv__)[-2])
	JSFunction *java_function = JS_ValueToFunction(cx, JS_ARGV_CALLEE(argv));
	
	if(!java_function)
		return JS_FALSE;

	JSObject *function_object = JS_GetFunctionObject(java_function);	
	
	jsval native_function_value;

	if(JS_FALSE == JS_GetReservedSlot(cx, function_object, 0, &native_function_value))
	{
		JS_ReportError(cx, "Failed to get reserved slot of %s", JS_GetFunctionName(java_function));
		
		return JS_FALSE;
	}
	
	const function::Function *native_function = static_cast<const function::Function *>(
		JSVAL_TO_PRIVATE(native_function_value));
	
	if(!native_function)
		return JS_FALSE;


	//////////
	jsval native_self_value;

	if(JS_FALSE == JS_GetReservedSlot(cx, obj, 0, &native_self_value))
	{
		JS_ReportError(cx, "Failed to get reserved slot of self for %s", JS_GetFunctionName(java_function));
		
		return JS_FALSE;
	}
	
	const void *native_object = JSVAL_TO_PRIVATE(native_self_value);
	
	if(!native_object)
		return JS_FALSE;
	
	const ObjectType *object_type = TypeOf<function::Function>();
	///////////
	
	return InvokeNative(cx, argc, argv, rval, native_function, Variant::FromConstOpaque(object_type, native_object));
}
*/

static JSBool FunctionClassCall(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *function_object = JSVAL_TO_OBJECT(argv[-2]);
	const function::Function *native_function =
		static_cast<const function::Function *>(JS_GetPrivate(cx, function_object));
	
	if(native_function->IsMethod())
	{
		reflect::Variant native_self;
		native_self.BindType(native_function->CallObjectType());
		if(JavaToVariant(cx, native_self, OBJECT_TO_JSVAL(obj)))
		{
			return InvokeNative(cx, argc, argv, rval, native_function, native_self);
		}
		else
		{
			JS_ReportError(cx, "native: FunctionCall: Failed to coerce method's 'this' to correct type");
			return JS_FALSE;
		}
	}
	else
	{
		return InvokeNative(cx, argc, argv, rval, native_function, Variant::Void());
	}
}

static JSBool FunctionClassResolve(JSContext *cx, JSObject *obj, jsval id)
{
	return JS_TRUE;
}

static JSBool FunctionClassConvert(JSContext *cx, JSObject *obj, JSType type, jsval *vp)
{
	const function::Function *native_function =
		static_cast<const function::Function *>(JS_GetPrivate(cx, obj));

	switch(type)
	{
	case JSTYPE_STRING:
	case JSTYPE_VOID:
		{
			string::String function_signature;
			function_signature.format(
				"function() {\n"
				" [Native function...]\n"
				"}");
			*vp = STRING_TO_JSVAL(JS_NewStringCopyN(cx, function_signature.data(), function_signature.size()));
		}
		return JS_TRUE;
	default:
		return JS_ConvertStub(cx, obj, type, vp);
	}
}

} }
