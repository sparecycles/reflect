#ifndef REFLECT_JS_JAVASCRIPTPRIVATE_H_
#define REFLECT_JS_JAVASCRIPTPRIVATE_H_

#include <reflect_js/JavaScript.h>
#include <reflect/utility/Context.h>
#include <jsapi.h>

namespace reflect { namespace function { class Function; } }

namespace reflect { namespace js {

bool JavaToVariant(JSContext *cx, Variant &variant, jsval val);
bool VariantToJava(JSContext *cx, jsval &val, const Variant &variant);

JSClass *JavaDynamicClass();
JSClass *JavaStructClass();
JSClass *JavaTypeClass();
JSClass *FunctionClass();

JSObject *DefineNativeFunction(JSContext *cx, JSObject *obj, const function::Function *native);

JSBool InvokeNative(JSContext *cx, uintN argc, jsval *argv, jsval *rval,
	const function::Function *native_function, const Variant &native_object);
//JSBool CallNativeFunction(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
//JSBool CallNativeMethod(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool EnumerateRootNamespaces(JSContext *cx, JSObject *obj);

const Type *GetNativeType(JSContext *cx, JSObject *obj);

class RuntimeData
{
public:
	RuntimeData(JSRuntime *rt)
		: mRuntime(rt)
		, mNative(0)
	{}
	
	JSObject *GetJavaObject(JSContext *cx, void *opaque, const Type *, JSClass *repclass);
	void ReleaseJavaObject(JSObject *object);
	void DecRef(void *opaque);
	void IncRef(void *opaque);
	
	void SetPrototype(const Type *type, JSObject *proto);
	
	JSObject *FindPrototype(const Type *type) const;
	
	void MakeNativeObject(JSContext *cx);

	~RuntimeData();	

private:
	struct NativeInfo
	{
		NativeInfo(const Type * = 0, void *ptr = 0);
		const Type *type;
		void *opaque;
	};

	struct JavaInfo
	{
		JSObject *object;
		int native_reference_count;
		JavaInfo(JSObject *obj = 0);
	};

	typedef std::map<const Type *, JSObject *> NativePrototypeMap;
	typedef std::map<void *, JavaInfo> NativeToJavaMap;
	typedef std::map<JSObject *, NativeInfo> JavaToNativeMap;

	JSRuntime *mRuntime;
	JSObject *mNative;

	NativeToJavaMap mOpaqueToObject;
	JavaToNativeMap mObjectToOpaque;
	NativePrototypeMap mTypePrototypes;
};

RuntimeData *GetRuntimeData(JSRuntime *rt);

class CallContextObject : public utility::Context<CallContextObject>
{
public:
	CallContextObject(const JavaScriptContext *ctx, int argc, jsval *argv, jsval *rval)
		: mCtx(ctx)
		, mArgc(argc)
		, mArgv(argv)
		, mReturnValue(rval)
		, mReturnValueSet(false)
	{}
	
	const JavaScriptContext *CallContext() const 
	{
		return mCtx;
	}
	
	int ArgumentCount() const
	{
		return mArgc;
	}
		
	bool GetArgument(int index, Variant &variant) const
	{
		if(index < 0 || index > mArgc) return false;
		return JavaToVariant(mCtx->GetJSContext(), variant, mArgv[index]);
	}
	
	const char *GetArgumentText(int index) const
	{
		if(index < 0 || index > mArgc)
			return "";
			
		if(JSString *str = JS_ValueToString(mCtx->GetJSContext(), mArgv[index]))
			return JS_GetStringBytes(str);
		else
			return "";
	}
	
	bool IsReturnValueSet() const
	{
		return mReturnValueSet;
	}
	
	bool SetReturnValue(Variant &variant) const
	{
		bool success = VariantToJava(mCtx->GetJSContext(), *mReturnValue, variant);

		if(success)
			mReturnValueSet = true;

		return success;
	}
	
private:
	const JavaScriptContext *mCtx;
	int mArgc;
	jsval *mArgv;
	jsval *mReturnValue;
	mutable bool mReturnValueSet;
};


} }

#endif
