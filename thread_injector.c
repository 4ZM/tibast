/**
 * Copyright (C) 2013 Anders Sundman <anders@4zm.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
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

 __declspec(dllexport) 
int thread_inject(int process_id, const char* pill) {

  printf("[+] Injecting thread into proc: %d\n", process_id);

  // Get handle to target process
  printf("[+] OpeningProcess %d\n", process_id);

  HANDLE targetProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
  if (targetProc == 0)
    goto error;

  printf("[+] OpenProcess got handle %d\n", targetProc);


  // Allocate a string for the dll in the remote process
  int pillLen = strlen(pill);
  printf("[+] Will now allocate %d bytes in the remote process\n", pillLen);

  void* remoteBuf = VirtualAllocEx(targetProc, NULL, pillLen, MEM_COMMIT, PAGE_READWRITE);
  if (remoteBuf == 0)
    goto error;
  if (WriteProcessMemory(targetProc, remoteBuf, pill, pillLen, NULL) == 0)
    goto error;

  printf("[+] Allocated and wrote: '%s' to %p\n", pill, remoteBuf);


  // Assuming that LoadLibraryA will be loaded at the same address in the target
  printf("[+] Find LoadLibrary address\n");

  HMODULE k32Module = LoadLibrary("Kernel32.dll");
  if (k32Module == 0)
    goto error;

  FARPROC loadLibraryPtr = GetProcAddress(k32Module, "LoadLibraryA");
  if (loadLibraryPtr == 0)
    goto error;

  printf("[+] Found LoadLibrary addr: %p\n", loadLibraryPtr);


  // Create new thread with LoadLibraryA('pill') as entry point
  DWORD newThreadId = 0;
  HANDLE newThreadHandle = CreateRemoteThread(targetProc, NULL, 0,
    (void*)loadLibraryPtr, remoteBuf, 0, &newThreadId);

  printf("[+] Spawned remote thread id: %d (handle %d)\n", newThreadId, newThreadHandle);

  // Let the thread finish loading the pill
  printf("[+] Waiting for remote thread to finish loading pill.\n");
  WaitForSingleObject(newThreadHandle, INFINITE);  

  DWORD err;
  char errBuffer[256];

  // finally clause
  cleanup:
    if (targetProc != 0 && remoteBuf != 0) VirtualFreeEx(targetProc, remoteBuf, 0, MEM_RELEASE);
    if (targetProc) CloseHandle(targetProc);
    if (k32Module) FreeLibrary(k32Module);

  return 0;

  // catch clause
  error:
    err = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), 
      errBuffer, sizeof(errBuffer) - 1, NULL);
    printf("[-] Error: %s", errBuffer, err);
    goto cleanup;
}
