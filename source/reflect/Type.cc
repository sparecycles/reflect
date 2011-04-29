#include <reflect/Class.hpp>
#include <reflect/Reflector.h>
#include <reflect/autocast.h>
#include <reflect/Serializer.h>
#include <reflect/Deserializer.h>
#include <reflect/PrimitiveTypes.h>
#include <reflect/string/ConstString.h>
#include <cstring>
#include <cstdio>
#include <map>

namespace reflect {

static Type *sTypeLink = 0;

static Type::TypeMapType sTypeMap;

static Type *sFirstRoot = 0;

const Type::TypeMapType &Type::GlobalTypeMap()
{
	return sTypeMap;
}

Type::Type(void (*init_cb)())
	: mParent(0)
	, mFirstChild(0)
	, mTypeInitializerCB(init_cb)
	, mName(0)
	, mDepth(-1)
	, mHierarchy(0)
	, mNextTypeToLoad(sTypeLink)
	, mSize(0)
	, mAlignment(1)
	, mConstructor(0)
	, mDestructor(0)
{
	mSibling = this;

	sTypeLink = this;
}

static void RemoveTypeFromLoadChain(Type *to_remove, Type *&chain, Type *Type::*next)
{
	if(chain)
	{
		if(to_remove == chain)
		{
			chain = chain->*next;
		}
		else
		{
			RemoveTypeFromLoadChain(to_remove, chain->*next, next);
		}
	}
}

Type::~Type()
{
	RemoveTypeFromLoadChain(this, sTypeLink, &Type::mNextTypeToLoad);
}

Type *Type::Parent() const
{
	return mParent;
}

void Type::SetParent(Type *parent)
{
	mParent = parent;
}

void Type::CompileHierarchy()
{
	if(mHierarchy)
		return;

	if(mFirstChild)
	{
		mFirstChild->CompileHierarchy();
		mDepth = mFirstChild->mDepth - 1;
		mHierarchy = mFirstChild->mHierarchy;
	}
	else
	{
		int depth = 0;

		for(Type *it = this; it != 0; it = it->Parent())
			depth++;

		mHierarchy = new Type *[depth];
		Type **rhierarchy = mHierarchy + depth;
		for(Type *it = this; it != 0; it = it->Parent())
			*--rhierarchy = it;

		mDepth = depth - 1; // index of self into mHierarchy, see Type::Derives
	}
}

void Type::LinkHierarchy()
{
	Type *&self_reference = *(Parent() ? &Parent()->mFirstChild : &sFirstRoot);

	if(self_reference)
	{
		mSibling = self_reference->mSibling;
		self_reference->mSibling = this;
	}
	else
	{
		self_reference = this;
	}
}

void Type::UnlinkHierarchy()
{
	// there are two mutually exclusive ways this class
	// may be referenced from higher in the hierarchy.
	Type *&possible_self_reference = *(Parent() ? &Parent()->mFirstChild : &sFirstRoot);

	// try to move the reference over
	// (this may not work, we'll fix that case below)
	if(this == possible_self_reference)
	{
		possible_self_reference = mSibling;
	}

	// remove self from sibling chain.
	
	// walk previous_sibling until it's next is this
	Type *previous_sibling = mSibling;

	while(previous_sibling->mSibling != this)
	{
		previous_sibling = previous_sibling->mSibling;
	}
	// relink to skip "this".
	previous_sibling->mSibling = mSibling;
	
	// relink sibling to self, just in case we relink.
	mSibling = this;

	// if the parent or root is still pointing to this as it's first child,
	// then we are the last child, so the parent has no children left.
	if(this == possible_self_reference)
	{
		possible_self_reference = 0;
	}
}

Type *Type::Child() const
{
	return mFirstChild;
}

Type *Type::Sibling() const
{
	return mSibling;
}

void Type::Init()
{
	Initialize();
}

void Type::Initialize()
{
	if(mTypeInitializerCB)
	{
		(*mTypeInitializerCB)();
	}
}

Type *Type::FirstTypeToLoad()
{
	return sTypeLink;
}

Type *Type::LoadTypes()
{
	Type *headlink = FirstTypeToLoad();

	sTypeLink = 0;

	for(Type *each = headlink; each != 0; each = each->NextTypeToLoad())
	{
		each->Init();
	}

	for(Type *each = headlink; each != 0; each = each->NextTypeToLoad())
	{
		each->LinkHierarchy();
	}

	for(Type *each = headlink; each != 0; each = each->NextTypeToLoad())
	{
		each->CompileHierarchy();
	}

	return headlink;
}

void Type::ReleaseCompiledHierarchy(Type **hierarchy)
{
	if(mHierarchy == hierarchy)
	{
		mHierarchy = 0;

		if(Type *child = Child()) do
		{
			child->ReleaseCompiledHierarchy(hierarchy);
		}
		while((child = child->Sibling()) != Child());
	
		if(Parent())
			Parent()->ReleaseCompiledHierarchy(hierarchy);
	}
}

void Type::UnloadTypes(Type *headlink)
{
	for(Type *each = headlink; each != 0; each = each->NextTypeToLoad())
	{
		if(each->Name())
		{ 
			Type::TypeMapType::iterator it = sTypeMap.find(string::SharedString::Find(each->Name()));
			if(it != sTypeMap.end() && it->second == each)
				sTypeMap.erase(it);
		}

		if(each->mHierarchy)
		{
			Type **hierarchy = each->mHierarchy;
			each->ReleaseCompiledHierarchy(hierarchy);
			delete hierarchy;
		}
	}

	for(Type *each = headlink; each != 0; each = each->NextTypeToLoad())
	{
		each->UnlinkHierarchy();
	}

	TypeOf<Dynamic>()->CompileHierarchy();
}

bool Type::DerivesType(const Type *other) const
{
	if(mHierarchy && other->mHierarchy)
	{
		return other->mDepth <= mDepth && mHierarchy[other->mDepth] == other;
	}
	else
	{
		// slower code, should never need to be run.

		const Type *iter = this;

		do
		{
			if(iter == other)
				return true;

			iter = iter->Parent();
		} while(iter != 0);

		return false;
	}
}

void Type::SetName(const char *name)
{
	mName = string::SharedString::Copy(name).c_str();

	RegisterName();
}

void Type::RegisterName()
{
	std::pair<Type::TypeMapType::iterator, bool> 
		ins = sTypeMap.insert(Type::TypeMapType::value_type(
			string::SharedString::Find(Name()), this));

	if(false == ins.second)
	{
		fprintf(stderr, "ERROR: Type %s already registered!\n", Name());
		//*(int*)0 = 1;
	}
}

Type *Type::AnyRootType()
{
	return sFirstRoot;
}

Type *Type::FindType(const char *name)
{
	if(string::SharedString str_name = string::SharedString::Find(name))
	{
		return FindType(str_name);
	}

	return 0;
}

Type *Type::FindType(string::FoundSharedString name)
{
	Type::TypeMapType::const_iterator it = sTypeMap.find(name);
	if(it != sTypeMap.end())
		return it->second;

	return 0;
}

const char *Type::Name() const
{
	return mName;
}

Type *Type::NextTypeToLoad() const
{
	return mNextTypeToLoad;
}

void Type::Serialize(const void *, void *, Reflector &reflector) const
{
	reflector.Fail();
}

void Type::SetBasicOperations(unsigned size, unsigned alignment, void *(*constructor)(void *), void *(*destructor)(void *))
{
	mSize = size;
	mAlignment = alignment;
	mConstructor = constructor;
	mDestructor = destructor;
}

unsigned Type::Size() const
{
	return mSize;
}

unsigned Type::Alignment() const
{
	return mAlignment;
}

bool Type::Abstract() const
{
	return Size() == 0;
}

void *Type::Construct(void *data) const
{
	void *result = 0;
	
	if(mConstructor)
	{
		result = (*mConstructor)(data);
	}
	
	return result;
}

void *Type::Destruct(void *data) const
{
	void *result = 0;
	
	if(mDestructor)
	{
		result = (*mDestructor)(data);
	}
	
	return result;
}

void Type::RegisterConversion(const Type *from, void (*convert)(void *, const void *))
{
	mConversions.insert(std::make_pair(from, convert));
}

bool Type::CanConvertFrom(const Type *from) const
{
	ConversionMap::const_iterator it = mConversions.find(from);

	if(it != mConversions.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Type::ConvertValue(void *opaque, const void *src, const Type *from) const
{
	if(TypeOf<void>() == this)
		return true;

	ConversionMap::const_iterator it = mConversions.find(from);
	
	if(it != mConversions.end())
	{
		Conversion conversion = it->second;
		(*conversion)(opaque, src);
		return true;
	}
	else
	{
		return false;
	}
}

class TypeType : public Class
{
public:
	TypeType(void (*init_cb)() = 0)
		: Class(init_cb)
	{}

	// virtual
	void SerializePointer(const Dynamic *in, Reflector &reflector) const
	{
		if(const Type *clazz = in % autocast)
		{
			if(clazz->Name()  && *clazz->Name())
			{
				reflector | string::ConstString(clazz->Name());
				return; // don't fail
			}
		}

		reflector.Fail();
	}
	
	// virtual
	void DeserializePointer(Dynamic *&out, Reflector &reflector) const
	{
		string::String name;

		if(reflector | name)
		{
			if(Type *clazz = Type::FindType(string::SharedString::Find(name)))
			{
				out = clazz;
			}
			else
			{
				reflector.Fail();
			}
		}

	}
};

}

DEFINE_PRIVATE_REFLECTION(reflect::Type, "reflect::Type", reflect::TypeType)
{
	Functions
		("Parent", &reflect::Type::Parent)
		("Derives", &reflect::Type::DerivesType)
		("FindType",
			static_cast<Type *(*)(reflect::string::FoundSharedString)>(
				&reflect::Type::FindType))
		(     "Name", &reflect::Type::Name)
		(    "Child", &reflect::Type::Child)
		(  "Sibling", &reflect::Type::Sibling)
		( "Abstract", &reflect::Type::Abstract)
		(     "Size", &reflect::Type::Size)
		("Alignment", &reflect::Type::Alignment)
		;
}
