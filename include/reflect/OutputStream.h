#ifndef REFLECT_OUTPUTSTREAM_H_
#define REFLECT_OUTPUTSTREAM_H_

#include <reflect/config/config.h>

namespace reflect {

// Class: OutputStream
//
// This class defines the interface most <Serializers> use to write data.
class ReflectExport(reflect) OutputStream
{
public:
	// Typedef: size_type
    typedef unsigned size_type;
	
	// Function: Write
	//    Writes binary data out.
	// 
	// Returns:
	//    The size of data actually written.
    virtual size_type Write(const void *buffer, size_type bufmax) = 0;
protected:
	virtual ~OutputStream();
};

}

#endif
