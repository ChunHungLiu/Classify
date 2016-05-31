#ifndef EXTERNALMEMORYMANAGER_HPP
#define EXTERNALMEMORYMANAGER_HPP

#include "memorymanager.hpp"

class ExternalMemoryManager : public QObject, public MemoryManager {
	Q_OBJECT

	#if Q_CC_MSVC
	HANDLE _processHandle;

	bool IsValidHandle(HANDLE handle) {
		return handle != NULL && handle != INVALID_HANDLE_VALUE;
	}
	#endif
public:
	ExternalMemoryManager(quint64 processId, QObject * parent = Q_NULLPTR);
	void read(const quintptr address, void* buffer, size_t size) override;
	void write(quintptr address, const void* buffer, size_t size) override;
	~ExternalMemoryManager();
	
};
#endif // EXTERNALMEMORYMANAGER_HPP