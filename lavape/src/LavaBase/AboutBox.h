#ifndef __CAboutBox_H
#define __CAboutBox_H

#include "idd_aboutbox.h"
#include "SynIDTable.h"
#include "LavaAppBase.h"
#include "qlineedit.h"


class LAVABASE_DLL CAboutBox : public QDialog, public Ui::IDD_AboutBox
{
	Q_OBJECT

public:
  CAboutBox(QWidget* parent = 0, const char* name = 0, bool modal = true);
//  ~CAboutBox();

public slots:
	void OnLavaHomePage();
	void OnSelectBrowser();
	void OnTextChanged(const QString &);
  void OnClose()  {QDialog::reject();}
};


#endif
