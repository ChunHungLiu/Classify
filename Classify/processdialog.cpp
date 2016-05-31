#include "stdafx.h"
#include "processdialog.hpp"

#if Q_CC_MSVC
#include <TlHelp32.h>
//#pragma comment(lib, "Dbghelp.lib")
#endif

ProcessDialog::ProcessDialog(QWidget * parent)
	: QDialog(parent) {
	setWindowTitle(tr("Select Process"));

	initializeObjects();

	initializeLayouts();
	initializeControls();
	initializeModels();
	initializeConnects();

	prepareLayout();

	//Set main layout
	setLayout(_mainLayout);

	//Check system architecture
	#if Q_CC_MSVC
	_fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	Q_ASSERT(_fnIsWow64Process != NULL);

	SYSTEM_INFO info;
	GetNativeSystemInfo(&info);
	_isOs64bit = (info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
	#endif
}

void ProcessDialog::initializeObjects() {
	_mainLayout = new QVBoxLayout;
	_topLayout = new QHBoxLayout;
	_bottomLayout = new QHBoxLayout;

	_proxyModel = new QSortFilterProxyModel(this);
	_listModel = new QStandardItemModel(_proxyModel);

	_processListView = new QListView;
	_searchEdit = new QLineEdit;
	_refreshButton = new QToolButton;
	_openButton = new QToolButton;
}

void ProcessDialog::initializeLayouts() {
	_mainLayout->setSpacing(6);
	_topLayout->setSpacing(6);
	_bottomLayout->setSpacing(6);

	_mainLayout->setContentsMargins(9, 9, 9, 9);
	_topLayout->setContentsMargins(0, 0, 0, 0);
	_bottomLayout->setContentsMargins(0, 0, 0, 0);

	_mainLayout->addLayout(_topLayout);
	_mainLayout->addWidget(_processListView);
	_mainLayout->addLayout(_bottomLayout);
}

void ProcessDialog::initializeControls() {
	//Setup icons
	_refreshButton->setIcon(QIcon(":/Refresh"));
	_openButton->setIcon(QIcon(":/Ok"));

	//Setup text
	_openButton->setText(tr("Open"));
	_searchEdit->setPlaceholderText(tr("Search..."));
}

void ProcessDialog::prepareLayout() {
	_topLayout->addWidget(new QLabel(tr("Processes"), this));
	_topLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	_topLayout->addWidget(_searchEdit);
	_topLayout->addWidget(_refreshButton);
	_bottomLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	_bottomLayout->addWidget(_openButton);
}

void ProcessDialog::initializeModels() {
	_proxyModel->setSourceModel(_listModel);
	_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	_processListView->setModel(_proxyModel);
	_selectionModel = _processListView->selectionModel();
	_processListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ProcessDialog::initializeConnects() {
	connect(_searchEdit, &QLineEdit::textEdited, this, &ProcessDialog::onSearchTextEdited);
	connect(_refreshButton, &QToolButton::released, this, &ProcessDialog::onRefreshButtonReleased);
	connect(_openButton, &QToolButton::released, this, &ProcessDialog::onOpenProcessReleased);
}

bool ProcessDialog::isSelected() {
	if (!_selectionModel->hasSelection()) {
		QMessageBox::warning(this, tr("Failed"), tr("Select process first"));
		return false;
	}

	return true;
}


void ProcessDialog::onRefreshButtonReleased() {
	#if Q_CC_MSVC
	HANDLE						hProcessSnapshot;
	PROCESSENTRY32				processEntry;

	hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	Q_ASSERT(hProcessSnapshot != INVALID_HANDLE_VALUE);

	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnapshot, &processEntry)) {
		do {

			HANDLE hOpen = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
			if (hOpen == NULL)
				continue;

			BOOL isWow64 = FALSE;

			if (_isOs64bit)
				_fnIsWow64Process(hOpen, &isWow64);
			
			_processList.push_back({ QString::fromWCharArray(processEntry.szExeFile), processEntry.th32ProcessID, !!isWow64 });
			
		} while (Process32Next(hProcessSnapshot, &processEntry));
	}

	CloseHandle(hProcessSnapshot);
	#else
	//Linux code
	#endif

	_listModel->clear();
	_processListView->setIconSize({ 32, 32 });

	for (const Process& process : _processList) {
		_listModel->appendRow(new QStandardItem(
			QIcon((process.is64bit ? ":/Resources/X64.png" : ":/Resources/X86.png")),
			process.name
		));
	}
}

void ProcessDialog::showEvent(QShowEvent *event) {
	event->accept();
	onRefreshButtonReleased();
}

void ProcessDialog::onSearchTextEdited(QString str) {
	_proxyModel->setFilterWildcard(QString("%1").arg(str));
}

void ProcessDialog::onOpenProcessReleased() {
	if (!isSelected())
		return;

	auto proc = std::find_if(_processList.cbegin(), _processList.cend(), [=](const Process& p) {
		return p.name.compare(_selectionModel->selectedRows().first().data().toString(), Qt::CaseInsensitive); 
	});
	Q_ASSERT(proc != Q_NULLPTR);

	_selectedProcess = *proc;

	accept();
}

ProcessDialog::~ProcessDialog() {}
