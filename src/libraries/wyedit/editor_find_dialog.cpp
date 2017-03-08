
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QtDebug>
#include <QtGlobal>

#include "editor.h"
#include "editor_config.h"
#include "editor_find_dialog.h"
#include "libraries/flat_control.h"

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(EditorFindDialog)
#endif

EditorFindDialog::EditorFindDialog(std::shared_ptr<EditorConfig> editor_config, QWidget *parent)
    : QDialog(parent)
    , _editor_config(editor_config) {
	setup_ui();
	setup_signals();
	assembly();
	setStatusTip("Find dialog");
	showEvent(new QShowEvent());
}

void EditorFindDialog::setup_ui(void) {
	_line_edit = new QLineEdit();
	_line_edit->setAutoFillBackground(true);
	_line_edit->setMinimumWidth(240);

	_math_case = new FlatCheckBox(tr("&Case sensitive"));
	_whole_words = new FlatCheckBox(tr("&Whole words only"));
	_search_backward = new FlatCheckBox(tr("Search &backward"));

	_find_button = new FlatToolButton(tr("&Find"), this);
	// findButton->setDefault(true);
	_find_button->setEnabled(false);

	this->setWindowTitle(tr("Find in the text"));
}

void EditorFindDialog::setup_signals(void) {
	connect(_line_edit, &QLineEdit::textChanged, this, &EditorFindDialog::enable_find_button);

	connect(_find_button, &QPushButton::clicked, this, &EditorFindDialog::find_clicked);
}

void EditorFindDialog::assembly(void) {
	QHBoxLayout *findLineLayout = new QHBoxLayout();
	findLineLayout->addWidget(_line_edit);
	findLineLayout->addWidget(_find_button);

	QVBoxLayout *centralLayout = new QVBoxLayout();
	centralLayout->addLayout(findLineLayout);
	centralLayout->addWidget(_math_case);
	centralLayout->addWidget(_whole_words);
	centralLayout->addWidget(_search_backward);

	this->setLayout(centralLayout);

	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
}

// Действия при нажатии кнопки Find
void EditorFindDialog::find_clicked(void) {
	QString text = _line_edit->text();

	QTextDocument::FindFlags flags = 0;
	if (_math_case->isChecked())
		flags |= QTextDocument::FindCaseSensitively;
	if (_whole_words->isChecked())
		flags |= QTextDocument::FindWholeWords;
	if (_search_backward->isChecked())
		flags |= QTextDocument::FindBackward;
	emit find_text(text, flags);
}

// Кнопка поиска активна только тогда, когда есть текст для поиска
void EditorFindDialog::enable_find_button(const QString &text) {
	_find_button->setEnabled(!text.isEmpty());
}

void EditorFindDialog::hideEvent(QHideEvent *event) {
	qDebug() << "Hide event of find dialog, window x " << this->x() << " y "
	         << this->y() << " width " << this->width() << " height "
	         << this->height();
	if (this->width() > 0 && this->height() > 0 &&
	    !(this->x() <= 0 && this->y() <= 0)) {
		// Получение ссылки в parent виджете на нужное поле
		// EditorConfig *_editor_config = qobject_cast<Editor
		// *>(parent())->_editor_config;

		// Запоминается геометрия
		QRect g = this->frameGeometry();
		qDebug() << "Frame geometry X " << g.x() << " Y " << g.y() << " W "
		         << g.width() << " H" << g.height();
		QString gs = QString::number(g.x()) + "," + QString::number(g.y()) + "," +
		    QString::number(g.width()) + "," + QString::number(g.height());
		_editor_config->set_finddialog_geometry(gs);
	}
	QWidget::hideEvent(event);
}

void EditorFindDialog::showEvent(QShowEvent *event) {
	qDebug() << "Show event of find dialog";

	_line_edit->setFocus();

	// Получение ссылки в parent виджете на нужное свойство
	// EditorConfig *_editor_config = qobject_cast<Editor
	// *>(parent())->_editor_config;

	QString geometry = _editor_config->get_finddialog_geometry();
	// Если была запомнена геометрия окна, устанавливается прежняя геометрия
	if (!geometry.isEmpty()) {
		QStringList geometry_split = geometry.split(",");
		int x = geometry_split.at(0).toInt();
		int y = geometry_split.at(1).toInt();
		// int w=geometry_split.at(2).toInt();
		// int h=geometry_split.at(3).toInt();
		this->move(x, y);
	} else
		qDebug() << "Previos geometry of find dialog is not setted";
	QWidget::showEvent(event);
}
