#ifndef REFLECT_CONFIG_PLATFORM_GNUC_H_
#define REFLECT_CONFIG_PLATFORM_GNUC_H_

// visibility attribute in gcc3.3 and above.
// 
// define NO_VISIBILITY_ATTRIBUTE to disable autodetection
// define HAVE_VISIBILITY_ATTRIBUTE to force enable use on
// non-gcc (or patched older gcc) platforms.
#if !defined(NO_VISIBILITY_ATTRIBUTE)
# if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)
#  define HAVE_VISIBILITY_ATTRIBUTE
# endif
#endif

#ifdef HAVE_VISIBILITY_ATTRIBUTE

#ifndef REFLECT_EXPORT_ANNOTATION
# define REFLECT_EXPORT_ANNOTATION __attribute__ ((visibility("default")))
#endif

#ifndef REFLECT_IMPORT_ANNOTATION
# define REFLECT_IMPORT_ANNOTATION  __attribute__ ((visibility("default")))
#endif

#endif

#endif
