#include <QObject>
#include <QtWidgets>
#include <QDialog>

#if Q_CC_MSVC
#include <Dbghelp.h>
#endif

#include <config.h>
#include <distorm.h>

#if defined(Q_PROCESSOR_X86_32) && !defined(Q_PROCESSOR_X86_64)
constexpr int processorArch = 32;
#elif defined(Q_PROCESSOR_X86_64)
constexpr int processorArch = 64;
#endif

//Frames
class MainWindow;

//Dialogs
class AboutDialog;
class OptionsDialog;
class ProcessDialog;

//Managers
class MemoryManagerInterface;