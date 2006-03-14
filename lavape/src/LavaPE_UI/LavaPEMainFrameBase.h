#ifndef _LavaPEMainFrameBase
#define _LavaPEMainFrameBase

#include "cmainframe.h"

#ifdef WIN32
#ifdef LAVAPEUI_EXPORT
#define LAVAPEUI_DLL __declspec( dllexport )
#else
#define LAVAPEUI_DLL __declspec( dllimport )
#endif
#else
#define LAVAPEUI_DLL
#endif


class LAVAPEUI_DLL CMainFrame : public QMainWindow, public Ui_CMainFrame  {
public:
  CMainFrame(); 

private:
  Q_OBJECT
};

#endif

