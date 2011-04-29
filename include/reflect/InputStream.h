#ifndef REFLECT_INPUTSTREAM_H_
#define REFLECT_INPUTSTREAM_H_

#include <reflect/config/config.h>

namespace reflect {

// Class: InputStream
//
// This class defines the interface most
// <Deserializers> use to read data.
class ReflectExport(reflect) InputStream
{
public:
    // Typedef: size_type
    typedef unsigned size_type;
    
  	// Function: Read
	//    Reads binary data.
	// 
	// Returns:
	//    The size of data actually read.
    virtual size_type Read(void *buffer, size_type bufmax) = 0;

protected:
	virtual ~InputStream();
};

}

#endif
