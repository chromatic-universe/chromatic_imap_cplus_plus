#ifndef __DEF_H__
#define __DEF_H__

#ifdef COMMONDLL
    #define EXPORT_COMMON  __declspec(dllexport)
#else
    #define EXPORT_COMMON  __declspec(dllimport)
#endif

#endif
