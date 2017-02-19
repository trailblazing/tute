#ifndef WINDOWSWITCHER_H
#define WINDOWSWITCHER_H

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#else
#include <QObject>
#endif

class EditorDock;
class EditingWindow;

class WindowSwitcher : public QObject {
#if QT_VERSION == 0x050600
    W_OBJECT(WindowSwitcher)
#else
    Q_OBJECT
#endif

public:
    explicit WindowSwitcher(QString object_name, EditorDock* editor_dock, QObject* parent = 0);

    void enable(void);
    void disable(void);

    void tree_to_record_screen(void);
    void tree_to_find_in_base(void);
    bool status(void);

    static void record_to_recordtable(void); // Используется как callback из редактора MetaEditor
    void record_to_find_in_base(void);

    void recordtable_ro_record_editor(void);
    void recordtable_to_find_in_base(void);
    void recordtable_to_tree(void);

    void close_find_in_base(void);

    void restore_focus_widget();

signals:

public slots:

    void find_in_base_click(void);

private:
    bool enableSwitcher;
};

#endif // WINDOWSWITCHER_H
