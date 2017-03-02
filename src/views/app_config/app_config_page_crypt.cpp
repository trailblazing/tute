
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QBoxLayout>
#include <QDir>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "app_config_page_crypt.h"
#include "libraries/crypt/password.h"
#include "libraries/disk_helper.h"
#include "libraries/global_parameters.h"
#include "models/app_config/app_config.h"
#include "models/database_config/database_config.h"
#include "views/enter_password/enter_password.h"

extern std::shared_ptr<AppConfig> appconfig;
extern std::shared_ptr<gl_para> gl_paras;
extern std::shared_ptr<DataBaseConfig> databaseconfig;
extern const char* program_title;
extern const QString program_title_qstring;
extern const std::string program_title_string;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(AppConfigPageCrypt)
#endif

AppConfigPageCrypt::AppConfigPageCrypt(QWidget* parent)
    : ConfigPage(parent)
{
    qDebug() << "Create crypt config page";

    setup_ui();
    update_ui();
    setup_signals();
    assembly();
}

AppConfigPageCrypt::~AppConfigPageCrypt()
{
}

void AppConfigPageCrypt::setup_ui(void)
{
    // Группировщик виджетов работы с паролем
    passRetrieveBox = new QGroupBox(this);
    passRetrieveBox->setTitle(tr("Password settings"));

    // Строка состояния пароля
    passRetrieveStatus = new QLabel(this);
    passRetrieveStatus->setWordWrap(true);

    // Кнопка запроса введения пароля
    passRetrieveButton = new QPushButton(this);
    passRetrieveButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    // Строка с аннотацией
    passRetrieveAnnotation = new QLabel(this);
    passRetrieveAnnotation->setWordWrap(true);

    // Виджеты вставляются в группировщик
    QVBoxLayout* passRetrieveLayout = new QVBoxLayout;
    passRetrieveLayout->addWidget(passRetrieveStatus);
    passRetrieveLayout->addWidget(passRetrieveButton);
    passRetrieveLayout->addWidget(passRetrieveAnnotation);
    passRetrieveBox->setLayout(passRetrieveLayout);

    // Группировщик виджетов выбора, как запрашивать пароль
    howPassRequestBox = new QGroupBox(this);
    howPassRequestBox->setTitle(tr("Access to encrypted data"));

    howPassRequestRadio1 = new QRadioButton(tr("Ask the password when you click on an encrypted item"));
    howPassRequestRadio2 = new QRadioButton(tr(std::string("Ask the password at " + program_title_string + " startup").c_str()));
    // Точка устанавливается возле того пункта, который настроен в конфиге
    if (appconfig->howpassrequest() == "atClickOnCryptBranch")
        howPassRequestRadio1->setChecked(true);
    else
        howPassRequestRadio2->setChecked(true);
    storePassRequestBox = new QGroupBox(this);
    storePassRequestBox->setTitle(tr("Store password"));

    passwordSaveEnable = new QCheckBox(tr("Store password locally"));

    passwordSaveAnnotation = new QLabel(tr("Password will be saved at first next entered.<br>Stored password will be cleared if uncheck this checkbox."));
    if (appconfig->password_save_flag()) {
        howPassRequestRadio1->setEnabled(false);
        howPassRequestRadio2->setEnabled(false);
        passwordSaveEnable->setChecked(true);
    } else {
        howPassRequestRadio1->setEnabled(true);
        howPassRequestRadio2->setEnabled(true);
        passwordSaveEnable->setChecked(false);
    }
    // Виджеты вставляются в группировщики
    QVBoxLayout* howPassRequestLayout = new QVBoxLayout;
    howPassRequestLayout->addWidget(howPassRequestRadio1);
    howPassRequestLayout->addWidget(howPassRequestRadio2);
    howPassRequestLayout->addWidget(passwordSaveEnable);
    howPassRequestBox->setLayout(howPassRequestLayout);

    QVBoxLayout* storePassRequestLayout = new QVBoxLayout;
    storePassRequestLayout->addWidget(passwordSaveEnable);
    storePassRequestLayout->addWidget(passwordSaveAnnotation);
    storePassRequestBox->setLayout(storePassRequestLayout);

    // Группировщик настойки автозакрытия окна пароля
    autoClosePasswordBox = new QGroupBox(this);
    autoClosePasswordBox->setTitle(tr("Auto closing password window"));

    autoClosePasswordEnable = new QCheckBox(tr("Enable auto closing password window, sec"), this);

    autoClosePasswordDelay = new QSpinBox(this);
    autoClosePasswordDelay->setValue(appconfig->auto_close_password_delay());
    autoClosePasswordDelay->setRange(1, 999);

    // Устанавливается галка и активность виджета выбора задержки
    onAutoClosePasswordEnableToggle(appconfig->auto_close_password_enable());

    // Виджеты вставляются в группировщик
    QHBoxLayout* autoClosePasswordLayout = new QHBoxLayout;
    autoClosePasswordLayout->addWidget(autoClosePasswordEnable);
    autoClosePasswordLayout->addWidget(autoClosePasswordDelay);
    autoClosePasswordLayout->addStretch();
    autoClosePasswordBox->setLayout(autoClosePasswordLayout);

    // азрешать ли расшифровывать зашифрованные файлы аттачей в каталог корзины при предпросмотре
    decryptFileToTrashDirectoryBox = new QGroupBox(this);
    decryptFileToTrashDirectoryBox->setTitle(tr("File decrypt method for attach preview"));

    decryptFileToTrashDirectoryEnable = new QCheckBox(tr("Enable temporary decrypt attach file to trash directory"));

    // Устанавливается галка согласно настройке из файла конфигурации
    decryptFileToTrashDirectoryEnable->setChecked(appconfig->enable_decrypt_file_to_trash_directory());

    // Виджеты вставляются в группировщик
    QHBoxLayout* decryptFileToTrashDirectoryLayout = new QHBoxLayout;
    decryptFileToTrashDirectoryLayout->addWidget(decryptFileToTrashDirectoryEnable);
    decryptFileToTrashDirectoryBox->setLayout(decryptFileToTrashDirectoryLayout);
}

void AppConfigPageCrypt::update_ui(void)
{
    // Строка состояния пароля
    passRetrieveStatus->setText(getRetrieveStatusText());

    // Кнопка запроса введения пароля
    passRetrieveButton->setText(getRetrieveButtonText());

    // Строка с аннотацией
    passRetrieveAnnotation->setText(getRetrieveAnnotationText());
}

void AppConfigPageCrypt::setup_signals(void)
{
    // При нажатии кнопки работы с паролем
    connect(passRetrieveButton, &QPushButton::clicked, this, &AppConfigPageCrypt::onPassRetrieveButtonClicked);

    // При клике на галку разрешения автозакрытия пароля
    connect(autoClosePasswordEnable, &QCheckBox::toggled, this, &AppConfigPageCrypt::onAutoClosePasswordEnableToggle);

    connect(passwordSaveEnable, &QCheckBox::toggled, this, &AppConfigPageCrypt::onPasswordSaveEnableToggle);
}

void AppConfigPageCrypt::assembly(void)
{
    // Собирается основной слой
    QVBoxLayout* central_layout = new QVBoxLayout();
    central_layout->addWidget(passRetrieveBox);
    central_layout->addWidget(howPassRequestBox);
    central_layout->addWidget(storePassRequestBox);
    central_layout->addWidget(autoClosePasswordBox);
    central_layout->addWidget(decryptFileToTrashDirectoryBox);
    central_layout->addStretch();

    // Основной слой устанавливается
    setLayout(central_layout);
}

QString AppConfigPageCrypt::getRetrieveStatusText(void)
{
    QString status = tr("<b>Status:</b> ");
    // Если в хранилище данных вообще не задан пароль
    if (databaseconfig->get_crypt_mode() == 0)
        status = status + tr("No password is set. ");
    else
        status = status + tr("Password is set. ");
    // Если пароль (точнее хеш пароля) хранится локально
    if (appconfig->password_save_flag() && appconfig->password_middle_hash().length() > 0)
        status = status + tr("Password is saved locally. ");
    return status;
}

QString AppConfigPageCrypt::getRetrieveButtonText(void)
{
    // Если в хранилище данных вообще не задан пароль
    if (databaseconfig->get_crypt_mode() == 0)
        return tr("Set a password");
    else
        return tr("Change password");
}

QString AppConfigPageCrypt::getRetrieveAnnotationText(void)
{
    // Если в хранилище данных вообще не задан пароль
    if (databaseconfig->get_crypt_mode() == 0)
        return tr("A password will be used to encrypt the item that you selected. Use \"Encrypt item\" or \"Decrypt item\" in context menu.");
    else
        return tr("If you change your password all encrypted item will be re-encrypted with a new password.");
}

// Действия при нажатии кнопки работы с паролем
void AppConfigPageCrypt::onPassRetrieveButtonClicked(void)
{
    // Если в хранилище данных вообще не задан пароль
    if (databaseconfig->get_crypt_mode() == 0) {
        // Включается диалог запроса пароля "с нуля"

        Password password;
        if (password.retrievePassword() == false)
            return;
        update_ui();

        return;
    } else {
        // Включается диалог изменения пароля
        Password password;

        password.replacePassword();
    }
}

void AppConfigPageCrypt::onPasswordSaveEnableToggle(bool checked)
{
    if (checked) {
        howPassRequestRadio1->setEnabled(false);
        howPassRequestRadio2->setEnabled(false);
    } else {
        howPassRequestRadio1->setEnabled(true);
        howPassRequestRadio2->setEnabled(true);
    }
}

// Действие при клике на галку настройки автоматического закрытия окна пароля
void AppConfigPageCrypt::onAutoClosePasswordEnableToggle(bool checked)
{
    // Устанавливается галка и активность виджета выбора задержки
    if (checked) {
        autoClosePasswordEnable->setChecked(true);
        autoClosePasswordDelay->setEnabled(true);
    } else {
        autoClosePasswordEnable->setChecked(false);
        autoClosePasswordDelay->setEnabled(false);
    }
}

// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPageCrypt::apply_changes(void)
{
    qDebug() << "Apply changes crypt";
    if (howPassRequestRadio1->isChecked() && appconfig->howpassrequest() == "atStartProgram")
        appconfig->howpassrequest("atClickOnCryptBranch");
    if (howPassRequestRadio2->isChecked() && appconfig->howpassrequest() == "atClickOnCryptBranch")
        appconfig->howpassrequest("atStartProgram");
    if (autoClosePasswordEnable->isChecked() != appconfig->auto_close_password_enable())
        appconfig->auto_close_password_enable(autoClosePasswordEnable->isChecked());
    if (autoClosePasswordDelay->value() != appconfig->auto_close_password_delay())
        appconfig->auto_close_password_delay(autoClosePasswordDelay->value());
    if (passwordSaveEnable->isChecked() != appconfig->password_save_flag()) {
        appconfig->password_save_flag(passwordSaveEnable->isChecked());
        // Если галка установлена что хранить локально пароль ненужно
        if (!passwordSaveEnable->isChecked()) {
            // Промежуточный хеш пароля удаляется
            appconfig->password_middle_hash("");
        }
    }
    if (decryptFileToTrashDirectoryEnable->isChecked() != appconfig->enable_decrypt_file_to_trash_directory())
        appconfig->enable_decrypt_file_to_trash_directory(decryptFileToTrashDirectoryEnable->isChecked());
    return 0;
}
