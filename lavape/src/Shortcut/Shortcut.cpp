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


// Shortcut.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Shortcut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
   
/////////////////////////////////////////////////////////////////////////////
// The one and only application object

HRESULT CreateShortCut (LPCSTR pszShortcutFile, LPSTR pszLink);
//HRESULT ResolveShortCut (HWND hwnd, LPCSTR pszShortcutFile);

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
  HRESULT hres;

	// initialize MFC and print an error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{/*
		// TODO: code your application's behavior here.
		strHello.LoadString(IDS_HELLO);
		cout << (LPCTSTR)strHello << endl;*/
   if (argc < 3)
    {
      cout << "At least two file names required: target of link, link file name" << endl;
      cin.ignore();
      return 1;
    }

    hres = CoInitialize(0);
    CreateShortCut(argv[1],argv[2]);
//    ResolveShortCut(0,"C:\\guenthk\\test.lnk");
	}

	return nRetCode;
}

HRESULT CreateShortCut (LPCSTR pszShortcutFile, LPSTR pszLink)
{
  HRESULT hres;
  IShellLink *psl;

  // Create an IShellLink object and get a pointer to the IShellLink 
  // interface (returned from CoCreateInstance).
  hres = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
  IID_IShellLink, (void **)&psl);
  if (SUCCEEDED (hres))
  {
    IPersistFile *ppf;

    // Query IShellLink for the IPersistFile interface for 
    // saving the shortcut in persistent storage.
    hres = psl->QueryInterface (IID_IPersistFile, (void **)&ppf);
    if (SUCCEEDED (hres))
    { 
      WORD wsz [MAX_PATH]; // buffer for Unicode string

      // Set the path to the shortcut target.
      hres = psl->SetPath (pszShortcutFile);

      if (! SUCCEEDED (hres))
        AfxMessageBox ("SetPath failed!");

      // Set the description of the shortcut.
      hres = psl->SetDescription ("");

      if (! SUCCEEDED (hres))
        AfxMessageBox ("SetDescription failed!");

      // Ensure that the string consists of ANSI characters.
      MultiByteToWideChar (CP_ACP, 0, pszLink, -1, wsz, MAX_PATH);

      // Save the shortcut via the IPersistFile::Save member function.
      hres = ppf->Save (wsz, TRUE);

      if (! SUCCEEDED (hres))
        AfxMessageBox ("Save failed!");

      // Release the pointer to IPersistFile.
      ppf->Release ();
    }
    // Release the pointer to IShellLink.
    psl->Release ();
  }
  return hres;
} 

/*
HRESULT ResolveShortCut (HWND hwnd, LPCSTR pszShortcutFile)
{
  HRESULT hres;
  IShellLink *psl;
  char szGotPath [MAX_PATH];
  char szDescription [MAX_PATH];
  WIN32_FIND_DATA wfd;

  // Get a pointer to the IShellLink interface.
  hres = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
  IID_IShellLink, (void **)&psl);
  if (SUCCEEDED (hres))
  {
    IPersistFile *ppf;

    // Get a pointer to the IPersistFile interface.
    hres = psl->QueryInterface (IID_IPersistFile, (void **)&ppf);

    if (SUCCEEDED (hres))
    {
      WORD wsz [MAX_PATH]; // buffer for Unicode string

      // Ensure that the string consists of Unicode characters.
      MultiByteToWideChar (CP_ACP, 0, pszShortcutFile, -1, wsz,
      MAX_PATH);

      // Load the shortcut.
      hres = ppf->Load (wsz, STGM_READ);

      if (SUCCEEDED (hres))
      {
        // Resolve the shortcut.
        hres = psl->Resolve (hwnd, SLR_ANY_MATCH);
        if (SUCCEEDED (hres))
        {
          strcpy (szGotPath, pszShortcutFile);
          // Get the path to the shortcut target.
          hres = psl->GetPath (szGotPath, MAX_PATH,
          (WIN32_FIND_DATA *)&wfd, SLGP_SHORTPATH);
          if (! SUCCEEDED (hres))
            AfxMessageBox ("GetPath failed!");
          else
            AfxMessageBox (szGotPath);
          // Get the description of the target.
          hres = psl->GetDescription (szDescription, MAX_PATH);
          if (! SUCCEEDED (hres))
            AfxMessageBox ("GetDescription failed!");
          else
            AfxMessageBox (szDescription);
        }
      }
      // Release the pointer to IPersistFile.
      ppf->Release ();
      }
      // Release the pointer to IShellLink.
      psl->Release ();
    }
  return hres;
}
*/