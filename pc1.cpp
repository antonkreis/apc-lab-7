#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <stdlib.h>

int main() {
	char buffer[81];
	DWORD numberOfBytesWritten;
	DWORD numberOfBytesRead;
	OVERLAPPED overlapped;
	DCB dcb;
	HANDLE portHandle, writtingFinished, readingFinished,
	readingMutex, writingMutex, canRead1, canWrite1, canRead2, canWrite2, exitEvent;
	readingMutex = CreateMutexA(NULL, FALSE, "reading_mutex");
	writingMutex = CreateMutexA(NULL, FALSE, "writing_mutex");
	portHandle = CreateFile(L"COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	writtingFinished = CreateEvent(NULL, FALSE, FALSE, L"write");
	readingFinished = CreateEvent(NULL, FALSE, FALSE, L"read");
	canRead1 = CreateEvent(NULL, TRUE, FALSE, L"can_read1");
	canRead2 = CreateEvent(NULL, TRUE, FALSE, L"can_read2");
	canWrite1 = CreateEvent(NULL, TRUE, TRUE, L"can_write1");
	canWrite2 = CreateEvent(NULL, TRUE, TRUE, L"can_write2");
	exitEvent = CreateEvent(NULL, TRUE, FALSE, L"exit");
	if (portHandle == INVALID_HANDLE_VALUE) {
		printf("Cannot initialize port\n");
		return -1;
	}
	GetCommState(portHandle, &dcb);
	dcb.fRtsControl = RTS_CONTROL_TOGGLE;
	SetCommState(portHandle, &dcb);
	while (1) {
		printf("==============================Lab 7==============================\n");
		printf("Welcome to Serial Port Client! Would you like to do?\n");
		printf("r - Read message\n");
		printf("w - Write message\n");
		printf("e - Exit\n");
		switch (_getch())
		{
		case 'r':
			if (WaitForSingleObject(exitEvent, 0) == WAIT_TIMEOUT) {
				system("cls");
				printf("==============================Reading==============================\n");
				if (WaitForSingleObject(canRead1, 0) == WAIT_TIMEOUT) {
					printf("Can not read!\n");
					system("pause");
					break;
				}
				if (WaitForSingleObject(readingMutex, 0) == WAIT_TIMEOUT) {
					printf("The port is busy\n");
					system("pause");
					break;
				}
				ResetEvent(readingFinished);
				if (WaitForSingleObject(writtingFinished, 0) == WAIT_TIMEOUT) {
					printf("The port is not ready\n");
					ReleaseMutex(readingMutex);
					system("pause");
					break;
				}
				overlapped.Offset = 0;
				overlapped.OffsetHigh = 0;
				overlapped.hEvent = readingFinished;
				ReadFile(portHandle, buffer, 80, &numberOfBytesRead, &overlapped);
				WaitForSingleObject(readingFinished, INFINITE);
				buffer[80] = '/0';
				printf("%s", buffer);
				printf("\n");
				SetEvent(canWrite2);
				ResetEvent(canRead1);
				ResetEvent(canRead2);
				SetEvent(canWrite1);
				ReleaseMutex(readingMutex);
				system("pause");
			}
			else {
				printf("Connection lost\n");
				CloseHandle(canRead1);
				CloseHandle(canRead2);
				CloseHandle(canWrite1);
				CloseHandle(canWrite2);
				CloseHandle(readingFinished);
				CloseHandle(writtingFinished);
				CloseHandle(portHandle);
				CloseHandle(readingMutex);
				CloseHandle(writingMutex);
				system("pause");
				return 0;
			}
			break;
		case 'w':
			if (WaitForSingleObject(exitEvent, 0) == WAIT_TIMEOUT) {
				system("cls");
				printf("==============================Writing==============================\n");
				if (WaitForSingleObject(canWrite1, 0) == WAIT_TIMEOUT) {
					printf("The port is full!\n");
					system("pause");
					break;
				}
				if (WaitForSingleObject(writingMutex, 0) == WAIT_TIMEOUT) {
					printf("The port is busy");
					break;
				}

				ResetEvent(writtingFinished);
				printf("Enter the message, please:\n");
				gets_s(buffer, 80);
				overlapped.Offset = 0;
				overlapped.OffsetHigh = 0;
				overlapped.hEvent = writtingFinished;
				WriteFile(portHandle, buffer, 80, &numberOfBytesWritten, &overlapped);
				WaitForSingleObject(writtingFinished, INFINITE);
				ResetEvent(canWrite1);
				ResetEvent(canRead1);
				SetEvent(canRead2);
				ResetEvent(canWrite2);
				SetEvent(writtingFinished);
				ReleaseMutex(writingMutex);
				system("pause");
			}
			else {
				printf("Connection lost\n");
				CloseHandle(canRead1);
				CloseHandle(canRead2);
				CloseHandle(canWrite1);
				CloseHandle(canWrite2);
				CloseHandle(readingFinished);
				CloseHandle(writtingFinished);
				CloseHandle(portHandle);
				CloseHandle(readingMutex);
				CloseHandle(writingMutex);
				system("pause");
				return 0;
			}
			break;
		case 'e':
			if (WaitForSingleObject(exitEvent, 0) == WAIT_TIMEOUT) {
				SetEvent(exitEvent);
				system("pause");
				return 0;
			}
			CloseHandle(canRead1);
			CloseHandle(canRead2);
			CloseHandle(canWrite1);
			CloseHandle(canWrite2);
			CloseHandle(readingFinished);
			CloseHandle(writtingFinished);
			CloseHandle(portHandle);
			CloseHandle(readingMutex);
			CloseHandle(writingMutex);
			system("pause");
			return 0;
		default:
			break;
		}
		system("cls");
	}
	return 0;
}
