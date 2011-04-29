// File: PrimitiveTypes.h
//
// Declares the reflection for the basic primitive types of C++.
//
// Definitions for the following types are defined here:
//   - bool
//   - char
//   - signed char
//   - unsigned char
//   - short
//   - unsigned short
//   - int
//   - unsigned int
//   - long
//   - unsigned long
//   - float
//   - double

#ifndef REFLECT_BASICTYPESERIALIZATION_H_
#define REFLECT_BASICTYPESERIALIZATION_H_

#include <reflect/Reflection.h>
#include <reflect/PrimitiveType.h>
#include <reflect/config/config.h>

DECLARE_PRIMITIVE_REFLECTION(reflect, void)

DECLARE_PRIMITIVE_REFLECTION(reflect, bool)
DECLARE_PRIMITIVE_REFLECTION(reflect, char)
DECLARE_PRIMITIVE_REFLECTION(reflect, signed char)
DECLARE_PRIMITIVE_REFLECTION(reflect, unsigned char)
DECLARE_PRIMITIVE_REFLECTION(reflect, signed short)
DECLARE_PRIMITIVE_REFLECTION(reflect, unsigned short)
DECLARE_PRIMITIVE_REFLECTION(reflect, signed int)
DECLARE_PRIMITIVE_REFLECTION(reflect, unsigned int)
DECLARE_PRIMITIVE_REFLECTION(reflect, signed long)
DECLARE_PRIMITIVE_REFLECTION(reflect, unsigned long)
DECLARE_PRIMITIVE_REFLECTION(reflect, float)
DECLARE_PRIMITIVE_REFLECTION(reflect, double)

DECLARE_PRIMITIVE_REFLECTION(reflect, const char *)
DECLARE_REFLECTION_ALIAS(char *, const char *)

#endif
