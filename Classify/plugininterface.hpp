#ifndef PLUGININTERFACE_HPP
#define PLUGININTERFACE_HPP

struct PluginInterface {
	virtual bool initialize(QObject *base) = 0;
	virtual bool reinitialize(QObject *base) = 0;
	virtual const QString& name() const = 0;
	virtual const QString& description() const = 0;
	virtual void shutdown() = 0;
};

#endif // PLUGININTERFACE_HPP