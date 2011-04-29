#include <reflect_js/JavaScript_private.h>
#include <reflect/Persistent.h>
#include <reflect/DataProperty.h>
#include <reflect/ArrayProperty.h>
#include <reflect/PrimitiveTypes.h>
#include <reflect/StructType.h>
#include <reflect/string/String.h>

namespace reflect { namespace js {

// root_type is necessary because XML data comes in funny
// (as function objects with valueOf conversions to strings).
// TODO: Complain to the spidermonkey team that there's no simple api
// for valueOf().
static bool JavaToProperty(JSContext *cx, void *opaque, const Property *prop, jsval value, JSType root_type)
{
	if(!JSVAL_IS_VOID(value))
	{
		if(const DataProperty *dprop = prop % autocast)
		{
			Variant native_value;

			if(JSTYPE_XML == root_type)
			{
				JSString *strval = JS_ValueToString(cx, value);
				native_value.BindType(dprop->DataType());
				native_value.FromString(JS_GetStringBytes(strval));
			}
			else
			{
				native_value.BindType(dprop->DataType());
				if(false == JavaToVariant(cx, native_value, value))
				{
					return false;
				}
			}
		
			if(false == dprop->WriteData(opaque, native_value))
			{
				return false;
			}
		}
		else if(const ArrayProperty *aprop = prop % autocast)
		{
			JSObject *array_obj;
			
			if(JS_ValueToObject(cx, value, &array_obj) && JS_IsArrayObject(cx, array_obj))
			{
				jsuint len;
				
				if(JS_FALSE == JS_GetArrayLength(cx, array_obj, &len))
				{
					return false;
				}
				
				if(!aprop->Resize(opaque, len))
				{
					return false;
				}
				
				for(int index = 0; index < int(len); index++)
				{
					jsval elem;
					if(JS_GetElement(cx, array_obj, index, &elem))
					{
						Variant elem_value;
						JavaToVariant(cx, elem_value, elem);
						aprop->WriteData(opaque, unsigned(index), elem_value);
					}
				}
			}
			else
			{
				JS_ClearPendingException(cx); // eat exception from JS_ValueToObject?
				return false;
			}
		}
	}
	
	return true;
}

bool VariantToJava(JSContext *cx, jsval &val, const Variant &variant)
{
	const Type *variant_type = variant.GetType();
	bool is_ptr = variant_type->GetClass()->Derives<DynamicPointerType>(),
	     is_persist = variant_type->Derives<Persistent>(),
		 is_struct = variant_type->GetClass()->Derives<StructType>() &&
				    !variant_type->Abstract();
	
	if(variant_type == TypeOf<void>())
	{
		val = JSVAL_VOID;
		return true;
	}
	
	if(is_ptr || is_persist)
	{
		Dynamic *native_ptr;
		
		if(is_ptr) variant.ReadValue(native_ptr);
		else native_ptr = &variant.AsRef<Dynamic>();

		if(0 == native_ptr)
		{
			val = JSVAL_NULL;
			return true;
		}

		JSRuntime *runtime = JS_GetRuntime(cx);
		
		RuntimeData *data = GetRuntimeData(runtime);
		
		JSObject *object = data->GetJavaObject(cx, opaque_cast(native_ptr), native_ptr->GetClass(), JavaDynamicClass());
		
		val = OBJECT_TO_JSVAL(object);
	}
	else if(is_struct)
	{
		void *native_ptr = new char[variant_type->Size()];
		native_ptr = variant_type->Construct(native_ptr);

		Variant::FromOpaque(variant_type, native_ptr).Set(variant);

		JSRuntime *runtime = JS_GetRuntime(cx);
		
		RuntimeData *data = GetRuntimeData(runtime);
		
		JSObject *object = data->GetJavaObject(cx, native_ptr, variant_type, JavaStructClass());
		val = OBJECT_TO_JSVAL(object);
		
		data->DecRef(native_ptr);
	}
	else if(variant.GetType()->Derives<double>() || variant.GetType()->Derives<float>())
	{
		jsdouble number;
		variant.ReadValue(number);
		return JS_FALSE != JS_NewNumberValue(cx, number, &val);
	}
	else if(variant.CanConstRefAs<bool>())
	{
		JSBool value;
		value = variant.AsConstRef<bool>() ? JS_TRUE : JS_FALSE;
		val = BOOLEAN_TO_JSVAL(value);
		return true;
	}
	else if(variant.CanReadAs<long>())
	{
		jsint number;
		variant.ReadValue(number);
		val = INT_TO_JSVAL(number);
		return true;
	}
	else if(variant.CanReadAs<string::String>())
	{
		string::String str;
		variant.ReadValue(str);
		if(str.size())
			val = STRING_TO_JSVAL(JS_NewStringCopyN(cx, str.data(), str.size()));
		else
			val = JS_GetEmptyStringValue(cx);
	}
	else
	{
		return false;
	}
	
	return true;
}

bool JavaToVariant(JSContext *cx, Variant &variant, jsval val)
{
	JSType type;
	switch(type = JS_TypeOfValue(cx, val))
	{
	case JSTYPE_BOOLEAN:
		return variant.SetValue(JSVAL_TO_BOOLEAN(val) != JS_FALSE);
		break;
	case JSTYPE_VOID:
		break;
	case JSTYPE_STRING:
		{
			JSString *string = JSVAL_TO_STRING(val);
			const char *text = JS_GetStringBytes(string);
			return variant.SetValue(text);
		}
		break;
	case JSTYPE_XML:
	case JSTYPE_OBJECT:
		if(JSObject *object = JSVAL_TO_OBJECT(val))
		{
			if(JS_InstanceOf(cx, object, JavaDynamicClass(), 0))
			{
				// if the object is a native object...
				Dynamic *val = translucent_cast<Dynamic *>(JS_GetPrivate(cx, object));
	
				if(0 == variant.GetType() || val->GetClass()->DerivesType(variant.GetType()))
				{
					// reference pointer if variant is unset.
					return variant.RefValue(*val);
				}
				else if(variant.GetType()->GetClass()->Derives<DynamicPointerType>())
				{
					// copy by pointer if variant is a pointer
					return variant.SetValue(val);
				}
				else if(variant.CanWriteAs(val->GetClass()))
				{
					// copy by value if variant is a value
					return variant.SetValue(*val);
				}
			}

			if(JS_InstanceOf(cx, object, JavaStructClass(), 0))
			{
				const Type *type = GetNativeType(cx, object);
				void *opaque = JS_GetPrivate(cx, object);
				if(variant.Set(Variant::FromOpaque(type, opaque)))
				{
					return true;
				}
			}
			
			if(JS_InstanceOf(cx, object, FunctionClass(), 0))
			{
				const Type *type = GetNativeType(cx, object);
				void *opaque = JS_GetPrivate(cx, object);
				if(variant.ConstRef(Variant::FromOpaque(type, opaque)))
				{
					return true;
				}
			}
			
			// otherwise, copy data property-by-property if the variant is an object.
			if(variant.GetType() && variant.GetType()->GetClass()->Derives<StructType>() && JS_IsArrayObject(cx, object))
			{
				const StructType *struct_type = variant.GetType() % autocast;
				jsuint len;
				
				if(JS_FALSE == JS_GetArrayLength(cx, object, &len))
					return false;
					
				if(len < unsigned(struct_type->NumMembers()))
					return false;
				
				if(!variant.Construct())
					return false;
				
				for(int i = 0; i < struct_type->NumMembers(); i++)
				{
					jsval elem;
					JS_GetElement(cx, object, jsint(i), &elem);
					
					if(false == JavaToProperty(cx, variant.Opaque(), struct_type->GetMember(i), elem, type))
					{
						JS_ReportError(cx, "native: failed to convert element %d to a struct field", i);
						return false;
					}
				}
			}
			// otherwise, copy data property-by-property if the variant is an object.
			else if(variant.CanRefAs<Persistent>())
			{
				Persistent &native_obj = variant.AsRef<Persistent>();
				for(PersistentClass::PropertyIterator it = native_obj.GetClass(); it; it.next())
				{
					jsval value = JSVAL_VOID;
					if(JS_FALSE == JS_GetProperty(cx, object, it->first.c_str(), &value))
					{
						JS_ReportError(cx, "Error checking property: %s\n", it->first.c_str());
						return false;
					}
					
					if(false == JavaToProperty(cx, opaque_cast(&native_obj), it->second, value, type))
					{
						return false;
					}
				}
			}
			else
			{
				return false;
			}
		
			return true;
		}
		// fall through
	case JSTYPE_NULL:
		return variant.SetValue(static_cast<Dynamic *>(0));
	case JSTYPE_NUMBER:
		if(JSVAL_IS_INT(val))
		{
			return variant.SetValue(JSVAL_TO_INT(val));
		}
		else if(JSVAL_IS_DOUBLE(val))
		{
			return variant.SetValue(*JSVAL_TO_DOUBLE(val));
		}
		break;
	default:
		break;
	}
	
	return false;
}

} }
