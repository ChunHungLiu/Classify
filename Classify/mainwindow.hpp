#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

private:
	void initializeObjects();

	void prepareLayouts();
	void prepareControls();
	void prepareModels();
	void prepareConnects();

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

private:
	QList<QString>		_filters;

	//Toolbars
	QToolBar			*_mainToolBar;
	QToolBar			*_dataTypesToolBar;

	//Menus
	QMenuBar			*_menuBar;
	QMenu				*_fileMenu;
	QMenu				*_toolMenu;
	QMenu				*_helpMenu;

	//Toolbar Actions
	QAction				*_undoAction;
	QAction				*_createNamespaceAction;
	QAction				*_createClassAction;
	QList<QAction*>		_dataTypeActionList;
	
	//Menu Actions
	QAction				*_selectProcessAction;
	QAction				*_newProjectAction;
	QAction				*_openProjectAction;
	QAction				*_importAction;
	QAction				*_saveAction;
	QAction				*_saveAsAction;
	QAction				*_quitAction;

	QAction				*_optionsAction;

	QAction				*_checkUpdatesAction;
	QAction				*_aboutAction;

	//Controls
	QDockWidget			*_projectDockWidget;
	QTreeView			*_projectTreeView;
	QTreeView			*_memoryTreeView;

	//Models
	QStandardItemModel	*_projectTreeViewModel;
	QItemSelectionModel	*_projectTreeViewSelectionModel;
	MemoryTreeViewModel	*_memoryTreeViewModel;

	//Dialogs
	ProcessDialog		*_processDialog;

	//Memory mananger
	MemoryManager		*_memoryManager;

};

#endif // MAINWINDOW_HPP
