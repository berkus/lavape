#ifndef _LavaMainFrameBase
#define __LavaMainFrameBase

#include "mdiframes.h"
#include "cmainframe.h"

#ifdef WIN32
#ifdef LAVAUI_EXPORT
#define LAVAUI_DLL __declspec( dllexport )
#else
#define LAVAUI_DLL __declspec( dllimport )
#endif
#else
#define LAVAUI_DLL
#endif


class LAVAUI_DLL CMainFrame : public wxMainFrame, public Ui_CMainFrame  {
public:
  CMainFrame(); 

private:
  Q_OBJECT
};

#endif