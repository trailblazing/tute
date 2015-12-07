#ifndef __GLOBALPARAMETERS_H__
#define __GLOBALPARAMETERS_H__

#include <QObject>
#include <QStatusBar>

//#ifndef QWEBENGINEPAGE_HITTESTCONTENT
//#define QWEBENGINEPAGE_HITTESTCONTENT
//#endif

namespace browser {
    class Entrance;
}

class TreeScreen;
class MetaEditor;
class RecordTableScreen;
class FindScreen;
class WindowSwitcher;
class MainWindow;

class GlobalParameters : public QObject {
    Q_OBJECT

public:
    GlobalParameters(QObject *pobj = 0);
    virtual ~GlobalParameters();

    void setMainProgramFile(QString file);
    QString getMainProgramFile(void);

    void init(void);

    QString getWorkDirectory(void);

    QString getTargetOs(void);
    QString getApplicationName(void);

    void setTreeScreen(TreeScreen *point);
    TreeScreen *getTreeScreen();

    browser::Entrance *entrance();
    void entrance(browser::Entrance *&b);

    void setRecordTableScreen(RecordTableScreen *point);
    RecordTableScreen *getRecordTableScreen();

    void setFindScreen(FindScreen *point);
    FindScreen *getFindScreen();

    void setMetaEditor(MetaEditor *point);
    MetaEditor *getMetaEditor();

    void setStatusBar(QStatusBar *point);
    QStatusBar *getStatusBar();

    void setWindowSwitcher(WindowSwitcher *point);
    WindowSwitcher *getWindowSwitcher();

    MainWindow *mainwindow() {return _mainwindow;}
    void mainwindow(MainWindow *mainwindow) {_mainwindow = mainwindow;}

    void setCryptKey(QByteArray hash);
    QByteArray getCryptKey(void);

    // Файл стилей может создаваться и после развертывания начальных файлов MyTetra
    // Так как в более старых версиях MyTetra его еще небыло
    void createStyleSheetFile(QString dirName);

    void style_source(const QString &source) {_style_source = source;}
    QString style_source()const {return _style_source;}


private:

    void initWorkDirectory(void);
    bool findWorkDirectory(void);
    bool isMytetraIniConfig(QString fileName);
    void createStandartProgramFiles(void);
    void createPortableProgramFiles(void);
    void createFirstProgramFiles(QString dirName);

    TreeScreen          *pointTreeScreen;
    browser::Entrance   *_browsermanager;
    RecordTableScreen   *pointRecordTableScreen;
    FindScreen          *pointFindScreen;
    MetaEditor          *pointMetaEditor;
    QStatusBar          *pointStatusBar;
    WindowSwitcher      *windowSwitcher;
    MainWindow          *_mainwindow;

    QString             mainProgramFile;
    QString             workDirectory;

    QByteArray          passwordHash;
    QString             _style_source;
};

#endif  /* __GLOBALPARAMETERS_H__ */

