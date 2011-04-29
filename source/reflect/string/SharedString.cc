#include <reflect/string/SharedString.h>
#include <reflect/string/String.h>
#include <reflect/string/StringPool.h>
#include <reflect/Reflector.h>
#include <reflect/Deserializer.h>
#include <reflect/utility/Context.hpp>
#include <reflect/PrimitiveType.hpp>
#include <reflect/PrimitiveTypes.h>

#ifndef REFLECT_FAST_SHARED_STRING
# define COMPARE(op,y) mpString op (y).mpString
#else
# define COMPARE(op,y) compare(y) op 0
#endif

template class reflect::utility::Context<reflect::string::StringPoolContext>;

namespace reflect { namespace string {

SharedString::SharedString(const char *text, const StringPool *)
	: mpString(text)
{
}

SharedString SharedString::Literal(const char *text)
{
	if(StringPoolContext *context = StringPoolContext::GetContext())
	{
		return context->Literal(text);
	}
	
	return SharedString();
}

SharedString SharedString::Copy(string::Fragment fragment)
{
	if(StringPoolContext *context = StringPoolContext::GetContext())
	{
		return context->Copy(fragment);
	}
	
	return SharedString();
}

SharedString SharedString::Find(string::Fragment fragment)
{
	if(StringPoolContext *context = StringPoolContext::GetContext())
	{
		return context->Find(fragment);
	}
	
	return SharedString();
}

SharedString::operator Fragment() const { return Fragment(c_str()); }
SharedString::operator ConstString() const { return ConstString(c_str()); }

#ifndef REFLECT_FAST_SHARED_STRING
bool SharedString::operator ==(const SharedString &other) const
{
	return COMPARE(==, other);
}

bool SharedString::operator <(const SharedString &other) const
{
	return COMPARE(<, other);
}
#endif

bool SharedString::operator <=(const SharedString &other) const
{
	return COMPARE(<=, other);
}

bool SharedString::operator >=(const SharedString &other) const
{
	return COMPARE(>=, other);
}


bool SharedString::operator  >(const SharedString &other) const
{
	return COMPARE(>, other);
}

bool SharedString::operator !=(const SharedString &other) const
{
	return COMPARE(!=, other);
}

SharedString::operator bool() const
{
	return mpString != NULL;
}

///////////////////////////////////////////////////////////

bool SharedString::empty() const
{
	return NULL == mpString || mpString[0] == '\0';
}

///////////////////////////////////////////////////////////

int SharedString::compare(const SharedString & rhs) const
{
#ifndef REFLECT_FAST_SHARED_STRING
	if(mpString == rhs.mpString) return 0;
	else if(mpString == NULL) return -1;
	else if(rhs.mpString == NULL) return 1;
	else return strcmp(mpString, rhs.mpString);
#else
	// not called.
	return 0;
#endif
}

///////////////////////////////////////////////////////////

StringPool::StringPool(const StringPool *parent)
	: mParent(parent)
{
}

const char *StringPool::FindRecursive(const Fragment &string) const
{
	bool search_result;
	const char *result = NULL;
	std::vector<const char *>::size_type pos = BinarySearch(string, search_result);

	if(search_result)
	{
		result = mStringPool[pos];
	}
	else if(mParent)
	{
		result = mParent->FindRecursive(string);
	}

	return result;
}

const char *StringPool::FindFirst(const Fragment &string, std::vector<const char *>::iterator &it)
{
	bool search_result;
	const char *result = NULL;
	std::vector<const char *>::size_type pos = BinarySearch(string, search_result);

	if(search_result)
	{
		result = mStringPool[pos];
	}
	else if(mParent)
	{
		result = mParent->FindRecursive(string);
	}

	if(NULL == result)
	{
		it = mStringPool.begin() + int(pos);
	}

	return result;
}

std::vector<const char *>::size_type StringPool::BinarySearch(const Fragment &string, bool &result) const
{
	std::vector<const char *>::size_type lo = 0;
	std::vector<const char *>::size_type hi = mStringPool.size();
	
	while(lo < hi)
	{
		std::vector<const char *>::size_type mid = (lo + hi) / 2;
		int cmp = string.compare(mStringPool[mid]);

		if(cmp < 0)
		{
			hi = mid;
		}
		else if(cmp > 0)
		{
			lo = mid + 1;
		}
		else
		{
			result = true;
			return mid;
		}
	}
	
	result = false;
	return lo;
}

SharedString StringPool::Copy(const Fragment &s)
{
	std::vector<const char *>::iterator insertion_point;
	const char *string = FindFirst(s, insertion_point);
	if(string == NULL)
	{
		char *new_string = new char[s.length() + 1];
		std::memcpy(new_string, s.data(), s.length());
		new_string[s.length()] = '\0';

		string = new_string;
		mStringPool.insert(insertion_point, string);
	}

	return SharedString(string, this);
}

SharedString StringPool::Literal(const ConstString &s)
{
	std::vector<const char *>::iterator insertion_point;
	const char *string = FindFirst(s, insertion_point);
	if(string == NULL)
	{
		string = s.c_str();
		mStringPool.insert(insertion_point, string);
	}

	return SharedString(string, this);
}

SharedString StringPool::Find(const Fragment &s) const
{
	return SharedString(FindRecursive(s), this);
}

StringPoolContext::StringPoolContext()
	: StringPool(StringPoolContext::GetContext())
{
}

static StringPoolContext sGlobalStringPoolContext;

///////////////////////////////////////////////////

void SharedString::Serialize(const SharedString *in, SharedString *out, reflect::Reflector &reflector)
{
	if(reflector.Serializing())
	{
		reflector | string::Fragment(*in);
	}
	else
	{
		string::String the_string;

		reflector | the_string;

		if(reflector.Ok())
			*out = string::SharedString::Copy(the_string.c_str());
	}
}

} }

DEFINE_STATIC_REFLECTION(reflect::string::FoundSharedString, "found_shared_string")
{
	SerializeAsWith<reflect::string::SharedString, &reflect::string::SharedString::Serialize>();
	
	+ Concrete;
	+ ConversionTo<const char *>();
	+ ConversionFrom<const char *>();
	//+ ConversionTo<reflect::string::ConstString>();
	//+ ConversionTo<reflect::string::Fragment>();
	//+ ConversionToEx<reflect::string::MutableString, reflect::string::Fragment>();
	//+ ConversionToEx<reflect::string::String, reflect::string::Fragment>();
}

DEFINE_STATIC_REFLECTION(reflect::string::SharedString, "shared_string")
{
	SerializeWith<&reflect::string::SharedString::Serialize>();
	
	+ Concrete;
	+ ConversionTo<reflect::string::ConstString>();
	+ ConversionTo<reflect::string::Fragment>();
	+ ConversionToEx<reflect::string::MutableString, reflect::string::Fragment>();
	+ ConversionToEx<reflect::string::String, reflect::string::Fragment>();
}
