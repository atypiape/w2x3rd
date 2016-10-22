#ifndef XZIP_CONF_H
#define XZIP_CONF_H

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) \
	|| defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#  define XZIP_WINDOWS
#elif defined(__ANDROID__)
#  define XZIP_ANDROID
#endif 

/* If building or using xzip as a DLL, define XZIP_DLL.
* This is not mandatory, but it offers a little performance increase.
*/
#ifdef XZIP_DLL
#  ifdef XZIP_INTERNAL
#    define XEXTERN extern __declspec(dllexport)
#  else
#    define XEXTERN extern __declspec(dllimport)
#  endif
#endif  /* XZIP_DLL */

#ifndef XEXTERN
#  define XEXTERN extern
#endif

#endif /* XZIP_CONF_H */