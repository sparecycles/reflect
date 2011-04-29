#ifndef REFLECT_CONFIG_H_
#define REFLECT_CONFIG_H_

#include <reflect/config/ReflectExport.h>

#ifdef REFLECT_BUILD_reflect
# define REFLECT_EXPORT_reflect REFLECT_EXPORT_ANNOTATION
#else
# define REFLECT_EXPORT_reflect REFLECT_IMPORT_ANNOTATION
#endif

#endif
