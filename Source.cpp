#include <stdio.h>
#include <conio.h>
#include <Windows.h>

int main(int argc, char *argv[]) {
	char buffer[81];
	DWORD numberOfBytesWritten;
	DWORD numberOfBytesRead;
	OVERLAPPED overlapped;
	HANDLE portHandle, writtingFinished, readingFinished;
	portHandle = CreateFile(L"COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	writtingFinished = CreateEvent(NULL, FALSE, FALSE, L"write");
	readingFinished = CreateEvent(NULL, FALSE, FALSE, L"read");
	if (portHandle == INVALID_HANDLE_VALUE) {
		printf("Cannot initialize port\n");
		return -1;
	}
	while (1) {
		printf("==============================Lab 7==============================\n");
		printf("Welcome to Serial Port Client! Would you like to do?\n");
		printf("r - Read message\n");
		printf("w - Write message\n");
		printf("e - Exit\n");
		switch (_getch())
		{
		case 'r':
			printf("==============================Reading==============================\n");
			overlapped.Offset = 0;
			overlapped.OffsetHigh = 0;
			overlapped.hEvent = readingFinished;
			ReadFile(portHandle, buffer, 80, &numberOfBytesRead, &overlapped);
			WaitForSingleObject(readingFinished, INFINITE);
			buffer[80] = '/0';
			printf("%s", buffer);
			printf("\n");
			system("pause");
			break;
		case 'w':
			printf("==============================Writing==============================\n");
			system("cls");
			printf("Enter the message, please:\n");
			gets_s(buffer, 80);
			overlapped.Offset = 0;
			overlapped.OffsetHigh = 0;
			overlapped.hEvent = writtingFinished;
			WriteFile(portHandle, buffer, 80, &numberOfBytesWritten, &overlapped);
			WaitForSingleObject(writtingFinished, INFINITE);
			system("pause");
			break;
		case 'e':
			CloseHandle(portHandle);
			return 0;
		default:
			break;
		}
		system("cls");
	}
	return 0;
}