#include <stdafx.h>
#include "memorytreeviewmodel.hpp"

QVariant MemoryTreeViewModel::RowData::getData(int column) const {
	return _columns.size() > column ? _columns[column] : QVariant();
}

int MemoryTreeViewModel::RowData::getColumnCount() const { return _columns.size(); }
void MemoryTreeViewModel::RowData::dataChanged(const int &column, const QVariant &value) {
	if (_columns.size() < column)
		return;

	_columns[column] = value;
}

bool MemoryTreeViewModel::RowData::canBeChanged(const int &column) const {
	return true;
}

MemoryTreeViewModel::MemoryTreeViewModel(QObject * parent)
	: QAbstractTableModel(parent) {}

QVariant MemoryTreeViewModel::data(const QModelIndex &index, int role) const {
	switch (role) {
		case Qt::DecorationRole:
			return QVariant();

		case Qt::DisplayRole:
			return QString("");

		case Qt::EditRole:
			return QVariant();
	}

	return QVariant();
}

int MemoryTreeViewModel::rowCount(const QModelIndex &index) const {
	return 0;
}

int MemoryTreeViewModel::columnCount(const QModelIndex &parent) const {
	return 0;
}

QVariant MemoryTreeViewModel::headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const {

}

bool MemoryTreeViewModel::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {
	if (role != Qt::EditRole)
		return false;
}

MemoryTreeViewModel::~MemoryTreeViewModel() {}
