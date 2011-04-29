#ifndef REFLECT_STRING_RESIZABLESTRING_H_
#define REFLECT_STRING_RESIZABLESTRING_H_

#include <reflect/string/Fragment.h>
#include <reflect/string/MutableString.h>
#include <reflect/Reflector.h>

#include <reflect/config/config.h>
#include <reflect/Reflection.h>

namespace reflect { namespace string {

/// A String is the most complex string type in the suite.
/// 
/// Strings own their data, like ArrayString&nbsp;s,
/// but will never overflow since they can reallocate.
class ReflectExport(reflect) String
{
public:
	typedef Fragment::size_type size_type;
	static const size_type npos = size_type(Fragment::npos);
	typedef char *iterator;
	typedef const char *const_iterator;

	/// Constructs an empty String
	String();
	/// Constructs a String from a pointer to NUL terminated data.
	String(const char *s);
	/// Constructs a String from a Fragment.
	String(const Fragment &s);
	/// Constructs a String from a Fragment.
	String(const String &s);

	/// @return a chararcter pointer to the data.
	const char *data() const;
	/// @return a chararcter pointer to NUL terminated data.
	const char *c_str() const;
	/// @return the current length of the string.
	size_type length() const;
	/// @return the current length of the string.
	size_type size() const;
	/// @return the current maximum length of the string.
	size_type capacity() const;

	/// releases the memory held by the string
	~String();

	/// prints formated text into the string, (like printf)
    size_type format(const char *format, ...);
	/// prints formated text into the string, (like vprintf)
    size_type vformat(const char *format, va_list);

	/// @return a const_iterator at the begining of the string.
	const_iterator begin() const;
	/// @return a const_iterator at the end of the string.
	const_iterator end() const;
	/// @return an iterator at the end of the string.
	iterator begin();
	/// @returns an iterator at the end of the string.
	iterator end();

	/// @see Fragment::find(char,size_type) const
	size_type find(char ch, size_type pos = 0) const;
	/// @see Fragment::find(const char *,size_type,size_type) const
	size_type find(const char *s, size_type pos = 0, size_type len = npos) const;
	/// @see Fragment::find(const Fragment &,size_type) const
	size_type find(const Fragment &s, size_type pos = 0) const;
	/// @see Fragment::find_first_of(char,size_type) const
	size_type find_first_of(char ch, size_type pos = 0) const;
	/// @see Fragment::find_first_of(const char *,size_type) const
	size_type find_first_of(const Fragment &s, size_type pos = 0) const;

	/// @see Fragment::rfind(char,size_type) const
	size_type rfind(char ch, size_type pos = npos) const;
	/// @see Fragment::rfind(const char *,size_type,size_type) const
	size_type rfind(const char *s, size_type pos = npos, size_type len = npos) const;
	/// @see Fragment::rfind(const Fragment &,size_type,size_type) const
	size_type rfind(const Fragment &s, size_type pos = npos) const;
	/// @see Fragment::find_last_of(char,size_type) const
	size_type find_last_of(char ch, size_type pos = npos) const;
	/// @see Fragment::find_last_of(const char *,size_type) const
	size_type find_last_of(const Fragment &s, size_type pos = npos) const;
	
	/// @see Fragment::substr(size_type,size_type) const
	Fragment substr(size_type index, size_type amount = npos) const;

	/// @see MutableString::replace()
	void replace(size_type offset, size_type size, Fragment value = Fragment("", 0));

	bool empty() const;

	bool operator ==(const Fragment &other) const;
	bool operator <=(const Fragment &other) const;
	bool operator >=(const Fragment &other) const;
	bool operator < (const Fragment &other) const;
	bool operator > (const Fragment &other) const;
	bool operator !=(const Fragment &other) const;

	const String &operator =(const String &other);
	const String &operator =(const Fragment &other);
	const String &operator =(const char *other);
	const String &operator +=(const Fragment &other);
	const String &operator +=(const char *other);
	const String &operator +=(char);
	String &operator <<(const Fragment &other);
	String &operator <<(const char *other);
	String &operator <<(char);

	operator ConstString() const;
	operator Fragment() const;
	operator MutableString();

	void reserve(size_type size);

	static String formatted(const char *format, ...);

	static void Serialize(const String *in, String *out, Reflector &);

private:
	void Serialize(Reflector &) const;
	void Deserialize(Reflector &);

	void copy(const char *s, size_type len);

	/// Resizes the string to newsize if size is less than required,
	/// retaining the number of characters specified by keep.
	/// @param required the threshold below which resize will occur.
	/// @param keep the amount of text to keep,
	///        (sometimes we don't need to retain the old text after a resize.)
    void resize(size_type required, size_type keep, size_type newsize);

	char *mpBuffer;
	size_type mLength;
	size_type mMaxSize;
};

} }

DECLARE_PRIMITIVE_REFLECTION(reflect, reflect::string::String)

#endif
