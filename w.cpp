#include <iostream>
#include <windows.h>

int main() {
	MessageBox(NULL, "test", "Test", 0x20);
	Sleep(2000);

	MessageBox(NULL, "test2", "Test2", 0x06);

	return 0;
}
