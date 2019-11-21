#ifndef ATTACHTABLESCREEN_H
#define ATTACHTABLESCREEN_H

#include "libraries/global_parameters.h"
#include <QAction>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include <QtGlobal>
#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class AttachTableView;
class AttachTableController;
class FlatToolButton;
class EditorWrap;
class Blogger;
// Виджет

class AttachTableScreen : public QWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(AttachTableScreen)
#else
	Q_OBJECT
#endif

    public:
	AttachTableScreen(Blogger* blogger_, QWidget* parent = 0);
	virtual ~AttachTableScreen();

	void clear(void);

	QAction* actionAddAttach;
	QAction* actionAddLink;
	QAction* actionEditFileName;
	QAction* actionDeleteAttach;
	QAction* actionOpenAttach;
	QAction* actionSaveAsAttach;
	QAction* actionShowAttachInfo;

	AttachTableController* attach_table_ctrl();
    signals:

    public slots:

    protected:
	void setupActions(void);
	void setupUI(void);
	void setupSignals(void);
	void assembly(void);

	// Экранные элементы
	QToolBar* _tools_line;
	// Указатель на контроллер таблицы приаттаченных файлов
	AttachTableController* _attach_table_crtl;

	// Указатель на таблицу приаттаченных файлов (представление)
	AttachTableView* _attach_table_view;

	QVBoxLayout* _screen_layout;

	QAction* _action_switch_to_editor;
};

#endif // ATTACHTABLESCREEN_H
