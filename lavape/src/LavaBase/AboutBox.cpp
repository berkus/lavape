/* LavaPE -- Lava Programming Environment
   Copyright (C) 2002 Fraunhofer-Gesellschaft
	 (http://www.sit.fraunhofer.de/english/)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#ifdef __GNUC__
#pragma implementation
#endif


//#include "stdafx.h"
#include "AboutBox.h"
#include "LavaBaseStringInit.h"
#include "LavaAppBase.h"
#include "qobject.h"
#include "qstring.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "qapplication.h"
#include "qprocess.h"

#ifdef WIN32
#include <process.h>
#endif
#include <errno.h>


/////////////////////////////////////////////////////////////////////////////
// CAboutBox dialog

CAboutBox::CAboutBox(QWidget* parent, const char* name, bool modal) :
	  IDD_AboutBox(parent,name,modal) {
  lineEdit1->setText(LBaseData->m_myWebBrowser);
} 

//CAboutBox::~CAboutBox() {}

void CAboutBox::OnSelectBrowser() {
	QString progDir, filter;

#ifdef WIN32
	progDir = "C:\\Programme";
	filter = "Executable Programs (*.exe)";
#else
	progDir = "/usr/bin";
	filter = "Executable Programs (*)";
#endif

  QString s = QFileDialog::getOpenFileName(
                  progDir,
                  filter,
                  this,
                  "open file dialog",
                  "Choose your favorite web browser" );
	if (s != QString::null) {
		lineEdit1->setText(s);
		LBaseData->m_myWebBrowser = s;
    wxTheApp->saveSettings();
	}
}

void CAboutBox::OnTextChanged (const QString &s) {
	LBaseData->m_myWebBrowser = s;
  wxTheApp->saveSettings();
}

void CAboutBox::OnLavaHomePage() 
{
	QStringList args;
	QString buf;

	if (lineEdit1->text().isEmpty()) {
    QMessageBox::critical(this,qApp->name(),ERR_MissingBrowserPath,QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
		return;
	}

	args << LBaseData->m_myWebBrowser << "http://lavape.sourceforge.net/";
	QProcess browser(args);

	if (!browser.launch(buf)) {
    QMessageBox::critical(this,qApp->name(),ERR_BrowserStartFailed.arg(errno),QMessageBox::Ok|QMessageBox::Default,QMessageBox::NoButton);
		return;
	}
}
