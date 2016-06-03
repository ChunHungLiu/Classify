#ifndef MEMORYMANAGERINTERFACE_HPP
#define MEMORYMANAGERINTERFACE_HPP

#include <memory>
#include <type_traits>

class MemoryManagerInterface {
public:
	//Virtuals
	virtual ~MemoryManagerInterface() {}
	virtual bool open(quint64 id) = 0;
	virtual const quint64& processId() = 0;
	virtual const quintptr base(const QString& moduleName = "") = 0;
	virtual const quint64 size(const QString& moduleName = "") = 0;
	virtual void read(const quintptr address, void* buffer, size_t size) = 0;
	virtual void write(quintptr address, const void* buffer, size_t size) = 0;
	virtual void close() = 0;

	//Templates
	template<typename T>
	T read(const quintptr addres, ...) {
		T object;
		read(address, &object, sizeof(T));
		return object;
	}

	template<typename T>
	std::unique_ptr<typename std::remove_pointer<T>::type>
		read(const quintptr address, typename std::enable_if<std::is_pointer<T>::value>::type* dummy = 0) {
		typedef std::remove_pointer<T>::type TType;

		TType* object = new TType;

		try {
			read(address, object, sizeof(TType));
		} catch (...) {
			delete object;

			return nullptr;
		}

		return std::unique_ptr<TType>(object);
	}

	template<typename T>
	std::unique_ptr<T>
		read(const quintptr address, size_t elementCount, typename std::enable_if<std::is_array<T>::value>::type* dummy = 0) {
		typedef std::remove_all_extents<T>::type TType;

		TType* objects = new TType[elementCount];

		try {
			read(address, objects, sizeof(TType) * elementCount);
		} catch (...) {
			delete[] objects;

			throw;
		}

		return std::unique_ptr<TType[]>(objects);
	}

	template<typename T>
	void write(quintptr address, const T &object) {
		write(address, &object, sizeof(T));
	}

	template<typename T, size_t elementCount>
	void write(quintptr address, T(&objects)[elementCount]) {
		write(address, &objects, sizeof(objects));
	}
};

Q_DECLARE_INTERFACE(MemoryManagerInterface, "org.theaifam5.Classify.MemoryManagerInterface")

#endif // MEMORYMANAGERINTERFACE_HPP