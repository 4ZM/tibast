#define  _WIN32_WINNT  0x0501 

#include <windows.h>
#include <iostream>

using namespace std;

void print_usage() {
	cout << "Usage: " << endl;
	cout << "  " << "$ mez pid [dll]" << endl;
}

int main(int argc, const char* argv[]) {
	if (argc == 1 || argc > 3) {
		print_usage();
		return 0;
	} 

	// First arg is process id to inject into
	uint32_t pid = ::atoi(argv[1]);
	if (pid == 0) {
		cout << "Error: Invalid pid argument '" << argv[1] << "'" << endl;
		return -1;
	}

	// Second arg is dll to inject
	string pill = R"(pill.dll)";
	if (argc == 3) {
		pill = argv[2];
	}

  cout << "Injecting into pid " << pid;
  if (argc == 3)
  	cout << " with custom payload " << pill; 
  cout << endl;

  // do injection

	return 0;
}