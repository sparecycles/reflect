#include <reflect_js/JavaScript_private.h>
#include <reflect/StructType.h>
#include <reflect/DataProperty.h>
#include <reflect/PrimitiveTypes.h>
#include <reflect/string/String.h>
#include <reflect/function/Function.h>

namespace reflect { namespace js {

JSClass *JavaStructClass();

static JSBool ReadStructProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	int32 prop_index;
	JS_ValueToInt32(cx, id, &prop_index);
	void *opaque = JS_GetPrivate(cx, obj);
	const StructType *type = GetNativeType(cx, obj) % autocast;
	
	Variant value;
	
	if(const DataProperty *prop = type->GetMember(int(prop_index)) % autocast)
	{
		if(prop->ReadData(opaque, value) && VariantToJava(cx, *vp, value))
		{
			return JS_TRUE;
		}
	}

	JS_ReportError(cx, "Failed to read property %d", int(prop_index));
	
	return JS_FALSE;
}

static JSBool WriteStructProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	int32 prop_index;
	JS_ValueToInt32(cx, id, &prop_index);
	void *opaque = JS_GetPrivate(cx, obj);
	const StructType *type = GetNativeType(cx, obj) % autocast;
	
	
	if(const DataProperty *prop = type->GetMember(int(prop_index)) % autocast)
	{
		Variant value;
		if(JavaToVariant(cx, value, *vp) && prop->WriteData(opaque, value))
		{
			return JS_TRUE;
		}
	}

	JS_ReportError(cx, "Failed to write property %d", int(prop_index));
	
	return JS_FALSE;
}

const Type *GetNativeType(JSContext *cx, JSObject *obj)
{
	jsval type_val;

	if(!JS_InstanceOf(cx, obj, JavaStructClass(), 0) &&
		!JS_InstanceOf(cx, obj, JavaDynamicClass(), 0) &&
		!JS_InstanceOf(cx, obj, FunctionClass(), 0))
	{
		return 0;
	}

	if(JS_FALSE == JS_GetReservedSlot(cx, obj, 0, &type_val))
	{
		JS_ReportError(cx, "StructClass: failed to get reserved slot");
		return 0;
	}
	
	return translucent_cast<const Type *>(
		JSVAL_TO_PRIVATE(type_val));
}

static JSBool EnumerateStruct(JSContext *cx, JSObject *obj)
{
	const StructType *type = GetNativeType(cx, obj) % autocast;
	if(0 == type)
	{
		if(!JS_IsExceptionPending(cx))
			JS_ReportError(cx, "StructType: Invalid native type");
		return JS_FALSE;
	}
	
	for(int property_index = 0; property_index < type->NumMembers(); property_index++)
	{
		JS_DefineProperty(cx, obj, (char *)0 + property_index, JSVAL_VOID, 
			ReadStructProperty, WriteStructProperty, 
			JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_INDEX);		
	}
	
    return JS_TRUE;
}


static JSBool ResolveStructProperty(JSContext *cx, JSObject *obj, jsval id)
{
	int32 property_index; 
	const StructType *type = GetNativeType(cx, obj) % autocast;

	if(!JS_ValueToInt32(cx, id, &property_index))
	{
		JS_ClearPendingException(cx);

		JSString *str = JS_ValueToString(cx, id);
		if(!str)
		{
			JS_ReportError(cx, "Failed to convert property id to string");
			return JS_FALSE;
		}

		const char *id_name = JS_GetStringBytes(str);
		if(!id_name)
		{
			return JS_FALSE;
		}

		if(const function::Function *func = type->FindFunction(id_name))
		{
			JSObject *f = DefineNativeFunction(cx, JS_GetPrototype(cx, obj), func);
			
			if(0 == f)
			{
				JS_ReportError(cx, "native: %s: Failed to define javascript function\n", id_name);
				return JS_FALSE;
			}
			
			return JS_TRUE;
		}

		// not an error, just not our property.
		return JS_TRUE;
	}

	if(0 == type)
	{
		if(!JS_IsExceptionPending(cx))
			JS_ReportError(cx, "StructType: Invalid native type");
		return JS_FALSE;
	}

	if(type->GetMember(property_index))
	{
		JS_DefineProperty(cx, obj, (char *)0 + property_index, JSVAL_VOID, 
			ReadStructProperty, WriteStructProperty, 
			JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_INDEX);
		
		return JS_TRUE;
	}

    return JS_TRUE;
}

static void FinalizeStruct(JSContext *cx, JSObject *obj)
{
	RuntimeData *data = GetRuntimeData(JS_GetRuntime(cx));	
	data->ReleaseJavaObject(obj);
}

static JSClass jsclass_StructClass =
{
	"Struct",
	JSCLASS_HAS_PRIVATE | JSCLASS_HAS_RESERVED_SLOTS(1),
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	EnumerateStruct,
    ResolveStructProperty,
    JS_ConvertStub,
    FinalizeStruct,

    JSCLASS_NO_OPTIONAL_MEMBERS
};

JSClass *JavaStructClass() { return &jsclass_StructClass; }

} }
