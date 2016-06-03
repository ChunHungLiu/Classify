#ifndef EXTERNALMEMORYMANAGER_HPP
#define EXTERNALMEMORYMANAGER_HPP

#include "globals.hpp"
#include <memorymanagerinterface.hpp>

class PLUGIN_EXPORT ExternalMemoryManager
	: public QObject, public MemoryManagerInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.theaifam5.Classify.Plugins.ExternalMemoryManager")
	Q_INTERFACES(MemoryManagerInterface)

	#if Q_CC_MSVC
	HANDLE _processHandle;

	bool IsValidHandle(HANDLE handle) {
		return handle != NULL && handle != INVALID_HANDLE_VALUE;
	}
	#endif

	bool _isOpened;
	quint64 _id;
	QString _name;
public:
	bool open(quint64 processId) override;
	const quint64& processId() override { return _id; }
	const quintptr base(const QString& moduleName = "") override;
	const quint64 size(const QString& moduleName = "") override;
	void read(const quintptr address, void* buffer, size_t size) override;
	void write(quintptr address, const void* buffer, size_t size) override;
	void close() override;
};

#endif // EXTERNALMEMORYMANAGER_HPP
