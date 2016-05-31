#ifndef PROCESSDIALOG_HPP
#define PROCESSDIALOG_HPP

struct Process {
	QString name;
	quint64 id;
	bool is64bit;
};

class ProcessDialog : public QDialog {
	Q_OBJECT

public:
	ProcessDialog(QWidget * parent = Q_NULLPTR);
	bool isSelected();
	const Process getSelectedProcess() const { return _selectedProcess; }
	~ProcessDialog();

private:
	void initializeObjects();
	void initializeLayouts();
	void initializeControls();
	void initializeModels();
	void initializeConnects();

	void prepareLayout();

private slots:
	void onRefreshButtonReleased();
	void onSearchTextEdited(QString str);
	void onOpenProcessReleased();
	void showEvent(QShowEvent * event) override;

private:
	//Models
	QStandardItemModel		*_listModel;
	QSortFilterProxyModel	*_proxyModel;
	QItemSelectionModel		*_selectionModel;

	//Layouts
	QVBoxLayout				*_mainLayout;
	QHBoxLayout				*_topLayout;
	QHBoxLayout				*_bottomLayout;

	//Controls
	QListView				*_processListView;
	QLineEdit				*_searchEdit;
	QToolButton				*_refreshButton;
	QToolButton				*_openButton;

	bool					_isOs64bit;
	QList<Process>			_processList;
	Process					_selectedProcess;

	#if Q_CC_MSVC
	using LPFN_ISWOW64PROCESS = BOOL(WINAPI *) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS		_fnIsWow64Process;
	#endif
};
#endif // PROCESSDIALOG_HPP