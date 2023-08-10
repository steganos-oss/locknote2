
// Copyright (c) 2023 Steganos Software GmbH
//
// LicenseName: Proprietary License

#ifndef _UTF8UNICODE
#define _UTF8UNICODE

#pragma once

#include <string>

#include <windows.h>

std::string wchar_to_utf8(LPCWSTR lpString, bool bIncludeZero = false)
{
	int nDestBufLen=0;
	int nDestStrLen=0;
	LPSTR lpBuffer=NULL;
	
	nDestBufLen = WideCharToMultiByte(
				CP_UTF8,		// code page
				NULL,			// performance and mapping flags
				lpString,		// wide-character string
				-1,				// number of chars in string
				NULL,			// buffer for new string
				0,				// size of buffer (Retrieve)
				NULL,			// default for unmappable chars
				NULL);			// set when default char used
	
	if(nDestBufLen)
	{
		lpBuffer = new CHAR[nDestBufLen];
		
		if(lpBuffer)
		{
			nDestStrLen = WideCharToMultiByte(
						CP_UTF8,		// code page
						NULL,			// performance and mapping flags
						lpString,		// wide-character string
						-1,			// number of chars in string
						lpBuffer,			// buffer for new string
						nDestBufLen,	// size of buffer
						NULL,			// default for unmappable chars
						NULL);			// set when default char used
		}
	}

	std::string strResult;
	if (lpBuffer)
	{
		strResult = lpBuffer;
		if (bIncludeZero)
		{
			strResult.resize(nDestBufLen);
		}
		else
		{
			strResult.resize(nDestBufLen - 1); // -1 to not include \0
		}
		delete [] lpBuffer;
		lpBuffer = NULL;
	}
	return strResult;
}

std::string wstring_to_utf8(std::wstring str, bool bIncludeZero = false)
{
	return wchar_to_utf8(str.c_str(), bIncludeZero);
}

std::wstring utf8_to_wstring(LPCSTR lpString)
{
	// Free with MemoryFree();
	int nDestBufLen=0;
	int nDestStrLen=0;
	LPWSTR lpBuffer=NULL;
	
	nDestBufLen = MultiByteToWideChar(
				CP_UTF8,		// code page
				NULL,			// performance and mapping flags
				lpString,		// multibyte string
				-1,				// number of chars in string
				NULL,			// buffer for new string
				0);				// size of buffer (Retrieve)

	if(nDestBufLen)
	{
		lpBuffer = new WCHAR[nDestBufLen];
		
		if(lpBuffer)
		{
			nDestStrLen = MultiByteToWideChar(
						CP_UTF8,		// code page
						NULL,			// performance and mapping flags
						lpString,		// multibyte string
						-1,			// number of chars in string
						lpBuffer,			// buffer for new string
						nDestBufLen * sizeof(WCHAR));	// size of buffer
		}
	}

	std::wstring strResult;
	if (lpBuffer)
	{
		strResult = lpBuffer;
		delete [] lpBuffer;
		lpBuffer = NULL;
	}
	return strResult;
}

std::wstring utf8_to_wstring(const std::string& s)
{
	return utf8_to_wstring(s.c_str());
}

// buffer must be deleted after use
LPWSTR utf8_to_wchar(LPCSTR lpString)
{
	// Free with MemoryFree();
	int nDestBufLen=0;
	int nDestStrLen=0;
	LPWSTR lpBuffer=NULL;
	
	nDestBufLen = MultiByteToWideChar(
				CP_UTF8,		// code page
				NULL,			// performance and mapping flags
				lpString,		// multibyte string
				-1,				// number of chars in string
				NULL,			// buffer for new string
				0);				// size of buffer (Retrieve)

	if(nDestBufLen)
	{
		lpBuffer = new WCHAR[nDestBufLen];
		
		if (lpBuffer)
		{
			nDestStrLen = MultiByteToWideChar(
						CP_UTF8,		// code page
						NULL,			// performance and mapping flags
						lpString,		// multibyte string
						-1,			// number of chars in string
						lpBuffer,			// buffer for new string
						nDestBufLen * sizeof(WCHAR));	// size of buffer
		}
	}

	return lpBuffer;
}

#endif // _UTF8UNICODE
