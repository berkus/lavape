#ifndef __LAVAUI
#define __LAVAUI


#ifdef WIN32
#ifdef LAVAUI_EXPORT
#define LAVAUI_DLL __declspec( dllexport )
#else
#define LAVAUI_DLL __declspec( dllimport )
#endif
#else
#define LAVAUI_DLL
#endif

#endif
