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
#include <iostream>

using namespace std;

extern "C" {
  int msg_inject(int process_id, const char* dll);
  int thread_inject(int process_id, const char* dll);
}


void print_usage() {
	cout << "Usage: " << endl;
	cout << "  " << "$ tibast pid [dll]" << endl;
}

int main(int argc, const char* argv[]) {
	if (argc == 1 || argc > 3) {
		print_usage();
		return 0;
	} 

	// First arg is process id to inject into
	int32_t pid = ::atoi(argv[1]);
	if (pid == 0 || pid < -1) {
		cout << "Error: Invalid pid argument '" << argv[1] << "'" << endl;
		return -1;
	}

	// Second arg is dll to inject
	string pill = R"(pill.dll)";
	if (argc == 3) {
		pill = argv[2];
	}

	// Convenient way to test payloads
	if (pid	== -1) {
		cout << "Injecting into self" << endl;
		HINSTANCE module = ::LoadLibrary(pill.c_str());
		if (module == NULL)
			cout << "Error: Could not find payload" << endl; 
		return 0;
	}

  cout << "Injecting into pid " << pid;
  if (argc == 3)
  	cout << " with custom payload " << pill; 
  cout << endl;

  // do injection
  //msg_inject(pid, pill.c_str());
  thread_inject(pid, pill.c_str());

	return 0;
}