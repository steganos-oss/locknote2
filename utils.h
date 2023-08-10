// Steganos LockNote - self-modifying encrypted notepad
// Copyright (C) 2006-2010 Steganos GmbH
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

#if _MSC_VER >= 1300 // for VC 7.0
	#ifndef _delayimp_h
	extern "C" IMAGE_DOS_HEADER __ImageBase;
	#endif
#endif

#include "utf8unicode.h"

std::string GetPasswordDlg(HWND hWnd = NULL);
std::string GetNewPasswordDlg(HWND hWnd = NULL);

typedef struct wintraits_t
{
	int m_nWindowSizeX;
	int m_nWindowSizeY;
	int m_nFontSize;
	int m_nLangId;
	std::string m_strFontName;
} LOCKNOTEWINTRAITS, *LPLOCKNOTEWINTRAITS;

namespace Utils
{
	using namespace CryptoPP;

	HMODULE GetModuleHandle(void)
	{
		return ::GetModuleHandle(nullptr);
//#if _MSC_VER < 1300 // earlier than .NET compiler (VC 6.0)
//		MEMORY_BASIC_INFORMATION mbi;
//		static int dummy;
//		VirtualQuery( &dummy, &mbi, sizeof(mbi) );
//
//		return reinterpret_cast<HMODULE>(mbi.AllocationBase);
//#else // VC 7.0
//		return reinterpret_cast<HMODULE>(&__ImageBase);
//#endif
	}

	std::string STR(UINT nResourceID)
	{
		std::string text;
		//text.resize(16384); // this creates a bug when comparing strings later on. don't.
		char szBuffer[16384]{ "" };
		LoadStringA(GetModuleHandle(), nResourceID, szBuffer, 16384);
		text = szBuffer;
		return text;
	}

	std::wstring WSTR(UINT nResourceID)
	{
		std::wstring text;
		//text.resize(16384); // this creates a bug when comparing strings later on. don't.
		CHAR szBuffer[16384]{ "" };
		LoadStringA(GetModuleHandle(), nResourceID, szBuffer, 16384);
		text = utf8_to_wstring(szBuffer);
		return text;
	}

	int MessageBox(HWND hWnd, const std::wstring& text, UINT uType)
	{
		return ::MessageBox(hWnd, text.c_str(), WSTR(IDR_MAINFRAME).c_str(), uType);
	}

	bool UpdateResource(const std::string& strExePath, const std::string& strResourceName, const std::string& strResourceSection, std::vector<unsigned char>& arrayBuffer)
	{
		bool bResult = false;
		HANDLE hFile = ::BeginUpdateResourceA(strExePath.c_str(), false);
		if (hFile)
		{
			bResult = ::UpdateResourceA(hFile, strResourceSection.c_str(), strResourceName.c_str(), LANG_NEUTRAL, &arrayBuffer[0], arrayBuffer.size())?true:false;

			if (!::EndUpdateResource(hFile, false))
			{
				bResult = false;
			}
		}
		return bResult;
	}

	bool UpdateResource(const std::string& strExePath, const std::string& strResourceName, const std::string& strResourceSection, const std::string& strText)
	{
		std::vector<unsigned char> arrayBuffer;
		size_t dwSize = (strText.size() + 1) * sizeof(char);
		arrayBuffer.resize(dwSize);
		memcpy(&arrayBuffer[0], strText.c_str(), dwSize);
		return UpdateResource(strExePath, strResourceName, strResourceSection, arrayBuffer);
	}

	bool LoadResource(const std::string& strResourceName, const std::string& strResourceSection, std::vector<unsigned char>& arrayBuffer, HMODULE hModule = GetModuleHandle())
	{
		bool bResult = false;
		HRSRC hResInfo = ::FindResourceA(hModule, strResourceName.c_str(), strResourceSection.c_str());
		if (hResInfo)
		{
			DWORD dwSize = ::SizeofResource(hModule, hResInfo);
			HGLOBAL hRes = ::LoadResource(hModule, hResInfo);
			if (hRes && dwSize)
			{
				void* pData = ::LockResource(hRes);
				if (pData)
				{			
					arrayBuffer.resize(dwSize,0);
					memcpy(&arrayBuffer[0],pData,dwSize);
					bResult = true;
					UnlockResource(hRes);
				}			
			}
		}
		return bResult;
	}

	bool LoadResource(const std::string& strResourceName, const std::string& strResourceSection, std::string& strText, HMODULE hModule = GetModuleHandle())
	{
		bool bResult = false;
		std::vector<unsigned char> arrayBuffer;
		bResult = LoadResource(strResourceName, strResourceSection, arrayBuffer, hModule);
		if (bResult)
		{
			arrayBuffer[arrayBuffer.size() - 1] = '\0';
			strText = (char*)&arrayBuffer[0];			
		}
		return bResult;
	}

	bool EncryptString(const std::string& strText, const std::string& strPassword, std::string& strEncryptedData)
	{
		RandomPool randPool;
		int nSeedlen = 32;
		SecByteBlock seed(nSeedlen);

		OS_GenerateRandomBlock(false, seed, seed.size());
		randPool.IncorporateEntropy(seed, seed.size());

		AESLayer Aes;
		DWORD dwMaxCipherTextLen = Aes.MaxCiphertextLen(strText.length());
		byte *pCipher = new byte[dwMaxCipherTextLen];
		memset(pCipher, 0, dwMaxCipherTextLen);
		DWORD dwCipherLen = Aes.Encrypt(randPool, strPassword, pCipher, strText);
		HexEncoder hex(new StringSink(strEncryptedData));
		hex.Put(pCipher, dwCipherLen);
		delete []pCipher;
		return true;
	}

	bool DecryptString(const std::string& strEncryptedData, const std::string& strPassword, std::string& strText)
	{
		AESLayer Aes;
		DWORD dwCipherTextLength = strEncryptedData.size() / 2;
		byte *pCipher = new byte[dwCipherTextLength];
		HexDecoder hex(new ArraySink(pCipher, dwCipherTextLength));
		hex.Put((byte*)strEncryptedData.c_str(), strEncryptedData.size());
		ConstByteArrayParameter cbar((const byte*)pCipher, dwCipherTextLength);
		byte *pPlaintext = new byte[dwCipherTextLength];
		DecodingResult res = Aes.Decrypt(strPassword, pPlaintext, cbar);
		delete[] pCipher;
		if (res.isValidCoding)
		{
			strText = (char*)pPlaintext;
			strText = strText.substr(0, res.messageLength); 
			delete[] pPlaintext;
			return true;
		}
		else 
		{
			delete[] pPlaintext;
			return false;
		}	
	}

	std::string Quote(const std::string& strText)
	{
		return std::string("\"") + strText + std::string("\"");
	}

	// lower a std::string. from https://en.cppreference.com/w/cpp/string/byte/tolower
	std::string str_tolower(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); }
		);
		return s;
	}

	bool LoadTextFromFile(const std::string& path, std::string& text, std::string& password)
	{
		std::string filepath = path;

		filepath = str_tolower(filepath);
		int nFileNameLength = filepath.size();
		if (nFileNameLength > 4)
		{
			if (filepath.substr(nFileNameLength - 4, -1) == ".txt")
			{
				FILE* f = nullptr;
				errno_t err = fopen_s(&f, path.c_str(), "rb");
				if (err == 0 && f)
				{
					fseek(f, 0, SEEK_END);
					DWORD dwFileSize = (DWORD)ftell(f);
					fseek(f, 0, SEEK_SET);
					if (dwFileSize)
					{
						text.resize(dwFileSize + 1);
						fread((char*)text.c_str(), sizeof(char), dwFileSize, f);
						text[dwFileSize] = '\0';
					}
					fclose(f);
					return true;
				}
				return false;
			}
		}

		return false;
	}

	bool SaveTextToFile(const std::string& path, const std::string& text, std::string& password, HWND hWnd = 0, LPLOCKNOTEWINTRAITS wintraits = nullptr)
	{
		std::string filepath = path;

		//strlwr((char*)filepath.c_str());
		int nFileNameLength = filepath.size();
		if (nFileNameLength > 4)
		{
			std::string extension = filepath.substr(nFileNameLength - 4, -1);
			//_strlwr_s((char*)extension.c_str(), extension.size());
			extension = str_tolower(extension);
			if (extension == ".txt")
			{
				FILE* f = nullptr;
				errno_t err = fopen_s(&f, filepath.c_str(), "rb");
				if (err == 0 && f)
				{
					bool bResult = (fwrite(text.c_str(), sizeof(char), text.size(), f) == text.size());
					fclose(f);
					return bResult;
				}
				return false;
			}
		}			
		
		if (password.empty())
		{
 			password = GetNewPasswordDlg();
			if (password.empty())
			{
				return false;
			}
		}

		std::string data = "";
		if (!text.empty())
		{
			Utils::EncryptString(text, password, data);
		}
		else
		{
			Utils::MessageBox(hWnd, WSTR(IDS_TEXT_IS_ENCRYPTED), MB_OK | MB_ICONINFORMATION);
		}

		char szModulePath[MAX_PATH] = {'\0'};

		::GetModuleFileNameA(Utils::GetModuleHandle(), szModulePath, MAX_PATH);
		::CopyFileA(szModulePath, filepath.c_str(), FALSE);	
		bool bResult = Utils::UpdateResource(filepath.c_str(), "CONTENT", "PAYLOAD", data);

		if (wintraits)
		{
			// write window sizes to resource
			std::string sizeinfo;
			char szSizeInfo[MAX_PATH] = "";
			sprintf_s(szSizeInfo, MAX_PATH, "%d", wintraits->m_nWindowSizeX);
			sizeinfo = szSizeInfo;
			Utils::UpdateResource(filepath.c_str(), "SIZEX", "INFORMATION", sizeinfo);
			sprintf_s(szSizeInfo, MAX_PATH, "%d", wintraits->m_nWindowSizeY);
			sizeinfo = szSizeInfo;
			Utils::UpdateResource(filepath.c_str(), "SIZEY", "INFORMATION", sizeinfo);
		
			// write font size
			std::string fontsize;
			char szFontSize[MAX_PATH] = "";
			sprintf_s(szFontSize, MAX_PATH, "%d", wintraits->m_nFontSize);
			fontsize = szFontSize;
			Utils::UpdateResource(filepath.c_str(), "FONTSIZE", "INFORMATION", fontsize);

			// write font typeface
			Utils::UpdateResource(filepath.c_str(), "TYPEFACE", "INFORMATION", wintraits->m_strFontName);

			// write language code if changed
			int langid = wintraits->m_nLangId;
			if (langid != 0)
			{
				std::stringstream sslanguage;
				sslanguage << langid;
				Utils::UpdateResource(filepath.c_str(), "LANGID", "INFORMATION", sslanguage.str());
			}
		}

		return bResult;
	}
}

using namespace Utils;
