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

// Change these values to use different versions
//#define _WIN32_WINNT		0x0501
//#define _WIN32_WINNT_WINXP	0x0501
//#define WINVER				_WIN32_WINNT_WINXP
#define _WIN32_IE			0x0600
#define _RICHEDIT_VER		0x0300

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <string>

#define CRYPTOPP_DEFAULT_NO_DLL

#include "default.h"
#include "hex.h"
#include "randpool.h"
#include "osrng.h"
#include "aeslayer.h"
