#ifndef _METAEDITOR_H_
#define _METAEDITOR_H_

#include <memory>
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QTextCodec>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include "libraries/wyedit/editor.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"

extern QMap<Qt::CheckState, QString>	_string_from_check_state;
extern QMap<QString, Qt::CheckState>	_state_check_from_string;


class QHBoxLayout;
class QGridLayout;
class QScrollArea;
class QSplitter;
class AttachTableScreen;
class Record;
class TreeItem;
class FindScreen;
class RecordModel;

class ClickableLabel : public QLabel {
    Q_OBJECT
    public:
	ClickableLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QLabel(parent, f){}
	ClickableLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QLabel(text, parent, f){}
Q_SIGNALS:
	void mousePressEvent(QMouseEvent *ev);
};

class MetaEditor : public Editor {
    Q_OBJECT

    public slots:

	void	field(QString n, QString v);
	void	clear_all(void);
	void	on_click_to_tag(const QString &text);


    signals:
	// Сигнал вырабатывается при клике на текстовой метке
	void set_find_text_signal(QString text);

    public:
	MetaEditor(QString object_name, FindScreen *_find_screen);
	~MetaEditor(void);

	void	tree_path(QString path);
	void	pin(QString pin_);
	void	switch_pin();
	void	name(QString name_);
	void	author(QString author_);
	void	home(QString url);
	void	url(QString url_);
	void	tags(QString tags_);

	static void to_attach_callback(void);

	void				to_editor_layout(void);
	void				to_attach_layout(void);
	void				bind(boost::intrusive_ptr<TreeItem> item_to_be_bound);		// {_record = r;}

	boost::intrusive_ptr<TreeItem>	item();		// {return _record;}

    private:
	void	setup_labels(void);
	void	setup_ui(void);
	void	assembly(void);
	void	setup_signals(FindScreen *_find_screen);

	FindScreen	*_find_screen;
	QWidget		*_editor_main_screen;
	QGridLayout	*_editor_main_layer;
	QLabel		*_tree_path;		// Надпись Path (только для мобильного интерфейса)
	QCheckBox	*_item_pin;

	QLabel		*_item_name;		// Надпись Title
	QLabel		*_item_author;		// Надпись Author(s)

	QLabel		*_label_home;		// Inscription    // Надпись "Url"
	QLabel		*_label_url;		// Inscription    // Надпись "Url"

	ClickableLabel	*_item_home;
	ClickableLabel	*_item_url;




	QScrollArea	*_item_tags_scrollarea;
	QHBoxLayout	*_item_tags_scrollarea_layout;
	QWidget		*_item_tags_container;
	QHBoxLayout	*_item_tags_layout;
	QLabel		*_label_tags;		// Надпись "Tags"

	QString		_item_tags_text;
	QStringList	_item_tags_text_list;
	QList<QLabel *> _item_tags_labels;

	QSplitter       *_editor_and_filetable_splitter;

	// Виджет слоя прикрепляемых файлов
	AttachTableScreen   *_attachtable_screen;

	// Виджет слоя редактирования текста


	// Группировалка виджетов всех слоев (слоя редактирования и слоя прикрепляемых файлов)
	QVBoxLayout			*_meta_editor_join_layer;
	QMetaObject::Connection		_home_connection;	// for disconnect
	QMetaObject::Connection		_url_connection;	// for disconnect
	boost::intrusive_ptr<TreeItem>	_item;
};

#endif /* _METAEDITOR_H_ */
