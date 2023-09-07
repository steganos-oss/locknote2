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

#pragma once

#include <string>
#include <sstream>
#include <list>
#include <iostream>
#include <algorithm>
#include <atldlgs.h>
//#include <windows.h>
#include "utils.h"

constexpr char* DEFAULT_FONT_NAME = NAME_FONT_CONSOLAS;
constexpr unsigned int DEFAULT_FONT_SIZE = 12;
constexpr unsigned int DEFAULT_WIDTH = 650;
constexpr unsigned int DEFAULT_HEIGHT = 500;

UINT UWM_FINDMSGSTRING = CFindReplaceDialog::GetFindReplaceMsg();

class CMainFrame 
	: public CFrameWindowImpl<CMainFrame>, public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	struct UndoBuffer
	{
		int m_nStartChar;
		int m_nEndChar;
		std::string m_strText;
	};

	std::list<UndoBuffer> m_listUndo;

	CLockNoteView m_view;
	CFont m_fontEdit;
	std::string m_text;
	UndoBuffer m_currentBuffer;
	std::string m_password;

	DWORD m_dwSearchFlags = FR_DOWN;
	std::string m_strSearchString;
	CFindReplaceDialog* m_pCurrentFindReplaceDialog{ nullptr };

	int m_nWindowSizeX{ DEFAULT_WIDTH };
	int m_nWindowSizeY{ DEFAULT_HEIGHT };
	int m_nFontSize{ DEFAULT_FONT_SIZE };
	int m_nLanguage{ 0 };
	std::map<WORD, std::wstring> m_languages{ {LANG_ENGLISH, L"en_US"}, { LANG_GERMAN, L"de_DE" }, { LANG_FRENCH, L"fr_FR" }, { LANG_SPANISH, L"es_ES" }, { LANG_PORTUGUESE, L"pt_PT" }, { LANG_ITALIAN, L"it_IT" }, { LANG_DUTCH, L"nl_NL" }, { LANG_SWEDISH, L"sv_SE" }, { LANG_POLISH, L"pl_PL" }, { LANG_RUSSIAN, L"ru_RU" } };

	std::string m_strFontName{ DEFAULT_FONT_NAME };
	bool m_bTraitsChanged{ false };

	CMainFrame()
	{
	}

	virtual ~CMainFrame() = default;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (m_pCurrentFindReplaceDialog)
		{
			if (m_pCurrentFindReplaceDialog->IsDialogMessage(pMsg))
			{
				return TRUE;
			}
		}

		if (/*pMsg->message == WM_KEYDOWN || */pMsg->message == WM_KEYUP)
		{
			if (pMsg->wParam >= VK_PRIOR && pMsg->wParam <= VK_DOWN)
			{
				UpdateStatusBar();
			}
		}
		else if (/*pMsg->message == WM_LBUTTONDOWN || */pMsg->message == WM_LBUTTONUP/* || pMsg->message == WM_LBUTTONDBLCLK*/)
		{
			UpdateStatusBar();
		}
		else if (pMsg->message == WM_LBUTTONDBLCLK)
		{
			UpdateStatusBar();
		}

		if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		{
			return TRUE;
		}

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
		MESSAGE_HANDLER(UWM_FINDMSGSTRING, OnFindMsgString)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_CHANGEPASSWORD, OnFileChangePassword)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditSelectAll)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
		COMMAND_ID_HANDLER(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER(ID_EDIT_FINDNEXT, OnEditFindNext)
		COMMAND_ID_HANDLER(ID_VIEW_FONTSIZE_9, OnViewFontSize)
		COMMAND_ID_HANDLER(ID_VIEW_FONTSIZE_10, OnViewFontSize)
		COMMAND_ID_HANDLER(ID_VIEW_FONTSIZE_12, OnViewFontSize)
		COMMAND_ID_HANDLER(ID_VIEW_FONTSIZE_14, OnViewFontSize)
		COMMAND_ID_HANDLER(ID_FONT_ARIAL, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_COURIER_NEW, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_LUCIDA_CONSOLE, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_TAHOMA, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_VERDANA, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_CONSOLAS, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_FONT_CASCADIA_CODE, OnViewFontTypeFace)
		COMMAND_ID_HANDLER(ID_CHANGE_LANGUAGE, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_ENGLISH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_GERMAN, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_FRENCH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_SPANISH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_ITALIAN, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_PORTUGUESE, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_DUTCH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_SWEDISH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_POLISH, OnChangeLanguage)
		COMMAND_ID_HANDLER(LANG_RUSSIAN, OnChangeLanguage)
		COMMAND_ID_HANDLER(ID_STEGANOS_PASSWORD_MANAGER, OnVisitSteganos)
		COMMAND_ID_HANDLER(ID_STEGANOS_SAFE, OnVisitSteganos)
		COMMAND_ID_HANDLER(ID_STEGANOS_PRIVACYSUITE, OnVisitSteganos)
		COMMAND_ID_HANDLER(ID_STEGANOS_ONLINESHIELD, OnVisitSteganos)
		COMMAND_ID_HANDLER(ID_STEGANOS_LATESTOFFERS, OnVisitSteganos)
		COMMAND_ID_HANDLER(ID_STEGANOS_CHECKFORLOCKNOTEUPDATES, OnVisitSteganos)
		
		COMMAND_CODE_HANDLER(EN_CHANGE, OnChange)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	std::string GetText()
	{
		std::string text;
		int nLength = m_view.GetWindowTextLength();
		text.resize(nLength);
		WCHAR* wszBuffer = new WCHAR[nLength + 1];
		wszBuffer[nLength] = L'\0';
		::GetWindowText(m_view, wszBuffer, nLength + 1);
		text = wchar_to_utf8(wszBuffer);
		memset(wszBuffer, 0, nLength + 1);
		delete[]wszBuffer;
		wszBuffer = nullptr;
		return text;
	}

	bool SaveTextToFile(const std::string& path, const std::string& text, std::string& password, HWND hWnd = 0)
	{
		LOCKNOTEWINTRAITS wintraits;
		wintraits.m_nFontSize = m_nFontSize;
		wintraits.m_nWindowSizeX = m_nWindowSizeX;
		wintraits.m_nWindowSizeY = m_nWindowSizeY;
		wintraits.m_nLangId = m_nLanguage;
		wintraits.m_strFontName = m_strFontName;
		return Utils::SaveTextToFile(path, text, password, *this, &wintraits);
	}

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void UpdateStatusBar()
	{
		auto num_lines = SendMessage(m_hWndClient, EM_GETLINECOUNT, 0, 0);
		auto current_line_number = SendMessage(m_hWndClient, EM_LINEFROMCHAR, -1, 0);
		auto index = SendMessage(m_hWndClient, EM_LINEINDEX, -1, 0);
		DWORD dwStart = 0, dwEnd = 0;
		auto column = index;
		SendMessage(m_hWndClient, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
		if (dwStart > 0 && static_cast<unsigned int>(index) <= dwStart)
		{
			column = dwStart - index;
		}

		current_line_number += 1;
		column += 1;
		if (column < 1)
		{
			column = 1;
		}

		// old but compatible...
		std::string text;
		text.resize(32768);
		sprintf_s((char*)text.c_str(), text.size(), STR(IDS_STATUSBAR_STATS).c_str(), num_lines, GetText().size(), current_line_number, column);
		SendMessage(m_hWndStatusBar, WM_SETTEXT, text.size() + 1, (LPARAM)(utf8_to_wstring(text).c_str()));

		/*std::stringstream ss;
		ss << num_lines << _T(" lines, ") << GetText().size() << _T(" characters. Line: ");
		ss << current_line_number << _T(", column: ") << column << _T(".");
		SendMessage(m_hWndStatusBar, WM_SETTEXT, ss.str().size() + 1, (LPARAM)(ss.str().c_str()));*/
	}

	void CheckFontSize()
	{
		// check the correct menu item
		HMENU hMenu = this->GetMenu();
		CMenuHandle menu(hMenu);
		menu.CheckMenuItem(ID_VIEW_FONTSIZE_9, MF_UNCHECKED);
		menu.CheckMenuItem(ID_VIEW_FONTSIZE_10, MF_UNCHECKED);
		menu.CheckMenuItem(ID_VIEW_FONTSIZE_12, MF_UNCHECKED);
		menu.CheckMenuItem(ID_VIEW_FONTSIZE_14, MF_UNCHECKED);
		switch (m_nFontSize)
		{
		case 9:
			menu.CheckMenuItem(ID_VIEW_FONTSIZE_9, MF_CHECKED);
			break;
		case 12:
			menu.CheckMenuItem(ID_VIEW_FONTSIZE_12, MF_CHECKED);
			break;
		case 14:
			menu.CheckMenuItem(ID_VIEW_FONTSIZE_14, MF_CHECKED);
			break;
		case 10:
		default:
			menu.CheckMenuItem(ID_VIEW_FONTSIZE_10, MF_CHECKED);
		};
	}

	LRESULT OnViewFontSize(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// get selected font size and check the correct menuitem
		int nOldFontSize = m_nFontSize;

		switch (wID)
		{
		case ID_VIEW_FONTSIZE_9:
			m_nFontSize = 9;
			break;
		case ID_VIEW_FONTSIZE_12:
			m_nFontSize = 12;
			break;
		case ID_VIEW_FONTSIZE_14:
			m_nFontSize = 14;
			break;
		case ID_VIEW_FONTSIZE_10:
		default:
			m_nFontSize = 10;
		}
		CheckFontSize();

		CClientDC dc(*this);
		m_fontEdit.DeleteObject();
		m_fontEdit.CreateFont(
			-MulDiv(m_nFontSize, GetDeviceCaps(dc, LOGPIXELSY), 72),
			0,
			0,
			0,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			utf8_to_wstring(m_strFontName).c_str());

		ATLASSERT(m_fontEdit);
		m_view.SetFont(m_fontEdit);

		// set left and right margins
		auto dpi = GetDeviceCaps(dc, LOGPIXELSY);
		auto dpi_factor_font = MulDiv(m_nFontSize, dpi, 72);
		auto margin = MulDiv(20, dpi, 96);
		m_view.SetMargins(margin, margin);

		// reset text to force realign to margins
		m_text = GetText();
		if (m_text.length())
		{
			m_view.SetWindowText(utf8_to_wstring(m_text).c_str());
		}

		if (m_nFontSize != nOldFontSize)
		{
			m_bTraitsChanged = true;
		}
		return 0;
	}

	void CheckFontTypeFace()
	{
		// check the correct menu item
		HMENU hMenu = this->GetMenu();
		CMenuHandle menu(hMenu);
		menu.CheckMenuItem(ID_FONT_ARIAL, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_COURIER_NEW, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_LUCIDA_CONSOLE, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_TAHOMA, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_VERDANA, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_CONSOLAS, MF_UNCHECKED);
		menu.CheckMenuItem(ID_FONT_CASCADIA_CODE, MF_UNCHECKED);
		if (m_strFontName == "Arial")
		{
			menu.CheckMenuItem(ID_FONT_ARIAL, MF_CHECKED);
		}
		else if (m_strFontName == "Courier New")
		{
			menu.CheckMenuItem(ID_FONT_COURIER_NEW, MF_CHECKED);
		}
		else if (m_strFontName == "Tahoma")
		{
			menu.CheckMenuItem(ID_FONT_TAHOMA, MF_CHECKED);
		}
		else if (m_strFontName == "Verdana")
		{
			menu.CheckMenuItem(ID_FONT_VERDANA, MF_CHECKED);
		}
		else if (m_strFontName == "Lucida Console")
		{
			menu.CheckMenuItem(ID_FONT_LUCIDA_CONSOLE, MF_CHECKED);
		}
		else if (m_strFontName == "Consolas")
		{
			menu.CheckMenuItem(ID_FONT_CONSOLAS, MF_CHECKED);
		}
		else if (m_strFontName == "Cascadia Code")
		{
			menu.CheckMenuItem(ID_FONT_CASCADIA_CODE, MF_CHECKED);
		}
		else
		{
			menu.CheckMenuItem(ID_FONT_LUCIDA_CONSOLE, MF_CHECKED);
		}
	}

	LRESULT OnViewFontTypeFace(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// get selected font name and check the correct menu item
		std::string strOldFontName = m_strFontName;
		switch (wID)
		{
		case ID_FONT_ARIAL:
			m_strFontName = NAME_FONT_ARIAL;
			break;
		case ID_FONT_COURIER_NEW:
			m_strFontName = NAME_FONT_COURIER_NEW;
			break;
		case ID_FONT_TAHOMA:
			m_strFontName = NAME_FONT_TAHOMA;
			break;
		case ID_FONT_VERDANA:
			m_strFontName = NAME_FONT_VERDANA;
			break;
		case ID_FONT_LUCIDA_CONSOLE:
			m_strFontName = NAME_FONT_LUCIDA_CONSOLE;
			break;
		case ID_FONT_CASCADIA_CODE:
			m_strFontName = NAME_FONT_CASCADIA_CODE;
			break;
		case ID_FONT_CONSOLAS:
		default:
			wID = ID_FONT_CONSOLAS;
			m_strFontName = NAME_FONT_CONSOLAS;
		}
		CheckFontTypeFace();

		CClientDC dc(*this);
		m_fontEdit.DeleteObject();
		m_fontEdit.CreateFont(
			-MulDiv(m_nFontSize, GetDeviceCaps(dc, LOGPIXELSY), 72),
			0,
			0,
			0,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			utf8_to_wstring(m_strFontName).c_str());

		ATLASSERT(m_fontEdit);
		m_view.SetFont(m_fontEdit);

		// set left and right margins
		auto dpi = GetDeviceCaps(dc, LOGPIXELSY);
		auto dpi_factor_font = MulDiv(m_nFontSize, dpi, 72);
		auto margin = MulDiv(20, dpi, 96);
		m_view.SetMargins(margin, margin);
		
		// reset text to force realign to margins
		m_text = GetText();
		if (m_text.length())
		{
			m_view.SetWindowText(utf8_to_wstring(m_text).c_str());
		}

		if (strOldFontName != m_strFontName)
		{
			m_bTraitsChanged = true;
		}
		return 0;
	}

	void CheckLanguage(WORD wID)
	{
		// check the correct menu item
		HMENU hMenu = this->GetMenu();
		CMenuHandle menu(hMenu);
		for (const auto& langid : m_languages)
		{
			menu.CheckMenuItem(langid.first, MF_UNCHECKED);
		}
		/*menu.CheckMenuItem(LANG_ENGLISH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_GERMAN, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_FRENCH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_SPANISH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_PORTUGUESE, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_ITALIAN, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_DUTCH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_POLISH, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_RUSSIAN, MF_UNCHECKED);
		menu.CheckMenuItem(LANG_SWEDISH, MF_UNCHECKED);*/
		menu.CheckMenuItem(wID, MF_CHECKED);
	}

	LRESULT OnChangeLanguage(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// does the edit window still contain the default text? make sure to refresh it with the selected language
		bool bTextUnchanged = GetText() == STR(IDS_WELCOME);

		// set UI language
		DWORD dwLanguagesSet = 1;
		SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, m_languages[wID].c_str(), &dwLanguagesSet);
		if (bTextUnchanged)
		{
			m_text = STR(IDS_WELCOME);
			m_view.SetWindowText(utf8_to_wstring(m_text).c_str());
		}

		// refresh all menu items
		UpdateControlItemTexts();

		// refresh status bar
		UpdateStatusBar();

		// redraw the whole window
		Invalidate();
		::RedrawWindow(m_hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

		// menu bar needs special treatment
		DrawMenuBar();

		// set checkmark to selected language
		CheckLanguage(wID);

		// remember language to save it on exit
		m_nLanguage = wID;
		m_bTraitsChanged = true;

		return 0;
	}

	const int GetLanguage() const
	{
		return m_nLanguage;
	}

	void SetLanguage(int langid)
	{
		// set UI language
		m_nLanguage = langid;
		if (langid != 0)
		{
			DWORD dwLanguagesSet = 1;
			SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, m_languages[static_cast<WORD>(langid)].c_str(), &dwLanguagesSet);
		}
	}

	LRESULT OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		HDROP hDrop = (HDROP)wParam;
		UINT uFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
		int nResult = Utils::MessageBox(*this, WSTR(IDS_ASK_CONVERT_FILES), MB_YESNO | MB_ICONQUESTION);
		if (nResult == IDYES)
		{
			int nConverted = 0;
			std::string encryptPassword;
			//std::list<std::string> decryptPasswords;
			for (UINT uIndex = 0; uIndex < uFileCount; uIndex++)
			{
				std::string filename;
				UINT uLength = DragQueryFile(hDrop, uIndex, NULL, 0);
				filename.resize(uLength);
				DragQueryFileA(hDrop, uIndex, (char*)filename.c_str(), uLength + 1);
				std::string extension = filename.substr(uLength - 4, -1);
				std::string newfilename = filename.substr(0, uLength - 4);
				extension = str_tolower(extension);
				if (extension == ".txt")
				{
					//_tcscpy_s(&newfilename[uLength - 4], 4, _T(".exe"));
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
					std::string text;
					text.resize(32768);
					sprintf_s((char*)text.c_str(), text.size(), STR(IDS_CANT_CONVERT).c_str(), filename.c_str());
					Utils::MessageBox(*this, utf8_to_wstring(text), MB_OK | MB_ICONERROR);
				}
			}
			if (nConverted)
			{
				std::string text;
				text.resize(32768);
				sprintf_s((char*)text.c_str(), text.size(), STR(IDS_CONVERT_DONE).c_str(), nConverted);
				Utils::MessageBox(*this, utf8_to_wstring(text), MB_OK | MB_ICONINFORMATION);
			}
		}
		DragFinish(hDrop);
		return 0;
	}

	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		std::string encryptPassword;

		CFileDialog dlg(FALSE, _T("exe"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, *this);
		if (dlg.DoModal() == IDOK)
		{
			std::string text;
			text = GetText();
			if (SaveTextToFile(wstring_to_utf8(dlg.m_szFileName), text, encryptPassword))
			{
				//m_text = text;
			}
		}

		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		std::string text = GetText();
		if (m_text != text || (m_text != STR(IDS_WELCOME) && m_bTraitsChanged))
		{
			int nResult = Utils::MessageBox(*this, WSTR(IDS_SAVE_CHANGES), MB_YESNOCANCEL | MB_ICONQUESTION);
			if (nResult == IDCANCEL)
			{
				return FALSE;
			}

			bHandled = FALSE;

			if (nResult == IDNO)
			{
				m_bTraitsChanged = false;
				return FALSE;
			}

			if (text.empty())
			{
				m_text = text;
				return FALSE;
			}

			if (m_password.empty())
			{
				m_password = GetNewPasswordDlg();
				if (m_password.empty())
				{
					bHandled = TRUE;
					return FALSE;
				}
			}
		}

		bHandled = FALSE;
		m_text = text;

		return FALSE;
	}
	
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int m_nOldWindowSizeX = m_nWindowSizeX;
		int m_nOldWindowSizeY = m_nWindowSizeY;
		RECT rc;
		GetWindowRect(&rc);
		m_nWindowSizeX = rc.right - rc.left;
		m_nWindowSizeY = rc.bottom - rc.top;
		bHandled = FALSE;
		if (m_nWindowSizeX != m_nOldWindowSizeX || m_nWindowSizeY != m_nOldWindowSizeY)
		{
			m_bTraitsChanged = true;
			UpdateStatusBar();
		}
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_dwSearchFlags = FR_DOWN;
		m_pCurrentFindReplaceDialog = nullptr;

		DragAcceptFiles(TRUE);

		CreateSimpleStatusBar(); //m_hWndStatusBar

		m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL, WS_EX_WINDOWEDGE);
		m_view.SetLimitText((1<<31)); // allow a lot of text to be entered

		CClientDC dc(*this);
		auto dpi = GetDeviceCaps(dc, LOGPIXELSY);
		auto dpi_factor_font = MulDiv(m_nFontSize, dpi, 72);
		if (m_nWindowSizeX == DEFAULT_WIDTH && m_nWindowSizeY == DEFAULT_HEIGHT)
		{
			// resize window depending on dpi scale factor
			m_nWindowSizeX = MulDiv(m_nWindowSizeX, dpi, 96);
			m_nWindowSizeY = MulDiv(m_nWindowSizeY, dpi, 96);
		}
		m_fontEdit.CreateFont(
			-dpi_factor_font,
			0,
			0,
			0,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			utf8_to_wstring(m_strFontName).c_str());

		ATLASSERT(m_fontEdit);
		m_view.SetFont(m_fontEdit);

		auto margin = MulDiv(20, dpi, 96);
		m_view.SetMargins(margin, margin);

		if (m_text.length())
		{
			m_view.SetWindowText(utf8_to_wstring(m_text).c_str());
			//m_view.SetSelAll();

		}

		std::string modulepath;
		modulepath.resize(MAX_PATH+1);
		std::string title;
		::GetModuleFileNameA(Utils::GetModuleHandle(), (char*)modulepath.c_str(), MAX_PATH);
		int nSlashPos = modulepath.rfind("\\");
		if (nSlashPos != -1)
		{
			title = &modulepath[nSlashPos+1];
			int nDotPos = title.rfind(".");
			if (nDotPos != -1)
			{
				title[nDotPos] = '\0';
			}
		}

		std::string windowtitle;
		windowtitle.resize(MAX_PATH + 100);
		sprintf_s((char*)windowtitle.c_str(), MAX_PATH + 100, "%s - %s", title.c_str(), STR(IDR_MAINFRAME).c_str());

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		m_currentBuffer.m_strText = GetText();
		m_view.GetSel(m_currentBuffer.m_nStartChar, m_currentBuffer.m_nEndChar);

		UpdateControlItemTexts();

		// get default UI language, set checkmark in menu
		if (m_nLanguage == 0)
		{
			LANGID langid = GetUserDefaultUILanguage();
			m_nLanguage = langid & 0b0000000111111111;
		}
		CheckLanguage(static_cast<WORD>(m_nLanguage));

		SetWindowPos(NULL, 0, 0, m_nWindowSizeX, m_nWindowSizeY, SWP_NOZORDER|SWP_NOMOVE);
		CenterWindow();

		m_view.SetFocus();

		m_view.SetSel(m_currentBuffer.m_nStartChar + 2, m_currentBuffer.m_nStartChar + 2);

		SetWindowText(utf8_to_wstring(windowtitle).c_str());

		UpdateStatusBar();

		return 0;
	}

	void UpdateControlItemTexts()
	{
		// convert menus and menu items from utf-8 to unicode
		ChangeMenuText(0, WSTR(IDS_MENU_FILE));
		ChangeMenuText(1, WSTR(IDS_MENU_EDIT));
		ChangeMenuText(2, WSTR(IDS_MENU_VIEW));
		ChangeMenuText(3, WSTR(IDS_MENU_HELP));
		ChangeSubMenuText(2, 0, WSTR(IDS_MENU_FONT));
		ChangeSubMenuText(2, 1, WSTR(IDS_MENU_FONT_SIZE));
		ChangeSubMenuText(2, 2, WSTR(IDS_MENU_LANGUAGE));
		ChangeMenuItemText(57604, WSTR(IDS_MENUITEM_SAVEAS));
		ChangeMenuItemText(ID_FILE_CHANGEPASSWORD, WSTR(IDS_CHANGE_PASSWORD));
		ChangeMenuItemText(ID_APP_EXIT, WSTR(IDS_MENUITEM_QUIT));
		ChangeMenuItemText(ID_MENU_EDIT, WSTR(IDS_MENU_EDIT));
		ChangeMenuItemText(ID_EDIT_UNDO, WSTR(IDS_MENUITEM_UNDO));
		ChangeMenuItemText(ID_EDIT_CUT, WSTR(IDS_MENUITEM_CUT));
		ChangeMenuItemText(ID_EDIT_COPY, WSTR(IDS_MENUITEM_COPY));
		ChangeMenuItemText(ID_EDIT_PASTE, WSTR(IDS_MENUITEM_PASTE));
		ChangeMenuItemText(ID_EDIT_FIND, WSTR(IDS_MENUITEM_FIND));
		ChangeMenuItemText(ID_EDIT_FINDNEXT, WSTR(IDS_MENUITEM_FINDNEXT));
		ChangeMenuItemText(ID_EDIT_SELECT_ALL, WSTR(IDS_MENUITEM_SELECT_ALL));
		ChangeMenuItemText(ID_MENU_VIEW, WSTR(IDS_MENU_VIEW));
		ChangeMenuItemText(ID_MENU_FONT, WSTR(IDS_MENU_FONT));
		ChangeMenuItemText(ID_MENU_FONT_SIZE, WSTR(IDS_MENU_FONT_SIZE));
		ChangeMenuItemText(ID_MENU_HELP, WSTR(IDS_MENU_HELP));
		ChangeMenuItemText(ID_APP_ABOUT, WSTR(IDS_ABOUT_TITLE));
		ChangeMenuItemText(ID_STEGANOS_PASSWORD_MANAGER, WSTR(IDS_MENUITEM_SPM));
		ChangeMenuItemText(ID_STEGANOS_SAFE, WSTR(IDS_MENUITEM_SAFE));
		ChangeMenuItemText(ID_STEGANOS_PRIVACYSUITE, WSTR(IDS_MENUITEM_SSS));
		ChangeMenuItemText(ID_STEGANOS_ONLINESHIELD, WSTR(IDS_MENUITEM_SOS));
		ChangeMenuItemText(ID_STEGANOS_LATESTOFFERS, WSTR(IDS_MENUITEM_LATEST));
		ChangeMenuItemText(ID_STEGANOS_CHECKFORLOCKNOTEUPDATES, WSTR(IDS_MENUITEM_CHECKUPDATE));
	}

	// change the text of the given menu
	void ChangeMenuText(const UINT& menupos, const std::wstring& strNewText)
	{
		HMENU hMenu = GetMenu();
		MENUITEMINFO mii{ sizeof(MENUITEMINFO) };
		GetMenuItemInfo(hMenu, menupos, TRUE, &mii);
		mii.dwTypeData = (LPWSTR)strNewText.c_str();
		mii.fMask = MIIM_TYPE | MIIM_DATA;
		SetMenuItemInfo(hMenu, menupos, TRUE, &mii);
	}

	// change the text of the given menu's submenu
	void ChangeSubMenuText(const UINT& menupos, const UINT& submenupos, const std::wstring& strNewText)
	{
		HMENU hMenu = GetMenu();
		HMENU hSubMenu = GetSubMenu(hMenu, menupos);
		MENUITEMINFO mii{ sizeof(MENUITEMINFO) };
		GetMenuItemInfo(hSubMenu, submenupos, TRUE, &mii);
		mii.dwTypeData = (LPWSTR)strNewText.c_str();
		mii.fMask = MIIM_TYPE | MIIM_DATA;
		SetMenuItemInfo(hSubMenu, submenupos, TRUE, &mii);
	}

	// change the text of the given menu item
	void ChangeMenuItemText(const UINT& id, const std::wstring& strNewText)
	{
		HMENU hMenu = GetMenu();
		MENUITEMINFO mii{ sizeof(MENUITEMINFO) };
		GetMenuItemInfo(hMenu, id, 0, &mii);
		mii.dwTypeData = (LPWSTR)strNewText.c_str();
		mii.fMask = MIIM_TYPE | MIIM_DATA;
		SetMenuItemInfo(hMenu, id, 0, &mii);
	}

	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_view.Copy();
		return 0;
	}

	LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_view.Cut();
		return 0;
	}

	LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_pCurrentFindReplaceDialog)
		{
			m_pCurrentFindReplaceDialog->SendMessageToDescendants(WM_PASTE, 0, 0);
		}
		else
		{
			m_view.Paste();
		}
		return 0;
	}

	LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (!m_listUndo.empty())
		{
			UndoBuffer undo = m_listUndo.back();
			m_listUndo.pop_back();
			m_currentBuffer = undo;
			m_view.SetWindowText(utf8_to_wstring(m_currentBuffer.m_strText).c_str());
			m_view.SetSel(m_currentBuffer.m_nStartChar, m_currentBuffer.m_nEndChar);
		}
		return 0;
	}

	LRESULT OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_pCurrentFindReplaceDialog)
		{
			m_pCurrentFindReplaceDialog->ShowWindow(SW_SHOW);
			m_pCurrentFindReplaceDialog->SetFocus();
			return 0;
		}
		CFindReplaceDialog* pDlgFindReplace = new CFindReplaceDialog();
		pDlgFindReplace->Create(TRUE, utf8_to_wstring(m_strSearchString).c_str(), _T(""), m_dwSearchFlags, *this);
		pDlgFindReplace->ShowWindow(SW_SHOW);
		m_pCurrentFindReplaceDialog = pDlgFindReplace;
		return 0;
	}	

	void FindNext()
	{		
		int nBegin, nEnd;		
		std::string text;
		std::string searchtext;
		text = GetText();
		searchtext = m_strSearchString;
		if (!(m_dwSearchFlags & FR_MATCHCASE))
		{
			text = str_tolower(text);
			searchtext = str_tolower(searchtext);
		}
		m_view.GetSel(nBegin, nEnd);
		
		int nIndex;
		if (!(m_dwSearchFlags & FR_DOWN))
		{
			nIndex = text.rfind(searchtext, nBegin-1);
			if (nIndex == -1)
			{
				nIndex = text.rfind(searchtext, -1);
			}
		}
		else
		{
			nIndex = text.find(searchtext, nEnd);
			if (nIndex == -1)
			{
				nIndex = text.find(searchtext, 0);
			}
		}
		if (nIndex != -1)
		{
			m_view.SetSel(nIndex,nIndex+strlen(searchtext.c_str()));
		}
		else
		{
			char szErrorMessage[MAX_PATH] = "";
			sprintf_s(szErrorMessage, MAX_PATH, STR(IDS_FIND_NOT_FOUND).c_str(), m_strSearchString.c_str());
			Utils::MessageBox(*this, utf8_to_wstring(szErrorMessage).c_str(), MB_OK | MB_ICONWARNING);
			if (m_pCurrentFindReplaceDialog)
			{
				m_pCurrentFindReplaceDialog->SetFocus();
			}
		}
	}

	LRESULT OnEditFindNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (!m_pCurrentFindReplaceDialog)
		{
			BOOL bHandled = FALSE;
			OnEditFind(0, 0, NULL, bHandled);
		}
		else
		{
			FindNext();
		}
		return 0;
	}

	LRESULT OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_view.SetSelAll();
		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFindMsgString(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CFindReplaceDialog* pDlgFindReplace = CFindReplaceDialog::GetNotifier(lParam);
		ATLASSERT(m_pCurrentFindReplaceDialog == pDlgFindReplace);

		m_dwSearchFlags = (pDlgFindReplace->m_fr.Flags) & (FR_WHOLEWORD|FR_MATCHCASE|FR_DOWN);
		m_strSearchString = wstring_to_utf8(pDlgFindReplace->GetFindString());

		if (pDlgFindReplace->IsTerminating())
		{
			m_pCurrentFindReplaceDialog = nullptr;
		}
		else if (pDlgFindReplace->FindNext())
		{
			FindNext();
		}

		return 0;
	}

	LRESULT OnChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_listUndo.size() >= 1000)
		{
			m_listUndo.pop_front();
		}
		m_listUndo.push_back(m_currentBuffer);
		m_currentBuffer.m_strText = GetText();
		m_view.GetSel(m_currentBuffer.m_nStartChar, m_currentBuffer.m_nEndChar);

		UpdateStatusBar();

		return 0;
	}

	LRESULT OnFileChangePassword(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (!m_password.empty())
		{
			std::string strOldPassword = GetPasswordDlg(*this);
			if (strOldPassword.empty())
			{
				return 0;
			}
			if (strOldPassword != m_password)
			{
				Utils::MessageBox(*this, WSTR(IDS_INVALID_PASSWORD), MB_OK | MB_ICONERROR);
				return 0;
			}
		}

		std::string strNewPassword = GetNewPasswordDlg(*this);
		if (strNewPassword.empty())
		{
			return 0;
		}

		m_password = strNewPassword;
		Utils::MessageBox(*this, WSTR(IDS_PASSWORD_CHANGED), MB_OK | MB_ICONINFORMATION);
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnVisitSteganos(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		switch (wID)
		{
		case ID_STEGANOS_PASSWORD_MANAGER:
			::ShellExecuteA(*this, "open", "https://steganos.com/spm", NULL, NULL, SW_SHOW);
			break;
		case ID_STEGANOS_SAFE:
			::ShellExecuteA(*this, "open", "https://steganos.com/safe", NULL, NULL, SW_SHOW);
			break;
		case ID_STEGANOS_PRIVACYSUITE:
			::ShellExecuteA(*this, "open", "https://steganos.com/sss", NULL, NULL, SW_SHOW);
			break;
		case ID_STEGANOS_ONLINESHIELD:
			::ShellExecuteA(*this, "open", "https://steganos.com/sos", NULL, NULL, SW_SHOW);
			break;
		case ID_STEGANOS_LATESTOFFERS:
			::ShellExecuteA(*this, "open", "https://store.steganos.com/1234/purl-LNOFFER", NULL, NULL, SW_SHOW);
			break;
		case ID_STEGANOS_CHECKFORLOCKNOTEUPDATES:
		default:
			::ShellExecuteA(*this, "open", "https://steganos.com/locknote", NULL, NULL, SW_SHOW);
			break;
		}
		return 0;
	}
};
