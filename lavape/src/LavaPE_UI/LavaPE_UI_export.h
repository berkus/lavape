#ifndef __LAVAPEUI
#define __LAVAPEUI


#ifdef WIN32
#ifdef LAVAPEUI_EXPORT
#define LAVAPEUI_DLL __declspec( dllexport )
#else
#define LAVAPEUI_DLL __declspec( dllimport )
#endif
#else
#define LAVAPEUI_DLL
#endif

#endif
