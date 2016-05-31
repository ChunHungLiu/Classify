#include "stdafx.h"
#include "externalmemorymanager.hpp"

ExternalMemoryManager::ExternalMemoryManager(quint64 processId, QObject * parent)
	: QObject(parent) {
	#if Q_CC_MSVC
	_processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, processId);
	Q_ASSERT(!IsValidHandle(_processHandle));
	#else
	//TODO: Unix Code
	#endif
}

void ExternalMemoryManager::read(const quintptr address, void* buffer, size_t size) {
	size_t bytesRead;

	#if Q_CC_MSVC
	if (ReadProcessMemory(_processHandle, (LPCVOID)address, buffer, size, &bytesRead) == 0)
		qDebug() << "Can't read from the process memory!";

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
		qDebug() << "Can't write to the process memory!";

	if (bytesWritten != size)
		qDebug() << "Size not equal!";
	#else
	//TODO: Unix Code
	#endif
}

ExternalMemoryManager::~ExternalMemoryManager() {
	#if Q_CC_MSVC
	if (IsValidHandle(_processHandle)) {
		CloseHandle(_processHandle);
		_processHandle = NULL;
	}
	#else
	//TODO: Unix Code
	#endif
}
