#ifndef REFLECT_STRING_STRINGOUTPUTSTREAM_H_
#define REFLECT_STRING_STRINGOUTPUTSTREAM_H_

#include <reflect/OutputStream.h>
#include <reflect/string/String.h>

namespace reflect { namespace string {

class StringOutputStream : public OutputStream
{
public:
	using OutputStream::size_type;
	size_type Write(const void *data, size_type size)
	{
		mString += Fragment(static_cast<const char *>(data), size);
		return size;
	}

    ConstString Result() const { return mString; }
    String &Result() { return mString; }
private:
	String mString;
};

} }

#endif
