#include <QObject>
#include <QtWidgets>
#include <QDialog>

#if Q_CC_MSVC
#include <Dbghelp.h>
#endif

//Frames
class MainWindow;

//Dialogs
class AboutDialog;
class OptionsDialog;
class ProcessDialog;

//Managers
class MemoryManager;
class InternalMemoryManager;
class ExternalMemoryManager;