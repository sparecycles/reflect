#ifndef REFLECT_STRING_STRINGINPUTSTREAM_H_
#define REFLECT_STRING_STRINGINPUTSTREAM_H_

#include <reflect/InputStream.h>

namespace reflect { namespace string {

class StringInputStream : public reflect::InputStream
{
public:
	using reflect::InputStream::size_type;

    StringInputStream(Fragment fragment)
        : mString(fragment)
    {}

	size_type Read(void *data, size_type size)
	{
		if(size > mString.size())
		{
			size = mString.size();
		}

		std::memcpy(data, mString.data(), size);
		mString = mString.substr(size);
		return size;
	}
	
	Fragment Text() const
	{
		return mString;
	}

private:
	Fragment mString;
};

} }

#endif
