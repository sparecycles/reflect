#include <reflect/PropertyPath.h>
#include <reflect/Persistent.h>

#include <reflect/DataProperty.h>
#include <reflect/ArrayProperty.h>
#include <reflect/MapProperty.h>

#include <reflect/serialize/StandardSerializer.h>
#include <reflect/serialize/StandardDeserializer.h>

#include <reflect/string/StringOutputStream.h>
#include <reflect/string/StringInputStream.h>
#include <reflect/string/SharedString.h>

namespace reflect {

PropertyPath::PropertyPath()
	: mType(Invalid)
	, mInternalType(InvalidInternalType)
	, mConstObject(0)
	, mObject(0)
	, mProperty(0)
	, mArrayIndex(0)
	, mAnnotations(0)
{
}

PropertyPath::PropertyPath(
		const Persistent *in,
		Persistent *out)
	: mType(Object)
	, mInternalType(ObjectInternalType)
	, mConstObject(in)
	, mObject(out)
	, mProperty(0)
	, mArrayIndex(0)
	, mAnnotations(0)
{
	if(in == 0)
	{
		mType = Invalid;
		mInternalType = InvalidInternalType;
	}
}

PropertyPath::PropertyPath(
		const Persistent *in)
	: mType(Object)
	, mInternalType(ObjectInternalType)
	, mConstObject(in)
	, mObject(0)
	, mProperty(0)
	, mArrayIndex(0)
	, mAnnotations(0)
{
	if(in == 0)
	{
		mType = Invalid;
		mInternalType = InvalidInternalType;
	}
}

PropertyPath::PropertyPath(
		Persistent *out)
	: mType(Object)
	, mInternalType(ObjectInternalType)
	, mConstObject(out)
	, mObject(out)
	, mProperty(0)
	, mArrayIndex(0)
	, mAnnotations(0)
{
	if(out == 0)
	{
		mType = Invalid;
		mInternalType = InvalidInternalType;
	}
}

bool PropertyPath::Advance(PropertyPath &to, string::Fragment path) const
{
	to = *this;
	return to.Advance(path);
}

PropertyPath PropertyPath::Advanced(string::Fragment path) const
{
	PropertyPath result(*this);
	if(result.Advance(path))
		return result;
	else
		return PropertyPath();
}

bool PropertyPath::Advance(string::Fragment path)
{
	PropertyPath vanguard(*this);

	if(vanguard.AdvanceInternal(path))
	{
		*this = vanguard;
		return true;
	}
	
	return false;
}
	
bool PropertyPath::AdvanceInternal(string::Fragment path)
{
	if(mType != Invalid) while(path.size())
	{
		switch(path.data()[0])
		{
		case '.':
			if(IsRef() && IsDatum())
			{
				Dynamic *obj = ReadValue<Dynamic *>();
				*this = PropertyPath(obj % autocast, obj % autocast);
				path = path.substr(1);
			}
			else return false;
			
			break;

		case '[':
			if(IsArray())
			{
				path = path.substr(1);
				
				string::Fragment::size_type end = path.find(']');

				if(end != string::Fragment::npos)
				{
					utility::InOutReflector<> io(path.substr(0, end));					
					unsigned index;

					if(io >> index)
					{
						*this = ArrayItem(index);
						path = path.substr(end + 1);					
					}
					else return false;
				}
				else return false;
			}
			else return false;
			break;
			
		case '{':
			if(IsMap())
			{
				path = path.substr(1);
				
				string::Fragment::size_type end = path.find('}');

				if(end != string::Fragment::npos)
				{
					string::Fragment map_key = path.substr(0, end);
					path = path.substr(end + 1);
					*this = MapItem(map_key);
					
					if(mType == Invalid)
						return false;
				}
				else return false;
			}
			else return false;
			
			break;

		default:
			if(mType == Object)
			{
				string::Fragment::size_type end = path.find_first_of(".{[");
				if(end == string::Fragment::npos) end = path.size();
				string::Fragment propname = path.substr(0, end);
				path = path.substr(end);
				
				mAnnotations = mConstObject->GetClass()->GetAnnotations(propname);
				
				if(const Property *prop = mConstObject->GetClass()->FindProperty(propname))
				{
					mProperty = prop;
					mInternalType = PropertyInternalType;
					
					if(const MapProperty *map_prop = prop % autocast)
					{
						(void)map_prop;
						mType = Map;
					}
					else if(const ArrayProperty *array_prop = prop % autocast)
					{
						(void)array_prop;
						mType = Array;
					}
					else if(const DataProperty *data_prop = prop % autocast)
					{
						(void)data_prop;
						mType = Data;
					}
					else
					{
						mType = Opaque;
					}
				}
				else return false;
			}
			else return false;
			
			break;
		}
	}
	else return false;
	
	return true;
}

bool PropertyPath::WriteData(const Variant &value) const
{
	// optimized data path that bypasses serialization...
	if(GetType() == Data) switch(GetInternalType())
	{
	case PropertyInternalType:
		if(const DataProperty *data_prop = mProperty % autocast)
		{
			if(data_prop->WriteData(mObject, value))
			{
				return true;
			}
		}
		break;
	case ArrayItemPropertyInternalType:
		if(const ArrayProperty *data_prop = mProperty % autocast)
		{
			if(data_prop->WriteData(mObject, mArrayIndex, value))
			{
				return true;
			}
		}
		break;
	case MapItemPropertyInternalType:
		if(const MapProperty *data_prop = mProperty % autocast)
		{
			if(data_prop->WriteData(mObject, mKey, value))
			{
				return true;
			}
		}
		break;
	default:
		return false;
	}
	
	utility::InOutReflector<reflect::serialize::StandardSerializer, reflect::serialize::StandardDeserializer> io;
	
	Reflector reader(io.GetSerializer());
	value.Serialize(reader);
	io.Check(reader);
	
	if(io.Ok())
	{
		Reflector writer(io.GetDeserializer());
		Write(writer);
		io.Check(writer);
	}

	return io.Ok();
}

void PropertyPath::Write(Reflector &reflector) const
{
	if(mObject == NULL)
	{
		reflector.Fail();
	}
	else switch(GetInternalType())
	{
	case PropertyInternalType:
		mProperty->Serialize(mConstObject, mObject, reflector);
		break;
		
	case ArrayItemPropertyInternalType:
		if(const ArrayProperty *array_property = mProperty % autocast)
		{
			if(mArrayIndex < array_property->Size(mConstObject))
			{
				array_property->SerializeItem(mConstObject, mObject, mArrayIndex, reflector);
			}
			else
			{
				reflector.Fail();
			}
		}
		else
		{
			reflector.Fail();
		}
		
		break;

	case MapItemPropertyInternalType:
		if(const MapProperty *map_property = mProperty % autocast)
		{
			Variant value;
			reflector.Check(value.Construct(map_property->ValueType()));
			if(reflector | value)
				reflector.Check(map_property->WriteData(mObject, mKey, value));
		}
		else
		{
			reflector.Fail();
		}
		
		break;

	case ObjectInternalType:
		mObject->GetClass()->DeserializeProperties(mObject, reflector);
		break;		
		
	case InvalidInternalType:
		reflector.Fail();
		break;
	}
}

bool PropertyPath::ReadData(Variant &value) const
{
	if(mConstObject == NULL)
		return false;
	else switch(GetInternalType())
	{
	case PropertyInternalType:
		if(const DataProperty *data_prop = mProperty % autocast)
		{
			return data_prop->ReadData(mConstObject, value);
		}
		else
		{
		}	
		break;

	case ArrayItemPropertyInternalType:
		if(const ArrayProperty *array_property = mProperty % autocast)
		{
			if(mArrayIndex < array_property->Size(mConstObject))
			{
				return array_property->ReadData(mConstObject, mArrayIndex, value);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
		
		break;

	case MapItemPropertyInternalType:
		if(const MapProperty *map_property = mProperty % autocast)
		{
			return map_property->ReadData(mConstObject, mKey, value);
		}		
		break;

	case ObjectInternalType:
		//mConstObject->GetClass()->SerializeProperties(mConstObject, reflector);
		break;
		
	case InvalidInternalType:
		return false;
	}

	return false;
}

void PropertyPath::Read(Reflector &reflector) const
{
	if(mConstObject == NULL)
		reflector.Fail();
	else switch(GetInternalType())
	{
	case PropertyInternalType:
		mProperty->Serialize(mConstObject, NULL, reflector);
		break;

	case ArrayItemPropertyInternalType:
		if(const ArrayProperty *array_property = mProperty % autocast)
		{
			if(mArrayIndex < array_property->Size(mConstObject))
			{
				array_property->SerializeItem(mConstObject, NULL, mArrayIndex, reflector);
			}
			else
			{
				reflector.Fail();
			}
		}
		else
		{
			reflector.Fail();
		}
		
		break;
		
	case MapItemPropertyInternalType:
		if(const MapProperty *map_property = mProperty % autocast)
		{
			Variant value;
			reflector.Check(map_property->ReadData(mConstObject, mKey, value));
			reflector | value;
		}
		else
		{
			reflector.Fail();
		}
		
		break;

	case ObjectInternalType:
		mConstObject->GetClass()->SerializeProperties(mConstObject, reflector);
		break;
		
	case InvalidInternalType:
		reflector.Fail();
		break;
	}
}

bool PropertyPath::Writef(const char *format, ...) const
{
	string::String string;
	va_list args;
	va_start(args, format);
	string.vformat(format, args);
	va_end(args);
	
	return Write(string);
}

bool PropertyPath::Write(string::Fragment data) const
{
	string::StringInputStream input(data);
	serialize::StandardDeserializer serializer(input);
	Reflector reflector(serializer);

	Write(reflector);

	return reflector.Ok();
}

bool PropertyPath::Read(string::MutableString result) const
{
	string::String result_data;

	if(Read(result_data))
	{
		result = string::Fragment(result_data);
		return true;
	}

	return false;
}

bool PropertyPath::Read(string::String &result) const
{
	string::StringOutputStream output;
	serialize::StandardSerializer serializer(output);
	Reflector reflector(serializer);

	Read(reflector);

	if(reflector.Ok())
	{
		result = output.Result();
	}

	return reflector.Ok();
}

bool PropertyPath::IsRef() const
{
	return IsDatum() && GetDataType()->GetClass()->Derives<DynamicPointerType>();
}

bool PropertyPath::IsArray() const
{
	return GetType() == Array;
}

bool PropertyPath::IsMap() const
{
	return GetType() == Map;
}

bool PropertyPath::IsDatum() const
{
	return GetType() == Data;
}

bool PropertyPath::IsData() const
{
	return GetType() == Data && !IsRef();
}

bool PropertyPath::IsOpaque() const
{
	return GetType() == Opaque;
}

bool PropertyPath::IsMapData() const
{
	return GetInternalType() == MapItemPropertyInternalType;
}

bool PropertyPath::IsArrayData() const
{
	return GetInternalType() == ArrayItemPropertyInternalType;
}

bool PropertyPath::DataExists() const
{
	if(IsMapData())
	{
		if(const MapProperty *map_prop = mProperty % autocast)
		{
			return map_prop->HasKey(mConstObject, mKey);
		}
		
		return false;
	}
	else if(IsArrayData())
	{
		if(const ArrayProperty *array_prop = mProperty % autocast)
		{
			return array_prop->Size(mConstObject) > mArrayIndex;
		}
		
		return false;
	}
	
	return true;
}

PropertyPath::InternalType PropertyPath::GetInternalType() const
{
	return mInternalType;
}

string::String PropertyPath::Read() const
{
	string::String result;
	Read(result);
	return result;
}

PropertyPath::NormalizedType PropertyPath::GetType() const
{
	return mType;
}

unsigned PropertyPath::ArraySize() const
{
	if(const ArrayProperty *array_prop = mProperty % autocast)
	{
		return array_prop->Size(mConstObject);
	}

	return 0;
}

bool PropertyPath::ArrayResize(unsigned newsize) const
{
	if(const ArrayProperty *array_prop = mProperty % autocast)
	{
		return array_prop->Resize(mObject, newsize);
	}

	return 0;
}

PropertyPath PropertyPath::ArrayItem(unsigned index) const
{
	if(!IsArray())
	{
		return PropertyPath();
	}

	if(const ArrayProperty *array_prop = mProperty % autocast)
	{
		if(array_prop->Resizable(mConstObject) || index < array_prop->Size(mConstObject))
		{
			PropertyPath result(*this);
			result.mArrayIndex = index;
			result.mInternalType = ArrayItemPropertyInternalType;
			result.mType = Data;
			return result;
		}	
	}

	return PropertyPath();
}

PropertyPath PropertyPath::MapItem(string::Fragment key) const
{
	PropertyPath item(*this);
						
	item.mInternalType = MapItemPropertyInternalType;
	
	if(const MapProperty *map_prop = item.mProperty % autocast)
	{
		string::StringInputStream key_stream(key);
		serialize::StandardDeserializer key_deser(key_stream);
		Reflector reflector(key_deser);
		item.mKey.Construct(map_prop->KeyType());
		reflector | item.mKey;
		
		if(!reflector.Ok())
			return PropertyPath();
		
		item.mType = Data;
		
		return item;
	}
	
	return PropertyPath();
}

bool PersistentClass::WriteProperty(Persistent *object, string::Fragment path, string::Fragment value) const
{
	PropertyPath pathresult;

	if(ResolvePropertyPath(pathresult, object, path))
	{
		return pathresult.Write(value);
	}

	return false;
}

bool PersistentClass::WriteStringProperty(Persistent *object, string::Fragment path, string::Fragment value) const
{
	PropertyPath pathresult;

	if(ResolvePropertyPath(pathresult, object, path))
	{
		string::StringOutputStream data;
		reflect::serialize::StandardSerializer serializer(data);
		
		if(serializer | value)
		{
			return pathresult.Write(data.Result());
		}
	}

	return false;
}
bool PersistentClass::ReadProperty(const Persistent *object, string::Fragment path, string::String &value) const
{
	PropertyPath pathresult;

	if(ResolvePropertyPath(pathresult, object, path))
	{
		return pathresult.Read(value);
	}

	return false;
}

bool PersistentClass::ReadStringProperty(const Persistent *object, string::Fragment path, string::String &value) const
{
	PropertyPath pathresult;
	string::String data;

	if(ResolvePropertyPath(pathresult, object, path))
	{
		if(pathresult.Read(data))
		{
			string::StringInputStream stream(data);
			reflect::serialize::StandardDeserializer deserializer(stream);
			
			return deserializer | value;
		}
	}

	return false;
}
bool PersistentClass::WriteRefProperty(Persistent *object, string::Fragment path, Dynamic *value) const
{
	PropertyPath pathresult;

	if(ResolvePropertyPath(pathresult, object, path))
	{
		pathresult.WriteValue(value);
		return true;
	}

	return false;
}

bool PersistentClass::ReadRefProperty(const Persistent *object, string::Fragment path, Dynamic *&value) const
{
	PropertyPath pathresult;

	if(ResolvePropertyPath(pathresult, object, path))
	{
		value = pathresult.ReadValue<Dynamic *>();
		return true;
	}

	return false;
}

const Type *PropertyPath::GetDataType() const
{
	if(GetType() == Data) switch(GetInternalType())
	{
	case PropertyInternalType:
		if(const DataProperty *data_prop = mProperty % autocast)
		{
			return data_prop->DataType();
		}
		
		break;
		
	case ObjectInternalType:
		break;
		
	case ArrayItemPropertyInternalType:
		if(const ArrayProperty *array_prop = mProperty % autocast)
		{
			return array_prop->ItemType();
		}
		
		break;
		
	case MapItemPropertyInternalType:
		if(const MapProperty *map_prop = mProperty % autocast)
		{
			return map_prop->ValueType();
		}
		
		break;
		
	case InvalidInternalType:
	default:
		break;
	}
	
	return 0;
}

const Class *PropertyPath::GetRefClass() const
{
	if(IsRef()) switch(GetInternalType())
	{
	case PropertyInternalType:
		if(const DataProperty *data_prop = mProperty % autocast)
		{
			if(const DynamicPointerType *ref_class = data_prop->DataType() % autocast)
			{
				return ref_class->ValueClass();
			}
		}
		break;
		
	case ObjectInternalType:
		break;
		
	case ArrayItemPropertyInternalType:
		if(const ArrayProperty *array_prop = mProperty % autocast)
		{
			if(const DynamicPointerType *ref_class = array_prop->ItemType() % autocast)
			{
				return ref_class->ValueClass();
			}
		}
		break;
		
	case MapItemPropertyInternalType:
		if(const MapProperty *map_prop = mProperty % autocast)
		{
			if(const DynamicPointerType *ref_class = map_prop->ValueType() % autocast)
			{
				return ref_class->ValueClass();
			}
		}
		break;
		
	case InvalidInternalType:
	default:
		break;
	}
	
	return 0;
}

const Type *PropertyPath::GetArrayItemType() const
{
	if(IsArray()) switch(GetInternalType())
	{
	case PropertyInternalType:
		if(const ArrayProperty *array_prop = mProperty % autocast)
		{
			return array_prop->ItemType();
		}
		break;
		
	default:
		break;
	}
	
	return 0;
}

const Class *PropertyPath::GetArrayItemRefClass() const
{
	if(IsArray()) switch(GetInternalType())
	{
	case PropertyInternalType:
		if(const ArrayProperty *array_prop = mProperty % autocast)
		{
			if(const DynamicPointerType *ref_class = array_prop->ItemType() % autocast)
			{
				return ref_class->ValueClass();
			}
		}
		
		break;
		
	default:
		break;
	}
	
	return 0;
}

const Type *PropertyPath::GetMapKeyType() const
{
	if(IsMap()) switch(GetInternalType())
	{
	case PropertyInternalType:
		if(const MapProperty *map_prop = mProperty % autocast)
		{
			return map_prop->KeyType();
		}
		
		break;
		
	default:
		break;
	}
	
	return 0;
}

const Type *PropertyPath::GetMapValueType() const
{
	if(IsMap()) switch(GetInternalType())
	{
	case PropertyInternalType:
		if(const MapProperty *map_prop = mProperty % autocast)
		{
			return map_prop->ValueType();
		}
		
		break;
		
	default:
		break;
	}
	
	return 0;
}

const Class *PropertyPath::GetMapRefValueClass() const
{
	if(IsMap()) switch(GetInternalType())
	{
	case PropertyInternalType:
		if(const MapProperty *map_prop = mProperty % autocast)
		{
			if(const DynamicPointerType *ref_class = map_prop->ValueType() % autocast)
			{
				return ref_class->ValueClass();
			}
		}
		break;
		
	default:
		break;
	}
	
	return 0;
}

const PersistentClass::Annotations *PropertyPath::Annotations() const
{
	return mAnnotations;
}

Variant PropertyPath::Annotation(string::Fragment name) const
{
	if(mAnnotations)
	{
		PersistentClass::Annotations::const_iterator it 
			= mAnnotations->find(string::SharedString::Find(name));
		if(it != mAnnotations->end())
			return it->second;
	}
	
	return Variant::Void();
}

}
