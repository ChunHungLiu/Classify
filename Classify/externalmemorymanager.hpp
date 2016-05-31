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

	bool _processOpened;
public:
	ExternalMemoryManager(QObject * parent = Q_NULLPTR);
	bool openProcess(quint64 processId) override;
	void read(const quintptr address, void* buffer, size_t size) override;
	void write(quintptr address, const void* buffer, size_t size) override;
	~ExternalMemoryManager();

private:
	void closeProcess() override;
	
};
#endif // EXTERNALMEMORYMANAGER_HPP