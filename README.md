Steganos LockNote 2.0
---------------------
Self-modifying encrypted notepad
Copyright (C) 2023 Steganos Software GmbH

Steganos LockNote 2.0 is the modern incarnation of the popular Steganos LockNote from 2006 - with support
for High Definition displays (HDPI), new fonts, support for more languages as well as upgraded, state of
the art encryption and password derivation technology.

LockNote 2.0 saves your encrypted notes within itself - the program is the document. No installation and no
data folder is required.

Official page - https://www.steganos.com/locknote
Source code - https://github.com/steganos-oss/locknote2
Official binary executable - https://go.steganos.com/productpage_locknote2_download_en


Build Notes
-----------

LockNote 2 builds with Microsoft Visual Studio 2022.

The LockNote 2 source has following dependencies:

* CryptoPP 8.7.0, available at http://www.cryptopp.com/.
  The source must reside in the cryptopp subdirectory, and a compiled libfile
  named 'cryptlib.lib' must reside in the following locations for Debug and Release:
  ```cryptopp/output/win32/debug/cryptlib.lib``` (Debug)
  ```cryptopp/output/win32/release/cryptlib.lib``` (Release)

* WTL 10, available at http://wtl.sourceforge.net/.
  The WTL include files must reside in the wtl10 subdirectory.


History
-------

* 2.0.0, 2023/08/10:
	- NEW: Supported languages: DE EN FR ES PT IT NL SV PL RU
	- NEW: Language selection (from the View menu), selected language is saved
	- NEW: HDPI support
	- NEW: New fonts (Cascadia Code, Consolas) for Windows 10/11
	- NEW: Steganos sub menu with quick links to Steganos' software and the latest news about LockNote 2
	- NEW: Information about the document (Lines, chars, position) in the status bar
	- NEW: SCrypt password and IV derivation (optionally PBKDF2 can be used via compiler switch), added salt
	- NEW: AES CBC with HMAC-SHA256 instead of CFB with HMAC-SHA1, use PKCS#7 padding
	- FIX: replace RNG with modern alternative
	- FIX: fixed build warnings (replaced insecure posix functions with secure versions)
	- FIX: fix for string resource loading
	- FIX: Save As... functionality fixed
	- FIX: bug where text comparison failed and LockNote asked user to save on close although nothing had changed
	
* 1.0.5, 2010/02/16:
	- NEW: Changed default font to Lucida Console to avoid 1/l - O/0 confusion
	- NEW: Font typeface can be selected (Arial/Courier New/Lucida Console/Tahoma/Verdana)
	- NEW: Font size can be selected (9/10/12/14)
	- NEW: LockNote saves window size, font size and font typeface
	- FIX: Find Dialog: Enter/Tab/Escape/Space/Ctrl-V keys work
	- FIX: Find Dialog: Message displayed if search string is not found
	- FIX: Find Dialog: F3 brings up Find Dialog or finds next result
	
* 1.0.4, 2007/04/05:
	- NEW: Added Dutch translation
	- FIX: Path fix for cryptopp.lib in project

* 1.0.3, 2006/03/06:
	- NEW: Added Spanish translation (Castilian)

* 1.0.2, 2006/02/14:
	- NEW: Added French translation
	- NEW: Added 'Save As...' option

* 1.0.1, 2006/01/16:
	- FIX: Removed 30k character limit
	- NEW: Conversion displays a summary message box
	- FIX: Wrong filename displayed when conversion fails, fixed
	- NEW: Made conversion error message more comprehensible
	- FIX: Made case insensitive search work
	- FIX: Filenames are no longer being converted to lowercase
	- FIX: Document was touched even when closing without changes
	- NEW: Dragging files on program icon converts files as well

* 1.0, 2006/01/06: Initial release.


License
-------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
