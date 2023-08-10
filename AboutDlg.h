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

#include <sstream>
#include <string>

#include "utils.h"

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtrlColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtrlColor)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_VISIT_WEBSITE, OnVisitWebsite)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		SetDlgItemText(IDC_COPYRIGHT, WSTR(IDS_COPYRIGHT).c_str());

		unsigned int width = 600;
		unsigned int height = 400;
		CClientDC dc(*this);
		auto dpi = GetDeviceCaps(dc, LOGPIXELSY);
		auto width_adjusted = MulDiv(width, dpi, 96);
		auto height_adjusted = MulDiv(height, dpi, 96);
		auto padding_top_adjusted = MulDiv(20, dpi, 96);
		auto padding_left_adjusted = MulDiv(30, dpi, 96);
		auto image_padding_bottom_adjusted = MulDiv(70, dpi, 96);

		RECT rc, rc_window, rc_image{ 0 };
		GetClientRect(&rc);
		GetWindowRect(&rc_window);

		/*std::stringstream ss;
		ss << "rc.left: " << rc.left << ", rc.right: " << rc.right << ", rc.top: " << rc.top << ", rc.bottom: " << rc.bottom;
		MessageBox(utf8_to_wstring(ss.str()).c_str(), L"Sizes", MB_OK);*/

		// get image size for current dpi
		unsigned int rcwidth = rc.right - rc.left;
		int id_bitmap = IDB_INFO96;
		if (rcwidth < 600)
		{
			id_bitmap = IDB_INFO96;
			width = 450;
			height = 300;
		}
		else if (rcwidth < 700)
		{
			id_bitmap = IDB_INFO120;
			width = 563;
			height = 375;
		}
		else if (rcwidth < 800)
		{
			id_bitmap = IDB_INFO144;
			width = 675;
			height = 450;
		}
		else if (rcwidth < 900)
		{
			id_bitmap = IDB_INFO168;
			width = 788;
			height = 525;
		}
		else if (rcwidth < 1100)
		{
			id_bitmap = IDB_INFO192;
			width = 900;
			height = 600;
		}
		else
		{
			id_bitmap = IDB_INFO144;
			width = 1013;
			height = 675;
		}

		// set image placing for this dpi
		rc_image.left = (rcwidth - width) / 2;
		rc_image.top = padding_top_adjusted;
		rc_image.right = rc_image.left + width;
		rc_image.bottom = rc_image.top + height;
		auto image = GetDlgItem(IDC_STATIC);
		image.MoveWindow(&rc_image, 1);

		// set copyright text placing for this dpi
		RECT rc_copyright = rc;
		rc_copyright.left = padding_left_adjusted;
		rc_copyright.right = rc.right - padding_left_adjusted;
		rc_copyright.top = rc.top + padding_top_adjusted + width / 3 * 2 + padding_left_adjusted;
		rc_copyright.bottom = rc.bottom - rc.top - image_padding_bottom_adjusted;
		auto copyright = GetDlgItem(IDC_COPYRIGHT);
		copyright.MoveWindow(&rc_copyright, 1);

		HBITMAP bmp;
		bmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(id_bitmap), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		if (bmp == NULL)
		{
			MessageBox(L"Error", L"ERROR", MB_OK);
			return TRUE;
		}
		SendDlgItemMessage(IDC_STATIC, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp);
		SetDlgItemText(IDC_VISIT_WEBSITE, WSTR(IDS_VISIT_WEBSITE).c_str());
		
		// set utf-8 to unicode converted window title
		SetWindowText(WSTR(IDS_ABOUT_TITLE).c_str());

		return TRUE;
	}

	LRESULT OnCtrlColor(UINT n, WPARAM wParam, LPARAM lParam, BOOL& b)
	{
		// set background mode and text color
		SetBkMode((HDC)wParam, TRANSPARENT); // transparent background

		return (LRESULT)CreateSolidBrush(RGB(239, 244, 249)); // Windows 11 default window bg color
	}

	LRESULT OnVisitWebsite(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		::ShellExecuteA(*this, "open", "https://steganos.com/locknote", NULL, NULL, SW_SHOW);
		return 0;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
};
