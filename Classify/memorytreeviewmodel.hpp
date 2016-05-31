#ifndef MEMORYTREEVIEWMODEL_HPP
#define MEMORYTREEVIEWMODEL_HPP
#include <QAbstractItemModel>

class MemoryTreeViewModel : public QAbstractTableModel {
	Q_OBJECT

	class RowData {
		struct ColumnData {
			QIcon icon;
			QString name;
			quintptr address;
			QList<ClassNode*>* bases;
		};
		QList<> _columns;
	public:
		RowData(QIcon icon, quintptr address, QString& name, QList<ClassNode*>& bases );
		virtual ~RowData() {}
		virtual QVariant getData(int column) const;
		virtual int getColumnCount() const;
		virtual void dataChanged(const int &column, const QVariant &value);
		virtual bool canBeChanged(const int &column) const;
	};

public:
	MemoryTreeViewModel(QObject * parent = Q_NULLPTR);
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	~MemoryTreeViewModel();
};
#endif // MEMORYTREEVIEWMODEL_HPP