#include "stdafx.h"
#include "externalmemorymanager.hpp"

ExternalMemoryManager::ExternalMemoryManager(QObject * parent)
	: QObject(parent), _processOpened(false) {
}

bool ExternalMemoryManager::openProcess(quint64 processId) {
	if (_processOpened)
		closeProcess();

	#if Q_CC_MSVC
	_processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, processId);
	if (!IsValidHandle(_processHandle))
		return false;
	#else
	//TODO: Unix Code
	#endif

	_processOpened = true;
	return true;
}

void ExternalMemoryManager::read(const quintptr address, void* buffer, size_t size) {
	size_t bytesRead;

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
	size_t bytesWritten;
	#if Q_CC_MSVC
	if (WriteProcessMemory(_processHandle, (LPVOID)address, buffer, size, &bytesWritten) == 0)
		qDebug() << "Can't write to the process memory! GetLastError: " << GetLastError();

	if (bytesWritten != size)
		qDebug() << "Size not equal!";
	#else
	//TODO: Unix Code
	#endif
}

void ExternalMemoryManager::closeProcess() {
	#if Q_CC_MSVC
	if (IsValidHandle(_processHandle)) {
		CloseHandle(_processHandle);
		_processHandle = NULL;
	}
	#else
	//TODO: Unix Code
	#endif

	_processOpened = false;
}

ExternalMemoryManager::~ExternalMemoryManager() {
	if (_processOpened)
		closeProcess();
}
