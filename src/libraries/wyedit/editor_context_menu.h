#ifndef _EDITORCONTEXTMENU_H_
#define _EDITORCONTEXTMENU_H_


#include <wobjectdefs.h>
#include <QObject>

#include <QMenu>

class EditorContextMenu : public QMenu {
    W_OBJECT(EditorContextMenu)

    public:
	EditorContextMenu(QWidget *parent = 0);
	~EditorContextMenu(void);

	void set_edit_image_properties(bool is_image_select);

    signals:
	void	undo(void) W_SIGNAL(undo)// ;
	void	redo(void) W_SIGNAL(redo)// ;
	void	cut(void) W_SIGNAL(cut)	// ;
	void	copy(void) W_SIGNAL(copy)// ;
	void	paste(void) W_SIGNAL(paste)	// ;
	void	selectAll(void) W_SIGNAL(selectAll)	// ;
	void	contextMenuEditImageProperties(void) W_SIGNAL(contextMenuEditImageProperties)	// ;

    private:
	QAction * actionUndo;
	QAction *actionRedo;
	QAction *actionCut;
	QAction *actionCopy;
	QAction *actionPaste;
	QAction *actionSelectAll;
	QAction *actionEditImageProperties;

	void	setup_actions(void);
	void	setup_signals(void);
	void	setup_menu(void);
};

#endif	// _EDITORCONTEXTMENU_H_
