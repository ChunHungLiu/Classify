#include "stdafx.h"
#include "mainwindow.hpp"
#include "aboutdialog.hpp"
#include "processdialog.hpp"

#include "memorymanagerinterface.hpp"

MemoryView::MemoryView(MainWindow *parent) : QObject(parent) {
	view = new QTreeView(parent);
	viewModel = new QStandardItemModel(parent);
	selectionModel = view->selectionModel();

	viewModel->setColumnCount(0);

	QStringList headers;
	headers << "Address" << "Name" << "Data" << "Hex8" << "Hex16" << "Hex32";
	if (processorArch == 64)
		headers << "Hex64";
	headers << "Comment";

	viewModel->setHorizontalHeaderLabels(headers);
	view->setModel(viewModel);

	view->header()->hide();

	connect(selectionModel, &QItemSelectionModel::selectionChanged, parent, &MainWindow::onClassSelected);
}

//////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), _disabledControls(false) {

	initializePlugins();

	initializeObjects();

	prepareLayouts();
	prepareControls();
	prepareModels();
	prepareConnects();

	disableControls();

	//Setup
	addToolBar(Qt::RightToolBarArea, _dataTypesToolBar);
	statusBar()->showMessage(tr("Ready"));
	_filters.push_back("Classif Project (*.clp)");
	_filters.push_back("All files (*.*)");
}

void MainWindow::initializePlugins() {
	QDir pluginDir;
	pluginDir.setPath(QApplication::instance()->applicationDirPath() + QDir::separator() + "Plugins");
	pluginDir.setNameFilters({ "*.dll" });
	pluginDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	for (QFile file : pluginDir.entryList()) {
		QPluginLoader plugin;
		plugin.setFileName(file.fileName());
		if (!plugin.load()) {
			QMessageBox::warning(this, tr("Plugin Manager"), tr("Can't load a plugin %1").arg(file.fileName()));
			continue;
		}
		_plugins.push_back(plugin);
	}


}

void MainWindow::initializeObjects() {
	_processDialog = new ProcessDialog(this);

	_dataTypesToolBar = new QToolBar(tr("Data Types Toolbar"), this);
	_projectDockWidget = new QDockWidget(tr("Project"), this);

	_projectTreeView = new QTreeView(_projectDockWidget);
	_projectTreeViewModel = new QStandardItemModel(this);

	_currentMemoryView = Q_NULLPTR;

	_mainToolBar = addToolBar(tr("Main Toolbar"));
	_menuBar = menuBar();

	_fileMenu = _menuBar->addMenu(tr("&File"));
	_toolMenu = _menuBar->addMenu(tr("&Tools"));
	_helpMenu = _menuBar->addMenu(tr("&Help"));

	//Initialize toolbar actions
	_undoAction = new QAction(QIcon(":/Undo"), tr("&Undo"), _mainToolBar);
	_createNamespaceAction = new QAction(QIcon(":/CreateNamespace"), tr("Create &Namespace"), _mainToolBar);
	_createClassAction = new QAction(QIcon(":/CreateClass"), tr("Create &Class"), _mainToolBar);

	//Initialize menu actions
	_selectProcessAction = new QAction(QIcon(":/SelectProcess"), tr("Select &Process"), _fileMenu);
	_newProjectAction = new QAction(QIcon(":/NewProject"), tr("&New Project"), _fileMenu);
	_openProjectAction = new QAction(QIcon(":/OpenProject"), tr("&Open Project"), _fileMenu);
	_importAction = new QAction(QIcon(":/Import"), tr("&Import"), _fileMenu);
	_saveAction = new QAction(QIcon(":/Save"), tr("&Save"), _fileMenu);
	_saveAsAction = new QAction(QIcon(":/Save"), tr("Save &As"), _fileMenu);
	_quitAction = new QAction(QIcon(":/Quit"), tr("&Quit"), _fileMenu);
	_optionsAction = new QAction(QIcon(":/Options"), tr("&Options"), _toolMenu);
	_checkUpdatesAction = new QAction(QIcon(":/CheckUpdates"), tr("&Check updates"), _helpMenu);
	_aboutAction = new QAction(QIcon(":/About"), tr("&About"), _helpMenu);
}

void MainWindow::prepareLayouts() {
	//setCentralWidget(_memoryTreeView);

	addDockWidget(Qt::LeftDockWidgetArea, _projectDockWidget);
}

void MainWindow::prepareControls() {
	//Create items in toolbars
	_mainToolBar->addAction(_selectProcessAction);
	_mainToolBar->addSeparator();
	_mainToolBar->addAction(_saveAction);
	_mainToolBar->addAction(_undoAction);
	_mainToolBar->addSeparator();
	_mainToolBar->addAction(_createNamespaceAction);
	_mainToolBar->addAction(_createClassAction);

	QList<QString> dataTypes = { "HEX", "UINT", "INT" };
	for (auto& type : dataTypes) {
		for (int h = 8; h <= processorArch; h *= 2) {
			QString text, resName;

			text = QString("%1%2").arg(type, QString::number(h));
			resName = text.toLower();
			resName[0] = resName[0].toUpper();
			if (text.startsWith("U"))
				resName[1] = resName[1].toUpper();

			QAction *currentAction = new QAction(QIcon(QString(":/DataTypes/%1").arg(resName)), text, _dataTypesToolBar);
			currentAction->setObjectName(text);
			_dataTypesToolBar->addAction(currentAction);
			_dataTypeActionList.push_back(currentAction);
		}
		_dataTypesToolBar->addSeparator();
	}
	QList<QString> dataTypeName = { "BOOL", "|", "FLOAT", "DOUBLE", "|", "VEC2", "VEC3" };
	for (auto& type : dataTypeName) {
		QString resName;
		if (type == "|") {
			_dataTypesToolBar->addSeparator();
			continue;
		}

		resName = type.toLower();
		resName[0] = resName[0].toUpper();

		QAction *currentAction = new QAction(QIcon(QString(":/DataTypes/%1").arg(resName)), type, _dataTypesToolBar);
		currentAction->setObjectName(type);
		_dataTypesToolBar->addAction(currentAction);
		_dataTypeActionList.push_back(currentAction);
	}

	//Create items in menubars
	_fileMenu->addAction(_selectProcessAction);
	_fileMenu->addSeparator();
	_fileMenu->addAction(_newProjectAction);
	_fileMenu->addAction(_openProjectAction);
	_fileMenu->addAction(_importAction);
	_fileMenu->addSeparator();
	_fileMenu->addAction(_saveAction);
	_fileMenu->addAction(_saveAsAction);
	_fileMenu->addSeparator();
	_fileMenu->addAction(_quitAction);
	_toolMenu->addAction(_optionsAction);
	_helpMenu->addAction(_checkUpdatesAction);
	_helpMenu->addSeparator();
	_helpMenu->addAction(_aboutAction);

	_projectDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	_projectDockWidget->setWidget(_projectTreeView);
}

void MainWindow::prepareModels() {
	_projectTreeViewModel->setColumnCount(0);
	_projectTreeViewModel->setHorizontalHeaderLabels({ "Object name" });

	_projectTreeView->setModel(_projectTreeViewModel);

	_projectTreeViewSelectionModel = _projectTreeView->selectionModel();
}

void MainWindow::prepareConnects() {
	connect(_undoAction, &QAction::triggered, this, &MainWindow::onUndoAction);
	connect(_createNamespaceAction, &QAction::triggered, this, &MainWindow::onCreateNamespaceAction);
	connect(_createClassAction, &QAction::triggered, this, &MainWindow::onCreateClassAction);
	for (QAction *action : _dataTypeActionList)
		connect(action, &QAction::triggered, this, &MainWindow::onConvertAction);
	connect(_selectProcessAction, &QAction::triggered, this, &MainWindow::onSelectProcessAction);
	connect(_newProjectAction, &QAction::triggered, this, &MainWindow::onNewProjectAction);
	connect(_openProjectAction, &QAction::triggered, this, &MainWindow::onOpenProjectAction);
	connect(_importAction, &QAction::triggered, this, &MainWindow::onImportAction);
	connect(_saveAction, &QAction::triggered, this, &MainWindow::onSaveAction);
	connect(_saveAsAction, &QAction::triggered, this, &MainWindow::onSaveAsAction);
	connect(_quitAction, &QAction::triggered, this, &MainWindow::close);
	connect(_optionsAction, &QAction::triggered, this, &MainWindow::onOptionsAction);
	connect(_checkUpdatesAction, &QAction::triggered, this, &MainWindow::onCheckUpdatesAction);
	connect(_aboutAction, &QAction::triggered, this, &MainWindow::onAboutAction);
}

void MainWindow::enableControls() {
	if (!_disabledControls)
		return;

	for (QAction *action : _dataTypeActionList)
		action->setDisabled(false);

	_saveAction->setDisabled(false);
	_saveAsAction->setDisabled(false);
	_undoAction->setDisabled(false);
	_createNamespaceAction->setDisabled(false);
	_createClassAction->setDisabled(false);

	_disabledControls = false;
}

void MainWindow::disableControls() {
	if (_disabledControls)
		return;

	for (QAction *action : _dataTypeActionList)
		action->setDisabled(true);

	_saveAction->setDisabled(true);
	_saveAsAction->setDisabled(true);
	_undoAction->setDisabled(true);
	_createNamespaceAction->setDisabled(true);
	_createClassAction->setDisabled(true);

	_disabledControls = true;
}

void MainWindow::onClassSelected(const QItemSelection &selected, const QItemSelection &deselected) {
}

void MainWindow::onAboutAction() {
	AboutDialog about(this);
	about.exec();
}

void MainWindow::addRowToMemoryTree(const QAction *from, quintptr address, quint8 size) {
	if (_currentMemoryView == Q_NULLPTR)
		return;

	unsigned char *binaryData = new unsigned char[size];
	QStandardItem *item = Q_NULLPTR;
	QList<QStandardItem*> columns;

	memset(binaryData, 0xFF, size);

	//_processManager->read(address, binaryData, size);
	
	item = new QStandardItem(from->icon(), tr("0x%1").arg(address, 8, 16, QChar('0')));
	item->setForeground(QBrush(QColor("red")));
	columns.push_back(item);

	item = new QStandardItem("unknown");
	item->setForeground(QBrush(QColor("orange")));
	columns.push_back(item);

	item = new QStandardItem("unknown");
	item->setForeground(QBrush(QColor("blue")));
	columns.push_back(item);

	for (quint64 i = 0; i < sizeof(binaryData); ++i) {
		//TODO: Add color
		columns.push_back(new QStandardItem(QString("%1").arg(binaryData[i], 2, 16).toUpper()));
	}
		
	item = new QStandardItem("//");
	item->setForeground(QBrush(QColor("green")));
	columns.push_back(item);

	_currentMemoryView->viewModel->invisibleRootItem()->appendRow(columns);

	for (int i = 0; i < _currentMemoryView->view->header()->count(); ++i)
		_currentMemoryView->view->resizeColumnToContents(i);

	delete[] binaryData;
}

void MainWindow::createWatcher(QList<QStandardItem>& row) {
	/*QFuture<bool> future = QtConcurrent::run([](QList<QStandardItem> row) {
		return true;
	});*/
}

void MainWindow::onSelectProcessAction() {
	if (_processDialog->exec() != QDialog::Accepted)
		return;

	const Process proc = _processDialog->getSelectedProcess();

	if (!_processManager->open(proc.id)) {
		QMessageBox::warning(this, tr("Application"), tr("Can't open handle to process %1").arg(proc.name));
		return;
	}

	setWindowTitle(tr("Classify - %1 - %2").arg(QString::number(proc.id), proc.name));
}

void MainWindow::onUndoAction() {

}

void MainWindow::onSaveAction() {

}

void MainWindow::onSaveAsAction() {

}

void MainWindow::onConvertAction() {
	QObject *currentSender = sender();

	if (currentSender == Q_NULLPTR)
		return;

	QString currentSenderName = currentSender->objectName();
	bool converted = false;
	quint32 value = 0;

	if (currentSenderName.startsWith("HEX")) {
		value = currentSenderName.replace("HEX", "").toInt(&converted);
	} else if (currentSenderName.startsWith("UINT")) {
		value = currentSenderName.replace("UINT", "").toInt(&converted);
	} else if (currentSenderName.startsWith("INT")) {
		value = currentSenderName.replace("INT", "").toInt(&converted);
	} else if (currentSenderName == "BOOL") {
		value = 1;
		converted = true;
	} else if (currentSenderName == "FLOAT") {
		value = 4;
		converted = true;
	} else if (currentSenderName == "DOUBLE") {
		value = 4;
		converted = true;
	} else {
		converted = false;
	}

	if (!converted)
		return;

	addRowToMemoryTree((QAction*)currentSender, _processManager->base(), value);
}

void MainWindow::onCheckUpdatesAction() {

}

void MainWindow::onCreateClassAction() {
	QStandardItem *item = new QStandardItem(QIcon(":/CreateClass"), tr("NewClass"));
	QStandardItem *node = Q_NULLPTR;

	if (!_projectTreeViewSelectionModel->hasSelection())
		node = _projectTreeViewModel->invisibleRootItem();
	else
		node = _projectTreeViewModel->itemFromIndex(_projectTreeViewSelectionModel->currentIndex());

	node->appendRow(item);

	_memoryList.push_back(MemoryView(this));
	_currentMemoryView = &_memoryList.last();
	setCentralWidget(_currentMemoryView->view);
}

void MainWindow::onOpenProjectAction() {
	enableControls();
}

void MainWindow::onOptionsAction() {
	
}

void MainWindow::onNewProjectAction() {
	enableControls();
}

void MainWindow::onImportAction() {
	enableControls();
}

void MainWindow::onCreateNamespaceAction() {
	_projectTreeViewModel->invisibleRootItem()->appendRow(new QStandardItem(QIcon(":/CreateNamespace"), tr("NewNamespace")));
}

MainWindow::~MainWindow() {

}
