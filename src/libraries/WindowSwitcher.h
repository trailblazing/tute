#ifndef WINDOWSWITCHER_H
#define WINDOWSWITCHER_H

#include <QObject>

class MetaEditor;


class WindowSwitcher : public QObject {
    Q_OBJECT

public:
    explicit WindowSwitcher(QString object_name, MetaEditor *meta_editor, QObject *parent = 0);

    void enableSwitch(void);
    void disableSwitch(void);

    void switchFromTreeToRecordtable(void);
    void switchFromTreeToFindInBase(void);
    bool getSwitchStatus(void);

    static void switchFromRecordToRecordtable(void); // Используется как callback из редактора MetaEditor
    void switchFromRecordToFindInBase(void);

    void switchFromRecordtableToRecord(void);
    void switchFromRecordtableToFindInBase(void);
    void switchFromRecordtableToTree(void);

    void closeFindInBase(void);

    void restoreFocusWidget();

signals:

public slots:

    void findInBaseClick(void);

private:

    bool enableSwitcher;

};

#endif // WINDOWSWITCHER_H
