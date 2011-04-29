// File: SaveLoad.h

#ifndef REFLECT_UTILITY_SAVELOAD_H_
#define REFLECT_UTILITY_SAVELOAD_H_

#include <reflect/serialize/StandardSerializer.h>
#include <reflect/serialize/StandardDeserializer.h>
#include <reflect/Reflector.h>
#include <reflect/InputStream.h>
#include <reflect/OutputStream.h>
#include <reflect/string/String.h>

#include <cstdio>

namespace reflect { namespace utility {

// Class: FileOutputStream
// A FILE * based <OutputStream>
class FileOutputStream : public OutputStream
{
public:
	using OutputStream::size_type;

	// Constructor: FileOutputStream
	FileOutputStream(std::FILE *file)
		: mFile(file)
	{}

	/*virtual*/ size_type Write(const void *buffer, size_type bufmax)
	{
		return fwrite(buffer, 1, bufmax, mFile);
	}

	std::FILE *mFile;
};


// Class: FileInputStream
// A FILE * based <InputStream>
class FileInputStream : public InputStream
{
public:
	using InputStream::size_type;

	// Constructor: FileInputStream
	FileInputStream(std::FILE *file)
		: mFile(file)
	{}

	/*virtual*/ size_type Read(void *buffer, size_type bufmax)
	{
		return fread(buffer, 1, bufmax, mFile);
	}

	std::FILE *mFile;
};

// Section: Save/Load Functions

// Function: SaveFile
//
// Saves "data" into a file using a StandardSerializer.
//
// Parameters:
//    data - any kind of reflectable data.
//    filename - the name of the file to save to.
//
// Returns:
//   true - when serialization succeeded.
template<typename Type>
bool SaveFile(const Type &data, string::ConstString filename)
{
	if(std::FILE *file = std::fopen(filename.c_str(), "wb"))
	{
		FileOutputStream output(file);
		serialize::StandardSerializer serializer(output);
		Reflector reflector(serializer);

		reflector | data;

		fclose(file);

		return reflector.Ok();
	}
	else
	{
		return false;
	}
}

// Function: LoadFile
//
// Saves "data" into a file using a StandardDeserializer.
//
// Parameters:
//    data - any kind of reflectable data.
//    filename - the name of the file to load from.
//
// Returns:
//    true - when deserialization succeeded.
template<typename Type>
bool LoadFile(Type &data, string::ConstString filename)
{
	if(std::FILE *file = std::fopen(filename.c_str(), "rb"))
	{
		FileInputStream output(file);
		serialize::StandardDeserializer deserializer(output);
		Reflector reflector(deserializer);

		reflector | data;

		fclose(file);

		return reflector.Ok();
	}
	else
	{
		return false;
	}
}

} }


#endif