#ifndef _TStatic
#define _TStatic


/////////////////////////////////////////////////////////////////////////////
// CTStatic window

#include "GUIProgBase.h"
#include "qlabel.h"

class CTStatic : public QLabel
{
public:
  CTStatic(CGUIProgBase *guiPr, CHEFormNode* data, QWidget* parentWidget, const QString& label, const char* widgetName);
  virtual ~CTStatic();

  //bool CreateLabelWidget(QWidget* parentWidget, QSize size, LPCTSTR label, LPCTSTR widgetName);
  CHEFormNode* myFormNode;
  CGUIProgBase *GUIProg;
private:
  Q_OBJECT

};
#endif

