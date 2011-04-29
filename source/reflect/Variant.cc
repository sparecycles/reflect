#include <reflect/Variant.h>
#include <reflect/Reflector.h>
#include <reflect/utility/InOutReflector.h>
#include <reflect/string/StringInputStream.h>
#include <reflect/PrimitiveTypes.h>

namespace reflect {

Variant::Variant()
	: mAllocation(0)
	, mType(0)
	, mConstData(0)
	, mData(0)
{
	mLink = this;
}

Variant::Variant(void *opaque, const Type *type)
	: mAllocation(0)
	, mType(type)
	, mConstData(opaque)
	, mData(opaque)
{
	mLink = this;
}

Variant::Variant(const void *opaque, const Type *type)
	: mAllocation(0)
	, mType(type)
	, mConstData(opaque)
	, mData(0)
{
	mLink = this;
}

Variant::Variant(const Variant &other)
	: mAllocation(other.mAllocation)
	, mType(other.mType)
	, mConstData(other.mConstData)
	, mData(other.mData)
{
	mLink = this;	
	Share(other);
}

Variant::~Variant()
{
	Release();
}

Variant Variant::FromOpaque(const Type *type, void *opaque)
{
	return Variant(opaque, type);
}

Variant Variant::FromConstOpaque(const Type *type, const void *opaque)
{
	return Variant(opaque, type);
}

const Variant &Variant::operator =(const Variant &other)
{
	if(this != &other)
	{
		Release();

		mAllocation = other.mAllocation;
		mType = other.mType;
		mConstData = other.mConstData;
		mData = other.mData;
		Share(other);
	}
	
	return *this;
}

void Variant::ClearAllocation()
{
	if(mAllocation)
	{
		// Note: when a variant is created on a string literal,
		// it allocates a pointer value to use to point to the begining of the
		// string.  In (only) this case, there is an allocation but no mData pointer.
		// It shouldn't be a problem to not "destruct" a pointer.
		if(mData) mType->Destruct(mData);
		delete [] mAllocation;
		mType = 0;
		mConstData = mData = 0;
	}
}

void Variant::Release()
{
	bool copy_of_allocation_found = false;
	Variant *prev = 0;

	// traverse the ring list for other variants, check if they
	// have the same allocation, remember the previous variant in *prev*
	for(Variant *link = mLink; 
	    link != this;
        prev = link, link = link->mLink)
	{
		if(link->mAllocation == mAllocation)
		{
			copy_of_allocation_found = true;
		}
	}

	// if *prev* is set, then 
	// there is more than one variant in this ring,
	// unlink it from this.
	if(prev)
    {
		prev->mLink = mLink;
		mLink = this;
    }

	// if noone is sharing this allocation, clear it.
	if(false == copy_of_allocation_found)
		ClearAllocation();
}

void Variant::Share(const Variant &other)
{
	mLink = other.mLink;
	other.mLink = this;
}

void Variant::BindType(const Type *type)
{
	Release();
	mConstData = mData = 0;
	mType = type;
}

bool Variant::Construct(const Type *type, bool own_data)
{
	// default type parameter to bound class.
	if(type == 0)
	{
		type = mType;
		
		if(0 == type)
		{
			// user error. Can't construct without a bound type.
			return false;
		}
	}
	
	if(type == mType && 0 != mData)
	{
		// if own_data is not requested or,... it is but data is already owned,
		// then success.
		if(false == own_data || (0 != mAllocation && this == mLink))
			return true;
	}
	
	if(type->Abstract())
	{
		// wanted to construct but the type is abstract so failure.
		
		// void is ok, everything else is not.
		return type == TypeOf<void>();
	}
	
	Release();
	mType = type;
	mAllocation = new char[mType->Size()];
	mConstData = mData = mType->Construct(mAllocation);

	return true;
}

void Variant::Assimilate(Variant &value)
{
	Release();

	mType = value.mType;
	mConstData = mData = value.mData;
	mAllocation = value.mAllocation;

	value.Release();
	value.mAllocation = 0;
	value.mType = 0;
	value.mConstData = value.mData = 0;
}

bool Variant::ConvertToType(const Type *type)
{
	if(type == mType)
		return true;

	Variant new_value;
	if(new_value.Construct(type) && new_value.Set(*this))
	{
		Assimilate(new_value);
		return true;
	}

	return false;
}

bool Variant::Set(const Variant &value)
{
	if(0 == mData)
	{
		if(mConstData)
			return false;
		
		const Type *type = GetType();

		if(0 == type)
		{
			type = value.mType;
			if(0 == type)
				return false;
		}
		
		if(false == Construct(type))
			return false;
	}
	
	return mType->ConvertValue(mData, value.mConstData, value.mType);
}

Variant &Variant::Downcast()
{
	if(GetType()->GetClass()->Derives<Class>())
	{
		if(const void *opaque = mConstData)
		{
			const Dynamic *castable = translucent_cast<const Dynamic *>(opaque);
			mType = castable->GetClass();
		}
	}

	return *this;
}

bool Variant::ConstRef(const Variant &value)
{
	if(0 == GetType())
	{
		BindType(value.GetType());
	}
	else if(false == value.GetType()->DerivesType(GetType()))
	{
		return Set(value);
	}

	Release();
	mAllocation = value.mAllocation;
	mData = 0;
	mConstData = value.mConstData;
	Share(value);
	
	return true;
}

bool Variant::Ref(const Variant &value)
{
	if(0 == GetType())
	{
		BindType(value.GetType());
	}
	else if(false == value.GetType()->DerivesType(GetType()))
	{
		return false;
	}

	Release();
	mAllocation = value.mAllocation;
	mData = value.mData;
	mConstData = value.mConstData;
	Share(value);
	
	return true;
}

bool Variant::Alias(Variant &other)
{
	if(other.mData == 0)
		return false;
	
	Release();
	
	if(mType == other.mType)
	{
		mAllocation = other.mAllocation;
		mData = other.mData;
		mConstData = other.mConstData;
		Share(other);
		return true;
	}
	else if(!mType->Abstract())
	{
		// don't bother converting if we can't convert back.
		if(mType->CanConvertFrom(other.mType) && other.mType->CanConvertFrom(mType))
		{
			mAllocation = new char[mType->Size()];
			mConstData = mData = mType->Construct(mAllocation);
			bool result = Set(other);
			if(result)
			{
				Share(other);
				return true;
			}
			else
			{
				Release();
				return false;
			}
		}
	}
	
	return false;
}

bool Variant::ConstAlias(const Variant &value)
{
	return Ref(value) || Set(value);
}

void Variant::UpdateAliases() const
{
	for(Variant *other = mLink; other != this; other = other->mLink)
	{
		// assert ?
		if(other->mData && other->mData != mData)
			(void)other->Set(*this);
	}
}

bool Variant::CanConstRefAs(const Type *type) const
{
	return 0 != mConstData && mType->DerivesType(type);
}

bool Variant::CanRefAs(const Type *type) const
{
	return 0 != mData && mType->DerivesType(type);
}

bool Variant::CanReadAs(const Type *type) const
{
	return (CanConstRefAs(type) || type->CanConvertFrom(mType));
}

bool Variant::CanWriteAs(const Type *type) const
{
	return 0 != mData && (mType->CanConvertFrom(type));
}

void Variant::Serialize(Reflector &reflector) const
{
	if(reflector.Serializing())
	{
		if(0 == GetType() || 0 == mConstData)
		{
			reflector.Fail();
		}
		else
		{
			mType->Serialize(mConstData, mData, reflector);
		}
	}
	else
	{
		if(0 == mData)
		{
			reflector.Fail();
		}
		else
		{
			mType->Serialize(mConstData, mData, reflector);
		}
	}
}

const Type *Variant::GetType() const
{
	return mType;
}

string::String Variant::ToString() const
{
	utility::InOutReflector<> io;
	
	Reflector reflector(io.GetSerializer());
	
	Serialize(reflector);
	
	return io.Data();
}

bool Variant::FromString(string::Fragment fragment)
{
	if(mType == 0)
		return false;

	if(Construct())
	{
		string::StringInputStream input(fragment);
		serialize::StandardDeserializer deserializer(input);
		Reflector reflector(deserializer);
		
		return reflector | *this;
	}
	
	return false;
}

Variant &Variant::Void()
{
	static Variant sVoid(static_cast<const void *>(0), TypeOf<void>());
	
	return sVoid;
}

}
