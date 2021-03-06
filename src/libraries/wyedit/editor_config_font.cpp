
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QAbstractSpinBox>
#include <QBoxLayout>
#include <QColorDialog>
#include <QDir>
#include <QGroupBox>
#include <QLineEdit>
#include <QString>
#include <QWidget>
#include <QtDebug>

#include "editor_config.h"
#include "editor_config_font.h"
#include "libraries/flat_control.h"
#include "libraries/global_parameters.h"
#include "libraries/wyedit/editor_config.h"
#include "main.h"

extern std::shared_ptr<gl_para> gl_paras;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(EditorConfigFont)
#endif

EditorConfigFont::EditorConfigFont(std::shared_ptr<EditorConfig> editor_config_, QWidget* parent)
    : ConfigPage(parent), _editor_config(editor_config_)
{
	qDebug() << "Create editor config font widget";

	// Устанавливается указатель на объект работы с конфигфайлом
	//        _editor_config = _editor_config
	//	    gl_paras->editor_config();  // find_object<EditorConfig>("editorconfig");

	setup_ui();
	setup_signals();
	assembly();
}
void EditorConfigFont::setup_ui(void)
{
	// Настройка основного шрифта
	defaultFontselectLabel = new QLabel(this);
	defaultFontselectLabel->setText(tr("Default font"));

	// Шрифт
	defaultFontselect = new QFontComboBox(this);
	QFont defaultFont;
	defaultFont.fromString(_editor_config->get_default_font());
	defaultFontselect->setCurrentFont(defaultFont);

	// азмер
	defaultFontsize = new QSpinBox(this);
	defaultFontsize->setMinimum(1);
	defaultFontsize->setMaximum(250);
	defaultFontsize->setValue(_editor_config->get_default_font_size());

	// Настройка моноширинного шрифта
	monospaceFontselectLabel = new QLabel(this);
	monospaceFontselectLabel->setText(tr("Monospace font"));

	// Шрифт
	monospaceFontselect = new QFontComboBox(this);
	QFont monoFont;
	monoFont.fromString(_editor_config->get_monospace_font());
	monospaceFontselect->setCurrentFont(monoFont);

	// азмер
	monospaceFontsize = new QSpinBox(this);
	monospaceFontsize->setMinimum(1);
	monospaceFontsize->setMaximum(250);
	monospaceFontsize->setValue(_editor_config->get_monospace_font_size());
	monospaceFontsize->setEnabled(_editor_config->get_monospace_font_size_apply());

	// Галка применения размера при нажатии кнопки моноширинного форматирования
	monospaceFontsizeapply =
	    new QCheckBox(tr("Font size follows formatting"), this);
	monospaceFontsizeapply->setChecked(_editor_config->get_monospace_font_size_apply());

	// Настройка шрифта форматирования кода
	codeFontselectLabel = new QLabel(this);
	codeFontselectLabel->setText(tr("Code font"));

	// Шрифт
	codeFontselect = new QFontComboBox(this);
	QFont codeFont;
	codeFont.fromString(_editor_config->get_code_font());
	codeFontselect->setCurrentFont(codeFont);

	// азмер
	codeFontsize = new QSpinBox(this);
	codeFontsize->setMinimum(1);
	codeFontsize->setMaximum(250);
	codeFontsize->setValue(_editor_config->get_code_font_size());
	codeFontsize->setEnabled(_editor_config->get_code_font_size_apply());

	// Галка применения размера при нажатии кнопки форматирования кода
	codeFontsizeapply = new QCheckBox(tr("Font size follows formatting"), this);
	codeFontsizeapply->setChecked(_editor_config->get_code_font_size_apply());

	// азмер отступа
	codeIndentsize = new QSpinBox(this);
	codeIndentsize->setMinimum(1);
	codeIndentsize->setMaximum(250);
	codeIndentsize->setValue(_editor_config->get_code_indent_size());
	codeIndentsize->setEnabled(_editor_config->get_code_indent_size_apply());

	// Галка применения размера отступа
	codeIndentsizeapply =
	    new QCheckBox(tr("Indent size follows formatting"), this);
	codeIndentsizeapply->setChecked(_editor_config->get_code_indent_size_apply());

	// Кнопка выбора цвета текста кода
	codeSelectColorLabel = new QLabel(this);
	codeSelectColorLabel->setText(tr("Code color"));

	codeSelectColorButton = new FlatToolButton("", this);
	codeColor = new QColor(_editor_config->get_code_font_color());

	// Квадратик на кнопке выбора цвета кода
	QPixmap pix(16, 16);
	pix.fill(codeColor->rgb());
	codeSelectColorButton->setIcon(pix);
}
void EditorConfigFont::setup_signals(void)
{
	connect(monospaceFontsizeapply, &QCheckBox::stateChanged, this, &EditorConfigFont::on_monospace_applysize_state_changed);
	connect(codeFontsizeapply, &QCheckBox::stateChanged, this, &EditorConfigFont::on_code_applysize_state_changed);
	connect(codeIndentsizeapply, &QCheckBox::stateChanged, this, &EditorConfigFont::on_code_applyindent_state_changed);
	connect(codeSelectColorButton, &FlatToolButton::clicked, this, &EditorConfigFont::on_code_select_color_button_click);
}
void EditorConfigFont::assembly(void)
{
	// Сборка всех блоков в окно

	// Основной шрифт
	QHBoxLayout* defaultFontLayout = new QHBoxLayout();
	defaultFontLayout->addWidget(defaultFontselectLabel);
	defaultFontLayout->addWidget(defaultFontselect);
	defaultFontLayout->addWidget(defaultFontsize);

	// Моноширинный шрифт
	QHBoxLayout* monospaceFontLayout = new QHBoxLayout();
	monospaceFontLayout->addWidget(monospaceFontselectLabel);
	monospaceFontLayout->addWidget(monospaceFontselect);

	QGridLayout* monospaceGridLayout = new QGridLayout();
	monospaceGridLayout->addWidget(monospaceFontsizeapply, 0, 0);
	monospaceGridLayout->addWidget(monospaceFontsize, 0, 1);

	QVBoxLayout* monospaceLayout = new QVBoxLayout();
	monospaceLayout->addLayout(monospaceFontLayout);
	monospaceLayout->addLayout(monospaceGridLayout);

	QGroupBox* monospaceGroupBox = new QGroupBox(tr("Monospace font options"));
	monospaceGroupBox->setLayout(monospaceLayout);

	// Шрифт форматирования кода
	QHBoxLayout* codeFontLayout = new QHBoxLayout();
	codeFontLayout->addWidget(codeFontselectLabel);
	codeFontLayout->addWidget(codeFontselect);

	QGridLayout* codeGridLayout = new QGridLayout();
	codeGridLayout->addWidget(codeSelectColorLabel, 0, 0);
	codeGridLayout->addWidget(codeSelectColorButton, 0, 1);
	codeGridLayout->addWidget(codeFontsizeapply, 1, 0);
	codeGridLayout->addWidget(codeFontsize, 1, 1);
	codeGridLayout->addWidget(codeIndentsizeapply, 2, 0);
	codeGridLayout->addWidget(codeIndentsize, 2, 1);

	QVBoxLayout* codeLayout = new QVBoxLayout();
	codeLayout->addLayout(codeFontLayout);
	codeLayout->addLayout(codeGridLayout);

	QGroupBox* codeGroupBox = new QGroupBox(tr("Code font options"));
	codeGroupBox->setLayout(codeLayout);

	// Общая сборка
	QVBoxLayout* centralLayout = new QVBoxLayout();
	centralLayout->addLayout(defaultFontLayout);
	centralLayout->addWidget(monospaceGroupBox);
	centralLayout->addWidget(codeGroupBox);
	centralLayout->addStretch();

	setLayout(centralLayout);
}
void EditorConfigFont::on_monospace_applysize_state_changed(int i)
{
	if (i == Qt::Checked)
		monospaceFontsize->setEnabled(true);
	else
		monospaceFontsize->setEnabled(false);
}
void EditorConfigFont::on_code_applysize_state_changed(int i)
{
	if (i == Qt::Checked)
		codeFontsize->setEnabled(true);
	else
		codeFontsize->setEnabled(false);
}
void EditorConfigFont::on_code_applyindent_state_changed(int i)
{
	if (i == Qt::Checked)
		codeIndentsize->setEnabled(true);
	else
		codeIndentsize->setEnabled(false);
}
void EditorConfigFont::on_code_select_color_button_click()
{
	// Исходный цвет
	QColor color(_editor_config->get_code_font_color());

	// Диалог запроса цвета
	QColor selectedColor = QColorDialog::getColor(color, this);
	// Если цвет выбран, и он правильный
	if (selectedColor.isValid()) {
		// Меняется цвет кнопки (не работает в Ubuntu 10.04)
		// codeSelectColorButton->setPalette(QPalette(selectedColor));

		// Меняется цвет квадратика на кнопке
		QPixmap pix(16, 16);
		pix.fill(selectedColor);
		codeSelectColorButton->setIcon(pix);

		// Запоминается новый цвет
		codeColor->setNamedColor(selectedColor.name());
	}
}
int EditorConfigFont::apply_changes(void)
{
	qDebug() << "Apply changes editor font";

	// Уровень сложности вносимых изменений
	// 0 - изменения можно делать на лету
	// 1 - для принятия изменений нужна перезагрузка
	int difficultChanges = 0;
	// Если был изменен основной шрифт
	if (_editor_config->get_default_font() != defaultFontselect->currentFont().toString())
		_editor_config->set_default_font(defaultFontselect->currentFont().toString());
	// Если был изменен размер основного шрифта
	if (_editor_config->get_default_font_size() != defaultFontsize->value())
		_editor_config->set_default_font_size(defaultFontsize->value());
	// Если был изменен моноширинный шрифт
	if (_editor_config->get_monospace_font() !=
	    monospaceFontselect->currentFont().toString())
		_editor_config->set_monospace_font(monospaceFontselect->currentFont().toString());
	// Если был изменен размер моноширинного шрифта
	if (_editor_config->get_monospace_font_size() != monospaceFontsize->value())
		_editor_config->set_monospace_font_size(monospaceFontsize->value());
	// Если была изменена галка разрешения установки размера моноширинного шрифта
	if (_editor_config->get_monospace_font_size_apply() !=
	    monospaceFontsizeapply->isChecked())
		_editor_config->set_monospace_font_size_apply(monospaceFontsizeapply->isChecked());
	// Если был изменен шрифт форматирования кода
	if (_editor_config->get_code_font() != codeFontselect->currentFont().toString())
		_editor_config->set_code_font(codeFontselect->currentFont().toString());
	// Если был изменен размер шрифта форматирования кода
	if (_editor_config->get_code_font_size() != codeFontsize->value())
		_editor_config->set_code_font_size(codeFontsize->value());
	// Если была изменена галка разрешения установки размера шрифта кода
	if (_editor_config->get_code_font_size_apply() != codeFontsizeapply->isChecked())
		_editor_config->set_code_font_size_apply(codeFontsizeapply->isChecked());
	// Если был изменен отступ шрифта кода
	if (_editor_config->get_code_indent_size() != codeIndentsize->value())
		_editor_config->set_code_indent_size(codeIndentsize->value());
	// Если была изменена галка разрешения установки отступа шрифта кода
	if (_editor_config->get_code_indent_size_apply() != codeIndentsizeapply->isChecked())
		_editor_config->set_code_indent_size_apply(codeIndentsizeapply->isChecked());
	// Если был изменен цвет начертания кода
	if (_editor_config->get_code_font_color() != codeColor->name())
		_editor_config->set_code_font_color(codeColor->name());
	return difficultChanges;
}
