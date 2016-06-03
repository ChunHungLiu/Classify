#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets/QMainWindow>

class MemoryView : public QObject {
	Q_OBJECT
public:
	QTreeView			*view;
	QStandardItemModel	*viewModel;
	QItemSelectionModel	*selectionModel;

	MemoryView(MainWindow *parent = Q_NULLPTR);
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

	friend class MemoryView;

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

private:
	void initializePlugins();

	void initializeObjects();

	void prepareLayouts();
	void prepareControls();
	void prepareModels();
	void prepareConnects();

	void addRowToMemoryTree(const QAction *from, quintptr address, quint8 size);
	void createWatcher(QList<QStandardItem>& row);

	void enableControls();
	void disableControls();

private slots:
	void onSaveAction();
	void onSaveAsAction();
	void onConvertAction();
	void onAboutAction();
	void onCheckUpdatesAction();
	void onCreateClassAction();
	void onOpenProjectAction();
	void onOptionsAction();
	void onNewProjectAction();
	void onImportAction();
	void onCreateNamespaceAction();
	void onSelectProcessAction();
	void onUndoAction();
	void onClassSelected(const QItemSelection &selected, const QItemSelection &deselected);

private:
	QList<QPluginLoader>	_plugins;
	QList<QString>			_filters;

	bool					_disabledControls;

	//Toolbars
	QToolBar				*_mainToolBar;
	QToolBar				*_dataTypesToolBar;

	//Menus
	QMenuBar				*_menuBar;
	QMenu					*_fileMenu;
	QMenu					*_toolMenu;
	QMenu					*_helpMenu;

	//Toolbar Actions
	QAction					*_undoAction;
	QAction					*_createNamespaceAction;
	QAction					*_createClassAction;
	QList<QAction*>			_dataTypeActionList;
	
	//Menu Actions
	QAction					*_selectProcessAction;
	QAction					*_newProjectAction;
	QAction					*_openProjectAction;
	QAction					*_importAction;
	QAction					*_saveAction;
	QAction					*_saveAsAction;
	QAction					*_quitAction;

	QAction					*_optionsAction;

	QAction					*_checkUpdatesAction;
	QAction					*_aboutAction;

	//Controls
	QDockWidget				*_projectDockWidget;
	QTreeView				*_projectTreeView;

	//Models
	QStandardItemModel		*_projectTreeViewModel;
	QItemSelectionModel		*_projectTreeViewSelectionModel;

	//Dialogs
	ProcessDialog			*_processDialog;

	//Classes
	MemoryView				*_currentMemoryView;
	QList<MemoryView>		_memoryList;

	//Memory manager
	MemoryManagerInterface	*_currentMemoryManager;
};

#endif // MAINWINDOW_HPP
