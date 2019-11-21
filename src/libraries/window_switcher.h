#ifndef WINDOWSWITCHER_H
#define WINDOWSWITCHER_H

#include <QtGlobal>
#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#else
#include <QObject>
#endif

namespace web {
	template <typename>
	class Docker;
}
class EditorDock;
class Blogger;

class WindowSwitcher : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(WindowSwitcher)
#else
	Q_OBJECT
#endif

    public:
	explicit WindowSwitcher(QString object_name, QObject* parent = 0);

	void enable(void);
	void disable(void);

	void tree_screen_to_record_screen(void);
	void tree_screen_to_find_in_base(void);
	bool status(void);

	static void record_to_record_screen(
	    void); // Используется как callback из редактора MetaEditor
	void record_to_find_in_base(void);

	void record_screen_to_editor_docker(void);
	void record_screen_to_find_in_base(void);
	void record_screen_to_tree_screen(void);

	void close_find_in_base(void);

	void restore_focus_widget();

    signals:

    public slots:

	void find_in_base_click(void);

    private:
	bool enableSwitcher;
};

#endif // WINDOWSWITCHER_H
