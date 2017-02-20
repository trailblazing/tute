#ifndef _EDITORCONTEXTMENU_H_
#define _EDITORCONTEXTMENU_H_

#include <QMenu>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class EditorContextMenu : public QMenu {
#if QT_VERSION == 0x050600
W_OBJECT(EditorContextMenu)
#else
Q_OBJECT
#endif

public:
	EditorContextMenu(QWidget *parent = 0);
	~EditorContextMenu(void);

	void set_edit_image_properties(bool is_image_select);

signals:
	void undo(void)
#if QT_VERSION == 0x050600
	W_SIGNAL(undo) //
#else
	;
#endif
	void redo(void)
#if QT_VERSION == 0x050600
	W_SIGNAL(redo) //
#else
	;
#endif
	void cut(void)
#if QT_VERSION == 0x050600
	W_SIGNAL(cut) //
#else
	;
#endif
	void copy(void)
#if QT_VERSION == 0x050600
	W_SIGNAL(copy) //
#else
	;
#endif
	void paste(void)
#if QT_VERSION == 0x050600
	W_SIGNAL(paste) //
#else
	;
#endif
	void selectAll(void)
#if QT_VERSION == 0x050600
	W_SIGNAL(selectAll) //
#else
	;
#endif
	void contextMenuEditImageProperties(void)
#if QT_VERSION == 0x050600
	W_SIGNAL(contextMenuEditImageProperties) //
#else
	;
#endif

private: QAction *actionUndo;
	QAction *actionRedo;
	QAction *actionCut;
	QAction *actionCopy;
	QAction *actionPaste;
	QAction *actionSelectAll;
	QAction *actionEditImageProperties;

	void setup_actions(void);
	void setup_signals(void);
	void setup_menu(void);
};

#endif // _EDITORCONTEXTMENU_H_
