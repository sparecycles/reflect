#ifndef REFLECT_CONFIG_REFLECTEXPORT_H_
#define REFLECT_CONFIG_REFLECTEXPORT_H_

#include <reflect/config/platform/win32.h>
#include <reflect/config/platform/gnuc.h>
#include <reflect/config/platform/generic.h>

// Macro: ReflectExport
//
// Use this macro in the declarations of any classes or functions that need
// correct DLL linkage.  
//
// Example:
// | class ReflectExport(my_module) MyClass { ...
// | ReflectExport(my_module) int MyFunction() { ...
// | template<> ReflectExport(my_module) int MyTemplateFunction<float>() { ...
//
// This macro expands to REFLECT_EXPORT_##module, which should be #defined to
// either REFLECT_EXPORT_ANNOTATION if building the module,
// or REFLECT_IMPORT_ANNOTATION, if using it.
//
// Example from reflect/config/config.h:
// | #ifdef REFLECT_BUILD_reflect
// | # define REFLECT_EXPORT_reflect REFLECT_EXPORT_ANNOTATION
// | #else
// | # define REFLECT_EXPORT_reflect REFLECT_IMPORT_ANNOTATION
// | #endif

#define ReflectExport(the_module__) REFLECT_EXPORT_##the_module__

#define REFLECT_EXPORT_local

#endif
