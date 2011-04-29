#ifndef REFLECT_CONFIG_PLATFORM_WIN32_H_
#define REFLECT_CONFIG_PLATFORM_WIN32_H_

#ifdef _WIN32

// 
# define REFLECT_EXPORT_ANNOTATION __declspec(dllexport)
# define REFLECT_IMPORT_ANNOTATION __declspec(dllimport)

// disable warnings about dll-interface requirements
//  (assume that we are using the same C-runtime across all reflect dlls)
# pragma warning (disable : 4251)

// disable warnings about assignement operators not being generated
# pragma warning (disable : 4512)

// disable warnings about constant conditional expressions
# pragma warning (disable : 4127)

// disable warnings about unsafe functions
# pragma warning (disable : 4996)

#endif

#endif
