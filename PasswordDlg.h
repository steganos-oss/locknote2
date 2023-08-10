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

#include "utils.h"

class CPasswordDlg : public CDialogImpl<CPasswordDlg>
{
public:
	enum { IDD = IDD_PASSWORD };

	bool m_bDualInput;
	std::string m_strCaption;
	std::string m_strPasswordCaption1;
	std::string m_strPasswordCaption2;
	std::string m_strPassword1;
	std::string m_strPassword2;
	std::string m_strText;

	CPasswordDlg()
	{
		m_strCaption = STR(IDR_MAINFRAME);
		m_bDualInput = false;
		m_strPasswordCaption1 = STR(IDS_PASSWORD);
		m_strPasswordCaption2 = STR(IDS_PASSWORD_REPEAT);
		m_strText = STR(IDS_ENTER_PASSWORD);
	}

	BEGIN_MSG_MAP(CPasswordDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		SetWindowText(utf8_to_wstring(m_strCaption).c_str());
		SetDlgItemText(IDC_STATIC_PASSWORD1, utf8_to_wstring(m_strPasswordCaption1).c_str());
		SetDlgItemText(IDC_STATIC_PASSWORD2, utf8_to_wstring(m_strPasswordCaption2).c_str());
		SetDlgItemText(IDC_INFOTEXT, utf8_to_wstring(m_strText).c_str());
		SetDlgItemText(IDCANCEL, WSTR(IDS_CANCEL).c_str());
		SetDlgItemText(IDC_PASSWORD1, utf8_to_wstring(m_strPassword1).c_str());
		SetDlgItemText(IDC_PASSWORD2, utf8_to_wstring(m_strPassword2).c_str());
		if (!m_bDualInput)
		{
			GetDlgItem(IDC_PASSWORD2).ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_PASSWORD2).ShowWindow(SW_HIDE);
		}
		CenterWindow(GetParent());
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		std::wstring strPassword1;
		std::wstring strPassword2;
		strPassword1.resize(256);
		strPassword2.resize(256);
		GetDlgItemText(IDC_PASSWORD1, (TCHAR*)strPassword1.c_str(), 256);
		GetDlgItemText(IDC_PASSWORD2, (TCHAR*)strPassword2.c_str(), 256);
		m_strPassword1 = wstring_to_utf8(strPassword1);
		m_strPassword2 = wstring_to_utf8(strPassword2);
		EndDialog(wID);
		return 0;
	}
};

std::string GetPasswordDlg(HWND hWnd)
{
	CPasswordDlg dlg;
	if (dlg.DoModal(hWnd) == IDCANCEL)
	{
		return "";
	}
	return dlg.m_strPassword1;
}

std::string GetNewPasswordDlg(HWND hWnd)
{
	CPasswordDlg dlg;
	dlg.m_strText = STR(IDS_ENTER_NEW_PASSWORD);
	dlg.m_bDualInput = true;
	while (true)
	{
		if (dlg.DoModal(hWnd) == IDCANCEL)
		{
			return "";
		}
		if (dlg.m_strPassword1 != dlg.m_strPassword2)
		{
			dlg.m_strPassword1 = "";
			dlg.m_strPassword2 = "";
			MessageBox(hWnd, WSTR(IDS_PASSWORD_MISMATCH), MB_OK | MB_ICONERROR);
		}
		else if (dlg.m_strPassword1.empty())
		{
			MessageBox(hWnd, WSTR(IDS_PASSWORD_EMPTY), MB_OK | MB_ICONERROR);
		}
		else
		{
			return dlg.m_strPassword1;
		}
	}
	return "";
}
