#include "stdafx.h"
#include "externalmemorymanager.hpp"

bool ExternalMemoryManager::open(quint64 processId) {
	if (_isOpened)
		close();

	#if Q_CC_MSVC
	_processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, processId);
	if (!IsValidHandle(_processHandle))
		return false;

	PROCESSENTRY32 processEntry;
	HANDLE hSnapshot;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &processEntry)) {
		do {
			if (processEntry.th32ProcessID == processId) {
				_name = QString::fromWCharArray(processEntry.szExeFile);
				break;
			}
		} while (Process32Next(hSnapshot, &processEntry));
	}

	CloseHandle(hSnapshot);
	#else
	//TODO: Unix Code
	#endif

	if (_name.isEmpty()) {
		close();
		return false;
	}

	_id = processId;
	_isOpened = true;
	return true;
}

const quintptr ExternalMemoryManager::base(const QString& moduleName) {
	if (!_isOpened)
		return 0;

	quintptr address = 0;

	#if Q_CC_MSVC
	MODULEENTRY32 moduleEntry;
	HANDLE hSnapshot;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _id);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &moduleEntry)) {
		do {
			if (QString::compare(QString::fromWCharArray(moduleEntry.szModule), moduleName.isEmpty() ? _name : moduleName, Qt::CaseInsensitive) == 0) {
				address = (quintptr)moduleEntry.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnapshot, &moduleEntry));
	}

	CloseHandle(hSnapshot);
	#else
	//TODO: Unix Code
	#endif

	return address;
}

const quint64 ExternalMemoryManager::size(const QString& moduleName) {
	if (!_isOpened)
		return 0;

	quintptr address = 0;

	#if Q_CC_MSVC
	MODULEENTRY32 moduleEntry;
	HANDLE hSnapshot;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _id);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &moduleEntry)) {
		do {
			if (QString::compare(QString::fromWCharArray(moduleEntry.szModule), moduleName.isEmpty() ? _name : moduleName, Qt::CaseInsensitive) == 0) {
				address = (quintptr)moduleEntry.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnapshot, &moduleEntry));
	}

	CloseHandle(hSnapshot);
	#else
	//TODO: Unix Code
	#endif

	return address;
}

void ExternalMemoryManager::read(const quintptr address, void* buffer, size_t size) {
	SIZE_T bytesRead;

	if (!_isOpened)
		return;

	#if Q_CC_MSVC
	if (ReadProcessMemory(_processHandle, (LPCVOID)address, buffer, size, &bytesRead) == 0)
		qDebug() << "Can't read from the process memory! GetLastError: " << GetLastError();

	if (bytesRead != size)
		qDebug() << "Size not equal!";
	#else
	//TODO: Unix Code
	#endif
}

void ExternalMemoryManager::write(quintptr address, const void* buffer, size_t size) {
	SIZE_T bytesWritten;

	if (!_isOpened)
		return;

	#if Q_CC_MSVC
	if (WriteProcessMemory(_processHandle, (LPVOID)address, buffer, size, &bytesWritten) == 0)
		qDebug() << "Can't write to the process memory! GetLastError: " << GetLastError();

	if (bytesWritten != size)
		qDebug() << "Size not equal!";
	#else
	//TODO: Unix Code
	#endif
}

void ExternalMemoryManager::close() {
	if (!_isOpened)
		return;

	#if Q_CC_MSVC
	if (IsValidHandle(_processHandle)) {
		CloseHandle(_processHandle);
		_processHandle = NULL;
	}
	#else
	//TODO: Unix Code
	#endif

	_isOpened = false;
}
