/**
 * Copyright (C) 2013 Anders Sundman <anders@4zm.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 	This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define  _WIN32_WINNT  0x0501 
#include <windows.h>
#include <stdio.h>

/**
 * Sample DLL that pops a message box when loaded. Usefull for testing injection.
 */

BOOL WINAPI DllMain (HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved) {

	if (nReason == DLL_PROCESS_ATTACH) {
		// Do wicked evil pony dance here...

    char msg[128];
    DWORD currentProc = GetCurrentProcessId();
    sprintf(msg, "Pwnies! (%d)", currentProc);
		MessageBoxA(NULL, "We're in ur code, haXing it", msg, MB_OK);

    // Returning false will cause the dll to be unloaded
    return FALSE;
	}

	return TRUE;
}
