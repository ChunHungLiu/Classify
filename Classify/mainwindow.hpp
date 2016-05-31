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
	void initializeLayouts();
	void initializeControls();
	void initializeModels();
	void initializeConnects();

	void prepareLayout();

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
	QList<QString>	_filters;

	//Toolbars
	QToolBar		*_mainToolBar;
	QToolBar		*_dataTypesToolBar;

	//Menus
	QMenuBar		*_menuBar;
	QMenu			*_fileMenu;
	QMenu			*_toolMenu;
	QMenu			*_helpMenu;

	//Toolbar Actions
	QAction			*_undoAction;
	QAction			*_createNamespaceAction;
	QAction			*_createClassAction;
	QList<QAction*> _dataTypeActionList;
	
	//Menu Actions
	QAction			*_selectProcessAction;
	QAction			*_newProjectAction;
	QAction			*_openProjectAction;
	QAction			*_importAction;
	QAction			*_saveAction;
	QAction			*_saveAsAction;
	QAction			*_quitAction;

	QAction			*_optionsAction;

	QAction			*_checkUpdatesAction;
	QAction			*_aboutAction;

	//Dialogs
	ProcessDialog	*_processDialog;

	//Others
	HANDLE			_hSelectedProcess;
};

#endif // MAINWINDOW_HPP
