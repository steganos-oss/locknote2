// Steganos LockNote - self-modifying encrypted notepad
// Copyright (C) 2006-2023 Steganos GmbH
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include <process.h>

#include <atlfile.h>

#include "resource.h"

#include "locknoteView.h"
#include "aboutdlg.h"
#include "passworddlg.h"
#include "MainFrm.h"

#include "aeslayer.cpp" // prevents having to include precompiled header in aeslayer.cpp

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	TCHAR szModulePath[MAX_PATH] = {'\0'};
	::GetModuleFileName(Utils::GetModuleHandle(), szModulePath, MAX_PATH);

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	if (__argc == 3)
	{
#ifdef _UNICODE
		TCHAR* lpszCommand = __wargv[1];
		TCHAR* lpszPath = __wargv[2];
#else
		char* lpszCommand = __argv[1];
		char* lpszPath = __argv[2];
#endif
		if (!_tcscmp(lpszCommand, _T("-writeback")))
		{
			while (!::CopyFile(szModulePath, lpszPath, FALSE))
			{
				// wait and retry, maybe the process is still in use
				Sleep(100);
			}

			_tspawnl(_P_NOWAIT, lpszPath, utf8_to_wstring(Utils::Quote(wstring_to_utf8(lpszPath))).c_str(), _T("-erase"), utf8_to_wstring(Utils::Quote(wstring_to_utf8(szModulePath))).c_str(), NULL);

			return 0;
		}
		else if (!_tcscmp(lpszCommand, _T("-erase")))
		{
			while (PathFileExists(lpszPath) && !::DeleteFile(lpszPath))
			{
				// wait and retry, maybe the process is still in use
				Sleep(100);
			}

			return 0;
		}
	}

	if (__argc > 1)
	{
		int nResult = Utils::MessageBox(NULL, WSTR(IDS_ASK_CONVERT_FILES), MB_YESNO | MB_ICONQUESTION);
		if (nResult == IDYES)
		{
			int nConverted = 0;
			std::string encryptPassword;
			//std::list<std::string> decryptPasswords;
			for (int nIndex = 1; nIndex < __argc; nIndex++)
			{
				std::string filename = __argv[nIndex];
				UINT uLength = filename.size();
				std::string extension = filename.substr(uLength - 4, -1);
				std::string newfilename = filename.substr(0, uLength - 4);
				extension = str_tolower(extension);
				if (extension == ".txt")
				{
					newfilename += ".exe";
					std::string text;
					std::string password;
					if (LoadTextFromFile(filename, text, password))
					{
						if (SaveTextToFile(newfilename, text, encryptPassword))
						{
							nConverted += 1;
						}
					}
				}
				/*
				// convert SLIM back to text
				else if (extension == _T(".exe"))
				{
					_tcscpy(&newfilename[uLength-4], _T(".txt"));
				}
				*/
				else
				{					
					std::wstring text;
					text.resize(32768);
					swprintf_s((TCHAR*)text.c_str(), 32768, WSTR(IDS_CANT_CONVERT).c_str(), filename.c_str());
					Utils::MessageBox(NULL, text, MB_OK | MB_ICONERROR);
				}
			}
			if (nConverted)
			{
				std::wstring text;
				text.resize(32768);
				swprintf_s((TCHAR*)text.c_str(), 32768, WSTR(IDS_CONVERT_DONE).c_str(), nConverted);
				Utils::MessageBox(NULL, text, MB_OK | MB_ICONINFORMATION);
			}
		}

		return 0;
	}

	CHAR szFileMappingName[MAX_PATH];
	strcpy_s(szFileMappingName, MAX_PATH, wstring_to_utf8(szModulePath).c_str());
	CHAR* szChar = szFileMappingName;
	while (*szChar)
	{
		if ((*szChar == '\\')||(*szChar == ':'))
		{
			*szChar = '_';
		}
		szChar++;
	};

	CAtlFileMapping<HWND> fmSingleInstanceHWND;
	BOOL bAlreadyExisted = FALSE;
	fmSingleInstanceHWND.MapSharedMem(sizeof(HWND), utf8_to_wstring(szFileMappingName).c_str(), &bAlreadyExisted);
	if (bAlreadyExisted)
	{
		HWND hWndMain = *fmSingleInstanceHWND;
		::SetForegroundWindow(hWndMain);
		return 0;
	}

	std::string text;
	std::string data;
	std::string password;
	Utils::LoadResource("CONTENT", "PAYLOAD", data);
	if (!data.empty())
	{
		password = GetPasswordDlg();
		if (password.empty())
		{
			return -1;
		}
		if (!Utils::DecryptString(data, password, text))
		{
			MessageBox(NULL, WSTR(IDS_INVALID_PASSWORD), MB_OK | MB_ICONERROR);
			return -1;
		}		
	}
	else
	{
		text = STR(IDS_WELCOME);
	}

	CMainFrame wndMain;

	wndMain.m_password = password;
	wndMain.m_text = text;

	// get window sizes from resource
	std::string strSizeX;
	std::string strSizeY;
	Utils::LoadResource("SIZEX", "INFORMATION", strSizeX);
	Utils::LoadResource("SIZEY", "INFORMATION", strSizeY);
	if (!strSizeX.empty() && !strSizeY.empty())
	{
		wndMain.m_nWindowSizeX = atoi(strSizeX.c_str());
		wndMain.m_nWindowSizeY = atoi(strSizeY.c_str());
	}

	// get font size
	std::string fontsize;
	Utils::LoadResource("FONTSIZE", "INFORMATION", fontsize);
	if (!fontsize.empty())
	{
		// fontsize format: 10 (example)
		wndMain.m_nFontSize = atoi(fontsize.c_str());
	}
	else
	{
		wndMain.m_nFontSize = DEFAULT_FONT_SIZE;
	}

	// get font set
	std::string fontface;
	Utils::LoadResource("TYPEFACE", "INFORMATION", fontface);
	if (!fontface.empty())
	{
		// font typeface format: "Lucida Console" (example)
		wndMain.m_strFontName = fontface;
	}
	else
	{
		wndMain.m_strFontName = DEFAULT_FONT_NAME;
	}
	
	// get language id
	std::string language;
	Utils::LoadResource("LANGID", "INFORMATION", language);
	if (!language.empty())
	{
		// language id format: 409 (example)
		wndMain.SetLanguage(atoi(language.c_str()));
	}
	else
	{
		wndMain.SetLanguage(0);
	}

	// create window
	if (wndMain.CreateEx() == nullptr)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}
	wndMain.CheckFontSize();
	wndMain.CheckFontTypeFace();

	*fmSingleInstanceHWND = wndMain.m_hWnd;

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();

	if (((wndMain.m_text != text) || (wndMain.m_password != password) || wndMain.m_bTraitsChanged) && (wndMain.m_password.size()))
	{
		password = wndMain.m_password;

		data = "";
		if (!wndMain.m_text.empty())
		{
			Utils::EncryptString(wndMain.m_text, password, data);
		}
		else
		{
			MessageBox(NULL, WSTR(IDS_TEXT_IS_ENCRYPTED), MB_OK | MB_ICONINFORMATION);
		}

		CHAR szModulePath[MAX_PATH] = {'\0'};
		CHAR szTempPath[MAX_PATH] = {'\0'};
		CHAR szFileName[MAX_PATH] = {'\0'};

		::GetModuleFileNameA(Utils::GetModuleHandle(), szModulePath, MAX_PATH);
		::GetTempPathA(MAX_PATH, szTempPath);
		::GetTempFileNameA(szTempPath, "STG", 0, szFileName);

		::CopyFileA(szModulePath, szFileName, FALSE);
		Utils::UpdateResource(szFileName, "CONTENT", "PAYLOAD", data);

		// write window sizes to resource
		std::string sizeinfo;
		char szSizeInfo[MAX_PATH] = "";
		sprintf_s(szSizeInfo, MAX_PATH, "%d", wndMain.m_nWindowSizeX);
		sizeinfo = szSizeInfo;
		Utils::UpdateResource(szFileName, "SIZEX", "INFORMATION", sizeinfo);
		sprintf_s(szSizeInfo, MAX_PATH, "%d", wndMain.m_nWindowSizeY);
		sizeinfo = szSizeInfo;
		Utils::UpdateResource(szFileName, "SIZEY", "INFORMATION", sizeinfo);
	
		// write font size
		std::string fontsize;
		char szFontSize[MAX_PATH] = "";
		sprintf_s(szFontSize, MAX_PATH, "%d", wndMain.m_nFontSize);
		fontsize = szFontSize;
		Utils::UpdateResource(szFileName, "FONTSIZE", "INFORMATION", fontsize);

		// write font typeface
		Utils::UpdateResource(szFileName, "TYPEFACE", "INFORMATION", wndMain.m_strFontName);

		// write language code if changed
		int langid = wndMain.GetLanguage();
		if (langid != 0)
		{
			std::stringstream sslanguage;
			sslanguage << langid;
			Utils::UpdateResource(szFileName, "LANGID", "INFORMATION", sslanguage.str());
		}
		_tspawnl(_P_NOWAIT, utf8_to_wstring(szFileName).c_str(), utf8_to_wstring(Utils::Quote(szFileName)).c_str(), _T("-writeback"), utf8_to_wstring(Utils::Quote(szModulePath)).c_str(), NULL);
	}

	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
