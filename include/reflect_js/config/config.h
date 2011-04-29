#ifndef REFLECTJS_CONFIG_CONFIG_H_
#define REFLECTJS_CONFIG_CONFIG_H_

#include <reflect/config/ReflectExport.h>

#ifdef REFLECT_BUILD_reflect_js
# define REFLECT_EXPORT_reflect_js REFLECT_EXPORT_ANNOTATION
#else
# define REFLECT_EXPORT_reflect_js REFLECT_IMPORT_ANNOTATION
#endif

#endif

