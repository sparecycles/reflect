#ifndef REFLECT_REFLECTOR_H_
#define REFLECT_REFLECTOR_H_

#include <reflect/config/config.h>
#include <reflect/Serializer.h>
#include <reflect/Deserializer.h>
#include <reflect/autocast.h>
#include <reflect/Class.h>
#include <reflect/Variant.h>

namespace reflect {

class Serializer;
class Deserializer;


// Class: Reflector
//
// Reflectors provides an easy and uniform interface for using <Serializers>, and <Deserializers>,
// and maintain a bool for error detection/handling.
//
// Reflectors do the dirty job of invoking the template Serialize<T>(...) functions.
class ReflectExport(reflect) Reflector
{
public:
	// Constructor: Reflector
	// 
	// Builds a serializing reflector
    Reflector(Serializer &);
	
	// Constructor: Reflector
	// 
	// Builds a deserializing reflector
    Reflector(Deserializer &);

    Reflector &operator |(Variant &data);

	// Function: operator |
	// Serializes or Deserializes data.
    template<typename T> Reflector &operator |(T &data);
	
	// Function: operator |
	// Serializes the data.
    template<typename T> Reflector &operator |(const T &data);
	
	// Function: operator |
	// Serializes or Deserializes a pointer to a subclass of Dynamic.
    template<typename T> Reflector &operator |(T *&data);
    
	// Function: operator |
	// Serializes a pointer to a subclass of Dynamic.
    template<typename T> Reflector &operator |(T *const &data);

	// Function: Serializing
	// 
	// Return:
	//   true when this reflector represents a Serializer
    bool Serializing() const;
    
	// Function: Deserializing
	// 
	// Return:
	//   true when this reflector represents a Deserializer
    bool Deserializing() const;

	// Function: operator Serializer
	// returns the serializer represented by this Reflector
    operator Serializer &() const { return GetSerializer(); }
	// Function: operator Deserializer
	// returns the deserializer represented by this Reflector
	operator Deserializer &() const { return GetDeserializer(); }

	// Function: GetSerializer
	// returns the serializer represented by this Reflector
    Serializer &GetSerializer() const;

	// Function: GetDeserializer
	// returns the deserializer represented by this Reflector
    Deserializer &GetDeserializer() const;

	// Function: Fail
	// Flag an error condition on this Reflector.
    void Fail();

	// Function: Check
	// Flag an error condition on this Reflector if result is false.
    Reflector &Check(bool result);
    
	// Function: Ok
	// 
	// Return:
	//   true if an error has not been flagged on this reflector.
	bool Ok() const;

	// Function: operator bool
	// convenient alias for <Ok>
	operator bool() const { return Ok(); }

private:
    template<typename T>
    class BoundReflector;


public:
	// Function: operator(const T *, T *)
	//
	// Returns:
	//   a Reflector bound to an object which can
	//   serialize pointer-to-members bidirectionally.
    template<typename T>
    BoundReflector<T> operator()(const T *in, T *out)
    {
        return BoundReflector<T>(*this, in, out);
    }

private:
	// Function: OpaqueSerialize
	//
	// Helper function for invoking <Class::Serialize> which takes
	// void * arguments which need to be normalized with <opaque_cast>
	// so the implementation can cast back safely.
	//
	// Parameters:
	//    in - const T * to be serialized.
	//    out - T * to be deserialized (may be null if serializing).
	//    reflector - wraps a serializer or deserializer.
	template<typename T>
	static inline void OpaqueSerialize(const T *in, T *out, Reflector &reflector)
	{
		TypeOf<T>()->Serialize(
			opaque_cast(in),
			opaque_cast(out),
			reflector);
	}

	// Member: mSerializer
	//
	// The serializer this Reflector is managing,
	// or NULL if it is Deserializing.
	Serializer *mSerializer;

	// Member: mDeserializer
	//
	// The deserializer this Reflector is managing,
	// or NULL if it is Serializing.
	Deserializer *mDeserializer;

	// Member: mOk
	//
	// A bool indicating success, if this becomes false then there was a problem
	// with serialization/deserialization.
    bool mOK;

    // "Don't copy me" boilerplate:
    Reflector(const Reflector &);
    const Reflector &operator =(const Reflector &);
};

template<typename T>
inline
Reflector &Reflector::operator |(const T &data)
{
	OpaqueSerialize(&data, static_cast<T *>(0), *this);
    return *this;
}

template<typename T>
inline
Reflector &Reflector::operator |(T &data)
{
	OpaqueSerialize(&data, &data, *this);
    return *this;
}

template<typename T>
inline
Reflector &Reflector::operator |(T *&data)
{
	Dynamic *castable = data;
	OpaqueSerialize(&castable, &castable, *this);
	data = castable % autocast;
    if(castable && !data) Fail();
    return *this;
}

template<typename T>
inline
Reflector &Reflector::operator |(T *const &data)
{
	Dynamic *castable = data;
	TypeOf<Dynamic *>()->Serialize(&castable, 0, *this); 
    return *this;
}

template<typename T>
inline
bool operator |(Deserializer &deserializer, T &thing)
{
	return Reflector(deserializer) | thing;
}

template<typename T>
inline
bool operator |(Serializer &serializer, const T &thing)
{
	return Reflector(serializer) | thing;
}

// Class: Reflector::BoundReflector
//
// A utitlity class which makes it easier to sequence
// reads and writes of multiple member variables.
template<typename T>
class Reflector::BoundReflector 
{
public:
    BoundReflector(Reflector &reflector, const T *in, T *out)
        : mReflector(reflector)
        , mIn(in)
        , mOut(out)
    {}
    
    template<typename M>
    const BoundReflector &operator |(M T::*pm) const
    { 
        if(mReflector.Serializing()) mReflector | mIn->*pm;
        else mReflector | mOut->*pm;

        return *this; 
    }

    template<typename M, unsigned size>
    const BoundReflector &operator |(M (T::*pm)[size]) const
    { 
        if(mReflector.Serializing())
		{
			for(unsigned index = 0; index < size; index++)
				mReflector | (mIn->*pm)[index];
		}
		else 
		{
			for(unsigned index = 0; index < size; index++)
				mReflector | (mOut->*pm)[index];
		}
        return *this; 
    }

    template<typename M, unsigned size0, unsigned size1>
    const BoundReflector &operator |(M (T::*pm)[size0][size1]) const
    { 
        if(mReflector.Serializing())
		{
			for(unsigned index0 = 0; index0 < size0; index0++)
			for(unsigned index1 = 0; index1 < size1; index1++)
				mReflector | (mIn->*pm)[index0][index1];
		}
		else 
		{
			for(unsigned index0 = 0; index0 < size0; index0++)
			for(unsigned index1 = 0; index1 < size1; index1++)
				mReflector | (mOut->*pm)[index0][index1];
		}
        return *this; 
    }

    template<typename M, unsigned size0, unsigned size1, unsigned size2>
    const BoundReflector &operator |(M (T::*pm)[size0][size1][size2]) const
    { 
        if(mReflector.Serializing())
		{
			for(unsigned index0 = 0; index0 < size0; index0++)
			for(unsigned index1 = 0; index1 < size1; index1++)
			for(unsigned index2 = 0; index2 < size2; index2++)
				mReflector | (mIn->*pm)[index0][index1][index2];
		}
		else 
		{
			for(unsigned index0 = 0; index0 < size0; index0++)
			for(unsigned index1 = 0; index1 < size1; index1++)
			for(unsigned index2 = 0; index2 < size2; index2++)
				mReflector | (mOut->*pm)[index0][index1][index2];
		}
        return *this; 
    }
private:
    Reflector &mReflector;
    const T *mIn;
    T *mOut;
};

}

#endif
