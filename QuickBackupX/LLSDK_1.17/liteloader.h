#pragma once
#ifdef LITELOADER_EXPORTS
#    define LIAPI __declspec(dllexport)
#else
#    define LIAPI __declspec(dllimport)
#endif
typedef unsigned char uchar;
#define LiteLoaderVersion "1.0.3"
#define LiteLoaderVersionNum (unsigned short)9
