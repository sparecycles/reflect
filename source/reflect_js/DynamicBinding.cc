#include <reflect_js/JavaScript_private.h>
#include <reflect/Persistent.h>
#include <reflect/PropertyPath.h>
#include <reflect/DataProperty.h>
//#include <reflect/PrimitiveTypes.h>
#include <reflect/string/String.h>
#include <reflect/function/Function.h>

namespace reflect { namespace js {

static JSBool ReadDynamicProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	using reflect::Persistent;
	using reflect::TypeOf;
	Dynamic *dynamic = translucent_cast<Dynamic *>(JS_GetPrivate(cx, obj));
	JSString *id_str = JS_ValueToString(cx, id);
	if(!id_str) return JS_FALSE;	
	
	const char *id_string = JS_GetStringBytes(id_str);

	if(Persistent *persistent = dynamic % autocast)
	{
		reflect::PropertyPath path = persistent->Property(id_string);

		switch(path.GetType())
		{
		case reflect::PropertyPath::Data:
			if(TypeOf<int>() == path.GetDataType())
			{
				int value;
				path.ReadValue(value);
				*vp = INT_TO_JSVAL(value);
			}
			else if(TypeOf<string::String>() == path.GetDataType())
			{
				string::String value;
				path.ReadValue(value);
				*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, value.c_str()));
			}
			else
			{
				return JS_FALSE;
			}
			return JS_TRUE;
		default:
			break;
		}
	}
	
	if(const function::Function *fun = dynamic->GetClass()->FindFunction(id_string))
	{
		(void)fun;
		return JS_TRUE;
	}
	
	if(string::ConstString("__iterator__") != id_string)
	{
		JS_ReportError(cx, "native: No Property Named %s\n", id_string);
		return JS_FALSE;
	}
			
	return JS_TRUE;
}

static JSBool WriteDynamicProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	using reflect::Persistent;
	using reflect::TypeOf;
	Persistent *persistent = translucent_cast<Persistent *>(JS_GetPrivate(cx, obj));
	
	JSString *id_str = JS_ValueToString(cx, id);
	if(!id_str) return JS_FALSE;
	
	const char *id_string = JS_GetStringBytes(id_str);
	
	reflect::PropertyPath path = persistent->Property(id_string);

	Variant native;
	JavaToVariant(cx, native, *vp);
	
	if(path.GetType() == reflect::PropertyPath::Data) 
	{
		return path.WriteData(native) ? JS_TRUE : JS_FALSE;
	}
	
	return JS_FALSE;
}

static JSBool EnumerateDynamicProperties(JSContext *cx, JSObject *obj)
{
	if(Persistent *persistent = translucent_cast<Dynamic *>(JS_GetPrivate(cx, obj)) % autocast)
	{
		for(PersistentClass::PropertyIterator it = persistent->GetClass(); it; it.next())
		{
			JS_DefineProperty(cx, obj, it->first.c_str(), JSVAL_NULL, ReadDynamicProperty, WriteDynamicProperty, 
				JSPROP_ENUMERATE | JSPROP_PERMANENT);
		}
	}
	
	return JS_TRUE;
}


static JSBool ResolveDynamicProperty(JSContext *cx, JSObject *obj, jsval id)
{
	JSString *id_str = JS_ValueToString(cx, id);
	char *id_name = JS_GetStringBytes(id_str);
	using reflect::Persistent;
	using reflect::Signature;
	Dynamic *dynamic = translucent_cast<Dynamic *>(JS_GetPrivate(cx, obj));

	if(Persistent *persistent = dynamic % autocast)
	{
		if(persistent->Property(id_name).GetType() != reflect::PropertyPath::Invalid)
		{
			JS_DefineProperty(cx, obj, id_name, JSVAL_NULL, ReadDynamicProperty, WriteDynamicProperty, 
				JSPROP_ENUMERATE | JSPROP_PERMANENT);
			return JS_TRUE;
		}
	}
	
    return JS_TRUE;
}

static void FinalizeDynamic(JSContext *cx, JSObject *obj)
{
	RuntimeData *data = GetRuntimeData(JS_GetRuntime(cx));	
	data->ReleaseJavaObject(obj);
}

static JSClass jsclass_DynamicClass =
{
	"Dynamic",
	JSCLASS_HAS_PRIVATE | JSCLASS_HAS_RESERVED_SLOTS(1),
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub, // ReadDynamicProperty
	JS_PropertyStub, // WriteDynamicProperty
	EnumerateDynamicProperties,
    ResolveDynamicProperty,
    JS_ConvertStub,
    FinalizeDynamic,

    JSCLASS_NO_OPTIONAL_MEMBERS
};

JSClass *JavaDynamicClass() { return &jsclass_DynamicClass; }

} }
