#ifndef REFLECT_STRING_ARRAYSTRING_H_
#define REFLECT_STRING_ARRAYSTRING_H_

#include <reflect/string/MutableString.h>
#include <reflect/string/Fragment.h>
#include <reflect/string/ConstString.h>
#include <cstring>

namespace reflect { namespace string {

/// A string holding a fixed-length array for its data.
///
/// All Strings operate in a manner compatible with std::string
/// wherever feasable in the functions implemented.  ArrayStrings
/// have a maximum size, and can be passed generically to other
/// functions as MutableStrings, without loss of functionality.
///
/// @param template_size
///        the maximum length of the string, not including the NUL terminator.
///
/// @see Fragment
/// @see ConstString
/// @see MutableString
/// @see String
template<unsigned template_size>
class ArrayString
{
public:
	typedef Fragment::size_type size_type;
	static const size_type npos = size_type(Fragment::npos);

	typedef const char *const_iterator;
	typedef char *iterator;

	/// Constructs an array string from a Fragment.
	ArrayString(const Fragment &s);
	/// Constructs an array string from a NUL-terminated c-string.
	ArrayString(const char *s);
	/// Constructs an empty ArrayString.
	ArrayString();

	/// @return a chararcter pointer to the data.
	const char *data() const;
	/// @return a chararcter pointer to NUL terminated data.
	const char *c_str() const;
	/// @return the current length of the string.
	size_type length() const;
	/// @return the current length of the string.
	size_type size() const;
	/// @return the maximum length of the string.
	size_type capacity() const;

	/// @return a const_iterator at the begining of the string.
	const_iterator begin() const;
	/// @return a const_iterator at the end of the string.
	const_iterator end() const;
	/// @return an iterator at the end of the string.
	iterator begin();
	/// @returns an iterator at the end of the string.
	iterator end();

	/// Casts this string to a Fragment.
	/// Do not operate on the ArrayString
	/// during the Fragment's lifetime.
	/// @return A Fragment interface to the ArrayString.
	operator Fragment() const;
	/// Casts this string to a ConstString.
	/// Do not operate on the ArrayString
	/// during the ConstString's lifetime.
	/// @return A ConstString interface to the ArrayString.
	operator ConstString() const;
	/// Casts this string to a MutableString.
	/// Note that this mutable string is a view on the ArrayString,
	/// not a copy, and can be used to edit the ArrayString.
	///
	/// Multiple MutableStrings from the same ArrayString
	/// will remain correctly aliased to each-other.
	///
	/// @return A MutableString interface to the ArrayString.
	operator MutableString();

	bool operator ==(const Fragment &other) const;
	bool operator <=(const Fragment &other) const;
	bool operator >=(const Fragment &other) const;
	bool operator < (const Fragment &other) const;
	bool operator > (const Fragment &other) const;
	bool operator !=(const Fragment &other) const;
private:
	/// The size of the string.
	enum { TemplateSize = template_size };
	/// The array of character data
	char mArray[TemplateSize + 1];
	/// The length of the string.
	size_type mLength;
};

/////////////////////////////////////////////////////////

template<unsigned size>
inline ArrayString<size>::ArrayString(const char *s)
{
	std::strncpy(mArray, s, TemplateSize);
	mArray[TemplateSize] = '\0';
	mLength = std::strlen(mArray); // Ideally this would be strnlen, but that's not supported on CW.
}

template<unsigned size>
inline ArrayString<size>::ArrayString()
	: mLength(0)
{
	mArray[0] = '\0';
}

template<unsigned size>
inline ArrayString<size>::ArrayString(const Fragment &s)
	: mLength(0)
{
	MutableString(*this) = s;
}

template<unsigned size>
inline const char *ArrayString<size>::data() const { return mArray; }
template<unsigned size>
inline const char *ArrayString<size>::c_str() const { return mArray; }
template<unsigned size>
inline typename ArrayString<size>::size_type ArrayString<size>::length() const { return mLength; }
template<unsigned size>
inline typename ArrayString<size>::size_type ArrayString<size>::size() const { return mLength; }
template<unsigned size>
inline typename ArrayString<size>::size_type ArrayString<size>::capacity() const { return TemplateSize; }

template<unsigned size>
inline typename ArrayString<size>::const_iterator ArrayString<size>::begin() const { return data(); }
template<unsigned size>
inline typename ArrayString<size>::const_iterator ArrayString<size>::end() const { return data() + length(); }
template<unsigned size>
inline typename ArrayString<size>::iterator ArrayString<size>::begin() { return data(); }
template<unsigned size>
inline typename ArrayString<size>::iterator ArrayString<size>::end() { return data() + length(); }

template<unsigned size>
inline ArrayString<size>::operator Fragment() const { return Fragment(data(), length()); }
template<unsigned size>
inline ArrayString<size>::operator ConstString() const { return ConstString(c_str()); }
template<unsigned size>
inline ArrayString<size>::operator MutableString() { return MutableString(mArray, capacity() + 1, mLength); }

template<unsigned size>
inline bool ArrayString<size>::operator ==(const Fragment &other) const
{
	return Fragment(*this) == other;
}

template<unsigned size>
inline bool ArrayString<size>::operator <=(const Fragment &other) const 
{
	return Fragment(*this) <= other;
}

template<unsigned size>
inline bool ArrayString<size>::operator >=(const Fragment &other) const 
{
	return Fragment(*this) >= other;
}

template<unsigned size>
inline bool ArrayString<size>::operator < (const Fragment &other) const 
{
	return Fragment(*this) <  other;
}

template<unsigned size>
inline bool ArrayString<size>::operator > (const Fragment &other) const 
{
	return Fragment(*this) >  other;
}

template<unsigned size>
inline bool ArrayString<size>::operator !=(const Fragment &other) const 
{
	return Fragment(*this) != other;
}

} }

#endif
