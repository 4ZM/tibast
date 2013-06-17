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

  return 0;
}