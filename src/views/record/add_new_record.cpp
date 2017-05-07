

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QDir>
#include <QIcon>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSizePolicy>
#include <QTextDocumentFragment>
#include <QTextEdit>
#include <QWidget>
#include <QtDebug>

#include "add_new_record.h"
#include "info_field_enter.h"
#include "libraries/disk_helper.h"
#include "libraries/global_parameters.h"
#include "libraries/qtm/blogger.h"
#include "libraries/wyedit/editor.h"
#include "main.h"
#include "models/app_config/app_config.h"

#include "views/record/editor_wrap.h"

extern std::shared_ptr<gl_para> gl_paras;
extern std::shared_ptr<AppConfig> appconfig;

// Окно добавления новой записи

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(AddNewRecord)
#endif

#if QT_VERSION < 0x050000
AddNewRecord::AddNewRecord(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
    , _blogger(new Blogger(gl_para::_default_topic, gl_para::_default_post, appconfig->hide_editor_tools() + (QStringList() << "save"
															    << "show_text"
															    << "attach")))
#else
AddNewRecord::AddNewRecord(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , _blogger(new Blogger(gl_para::_default_topic, gl_para::_default_post, appconfig->hide_editor_tools() + (QStringList() << "save"
															    << "show_text"
															    << "attach")))
#endif
{
	setupUI();
	setupSignals();
	assembly();

	setupEventFilter();
}

AddNewRecord::~AddNewRecord()
{
}

void AddNewRecord::setupUI(void)
{
	this->setWindowTitle(tr("Enter a new note"));

	// Ввод инфополей записи
	infoField = new InfoFieldEnter();

//	_blogger =
//	    new Blogger(gl_para::_default_topic, gl_para::_default_post, appconfig->hide_editor_tools() + (QStringList() << "save"
//															 << "show_text"
//															 << "attach"));
	//		(new web::Browser(gl_para::_default_post,
	//gl_para::_default_topic
	//				    , QByteArray()
	//				    , appconfig->hide_editor_tools() + (QStringList()	<<
	//"save"
	//											<<
	//"show_text"
	//											<<
	//"attach")))->blogger();

	//// Редактор текста записи
	// recordTextEditor = _editing_window->editor();
	// new Editor(editing_window->main_stack()
	// , editing_window
	// , (appconfig->interface_mode() == "desktop") ? Editor::WYEDIT_DESKTOP_MODE
	// : Editor::WYEDIT_MOBILE_MODE
	// , appconfig->hide_editor_tools() + (QStringList()	<< "save"
	// << "show_text"
	// << "attach")
	// , false
	// , false);

	//
	// recordTextEditor->disable_tool_list(appconfig->hide_editor_tools() +
	// (QStringList()	<< "save"
	// << "show_text"
	// << "attach"));
	// recordTextEditor->init_enable_assembly(true);
	// recordTextEditor->init_config_file_name(globalparameters.root_path() + "/"
	// + globalparameters.target_os() + "/editorconf.ini");
	// recordTextEditor->init_enable_random_seed(false);
	// recordTextEditor->init(Editor::WYEDIT_DESKTOP_MODE); // Так как это окно, в
	// мобильном режие его инициализировать ненужно, так как есть кнопка Отмена

	// Кнопки OK и Cancel
	buttonBox = new QDialogButtonBox();
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::NoButton | QDialogButtonBox::Cancel);

	// На кнопку OK назначается комбинация клавиш Ctrl+Enter
	QPushButton* OkButton = buttonBox->button(
	    QDialogButtonBox::Ok); // Выясняется указатель на кнопку OK
	OkButton->setShortcut(
	    QKeySequence(Qt::CTRL + Qt::Key_Return)); // Устанавливается шорткат
	OkButton->setToolTip(tr("Ctrl+Enter"));
}

void AddNewRecord::setupSignals(void)
{
	connect(buttonBox, &QDialogButtonBox::accepted, this, &AddNewRecord::okClick);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &AddNewRecord::reject);
}

void AddNewRecord::assembly(void)
{
	// азмещалка элементов
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setMargin(8);
	layout->setSpacing(10);

	// Добавление элементов в размещалку
	layout->addWidget(infoField);
	layout->addWidget(_blogger->editor()); // recordTextEditor
	layout->addWidget(buttonBox, 0, Qt::AlignRight);

	setLayout(layout);

	// Фокус устанавливается на поле ввода названия записи
	infoField->setFocusToStart();

	// QWidget *wdgt=new QWidget;
	// wdgt->setLayout(vbl);
	// setCentralWidget(wdgt);
}

void AddNewRecord::setupEventFilter(void)
{
	// Для области редактирования задается eventFilter (используется для отлова
	// нажатия на ESC)

	_blogger->editor()->installEventFilter(
	    this); // recordTextEditor->installEventFilter(this);
}

bool AddNewRecord::eventFilter(QObject* object, QEvent* event)
{
	qDebug() << "Editor::eventFilter()";
	// Отслеживание нажатия ESC в области редактирования текста
	if (object == _blogger->editor()) { // recordTextEditor
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Escape) {
				// qDebug() << "Press ESC key";
				close();

				return true;
			}
		}
	}
	return true;
}

void AddNewRecord::okClick(void)
{
	// QString message = "";

	//// Проверка наличия названия записи
	// if(infoField->getField("name").length() == 0)
	// message = message + tr("Please enter the note's <b>title</b>. ");

	//// Проверка наличия текста записи
	// QTextDocumentFragment i;
	// QString j;
	// i = QTextDocumentFragment::fromHtml(getField("text"));
	// j = i.toPlainText();
	// qDebug() << "AddNewRecord::okClick() : recordtext " << j;

	// if(j.length() == 0)
	// message = message + tr("Please enter the note's <b>text</b>. ");

	//// Если что-то не заполнено, выдается предупреждение
	// if(message.length() > 0) {
	// QMessageBox::warning(this, tr("A new note cannot be added"), message,
	// QMessageBox::Close);
	// return;
	// }

	// Картинки сохраняются
	imagesDirName = DiskHelper::create_temp_directory();
	_blogger->editor()->work_directory(imagesDirName, QString(Blogger::_default_filename) + ".html"); // recordTextEditor->work_directory(imagesDirName);
	_blogger->editor()->save_textarea_images(Editor::SAVE_IMAGES_SIMPLE);                             // recordTextEditor->save_textarea_images(Editor::SAVE_IMAGES_SIMPLE);

	emit(accept());
}

QString AddNewRecord::getImagesDirectory(void)
{
	if (imagesDirName.length() == 0) {
		critical_error(
		    "In add new record function can not generate temp directory "
		    "with saved images.");

		return "";
	}
	return imagesDirName;
}

// Получение полей, заполненных в окне добавления записи
QString AddNewRecord::getField(QString name)
{
	if (name == "pin" || name == "name" || name == "author" || name == "home" ||
	    name == "url" || name == "tags")
		return infoField->getField(name);
	if (name == "text")
		return _blogger->editor()->textarea(); // recordTextEditor->textarea();
	// Если запрашиваемого поля нет, возвращается пустая строка
	return QString();
}
