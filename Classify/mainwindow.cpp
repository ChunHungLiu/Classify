#include "stdafx.h"
#include "mainwindow.hpp"
#include "aboutdialog.hpp"
#include "processdialog.hpp"

#include "externalmemorymanager.hpp"
#include "memorytreeviewmodel.hpp"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent) {

	initializeObjects();

	prepareLayouts();
	prepareControls();
	prepareModels();
	prepareConnects();

	//Setup
	addToolBar(Qt::RightToolBarArea, _dataTypesToolBar);
	statusBar()->showMessage(tr("Ready"));
	_filters.push_back("Classif Project (*.clp)");
	_filters.push_back("All files (*.*)");

}

void MainWindow::initializeObjects() {
	_memoryManager = new ExternalMemoryManager(this);
	_processDialog = new ProcessDialog(this);

	_dataTypesToolBar = new QToolBar(tr("Data Types Toolbar"), this);
	_projectDockWidget = new QDockWidget(tr("Project"), this);

	_projectTreeView = new QTreeView(_projectDockWidget);
	_projectTreeViewModel = new QStandardItemModel(this);

	_memoryTreeView = new QTreeView(this);
	_memoryTreeViewModel = new MemoryTreeViewModel(this);

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
	setCentralWidget(_memoryTreeView);

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
		for (int h = 8; h <= 64; h *= 2) {
			QString text, resName;

			text = QString("%1%2").arg(type, QString::number(h));
			resName = text.toLower();
			resName[0] = resName[0].toUpper();
			if (text.startsWith("U"))
				resName[1] = resName[1].toUpper();

			QAction *currentAction = new QAction(QIcon(QString(":/DataTypes/%1").arg(resName)), text, _dataTypesToolBar);
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
	_projectTreeViewModel->setColumnCount(1);
	_projectTreeViewModel->setHorizontalHeaderLabels({ "Object name" });

	_projectTreeView->setModel(_projectTreeViewModel);
	_projectTreeViewSelectionModel = _projectTreeView->selectionModel();

	//test
	_projectTreeViewModel->invisibleRootItem()->appendRow(new QStandardItem(QIcon(":/CreateClass"), tr("Test")));
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

void MainWindow::onAboutAction() {
	AboutDialog about(this);
	about.exec();
}

void MainWindow::onSelectProcessAction() {
	if (_processDialog->exec() != QDialog::Accepted)
		return;

	const Process proc = _processDialog->getSelectedProcess();

	if (!_memoryManager->openProcess(proc.id)) {
		QMessageBox::warning(this, tr("Application"), tr("Can't open handle to process %1").arg(proc.name));
		return;
	}

	setWindowTitle(tr("Classify - %1").arg(proc.name));
}

void MainWindow::onUndoAction() {

}

void MainWindow::onSaveAction() {

}

void MainWindow::onSaveAsAction() {

}

void MainWindow::onConvertAction() {
	auto *_pSender = sender();

	if (_pSender == Q_NULLPTR)
		return;
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
}

void MainWindow::onOpenProjectAction() {
}

void MainWindow::onOptionsAction() {
	
}

void MainWindow::onNewProjectAction() {
}

void MainWindow::onImportAction() {

}

void MainWindow::onCreateNamespaceAction() {
	_projectTreeViewModel->invisibleRootItem()->appendRow(new QStandardItem(QIcon(":/CreateNamespace"), tr("NewNamespace")));
}

MainWindow::~MainWindow() {

}
